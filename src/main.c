/**
 * @file main.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
// #define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL defined locally will override the global setting in this file"
#endif
#include <log.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>

#include <lvgl.h>
#include <list.h>
#include <ui/ui_core.h>
#include <ui/fonts.h>
#include <ui/ui.h>
#include <comm/cmd_payload.h>
#include <comm/f_comm.h>
#include <sched/workqueue.h>
#include <sched/task.h>

/*********************
 *      DEFINES
 *********************/
#define UI_LVGL_TIMER_MS                5
#define UI_SCR_WIDTH                    1024
#define UI_SCR_HEIGHT                   600

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/
extern int32_t event_fd;
volatile sig_atomic_t g_run = 1;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_display_t *drm_disp = NULL;
static lv_indev_t *touch_scr = NULL;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void sig_handler(int32_t sig) {
    switch (sig) {
        case SIGINT:
            LOG_WARN("[+] Received SIGINT (Ctrl+C). Exiting...");
            g_run = 0;
            event_set(event_fd, SIGINT);
            workqueue_stop();
            break;
        case SIGTERM:
            LOG_WARN("[+] Received SIGTERM. Shutdown...");
            exit(0);
        case SIGABRT:
            LOG_WARN("[+] Received SIGABRT. Exiting...");
            event_set(event_fd, SIGABRT);
            break;
        default:
            LOG_WARN("[!] Received unidentified signal: %d", sig);
            break;
    }
}

static int32_t setup_signal_handler()
{
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        LOG_ERROR("Error registering signal SIGINT handler");
        return -1;
    }

    if (signal(SIGTERM, sig_handler) == SIG_ERR) {
        LOG_ERROR("Error registering signal SIGTERM handler");
        return -1;
    }

    if (signal(SIGABRT, sig_handler) == SIG_ERR) {
        LOG_ERROR("Error registering signal SIGABRT handler");
        return -1;
    }

    return 0;
}

static int32_t sf_init_drm_display() {
    drm_disp = lv_linux_drm_create();
    if (drm_disp == NULL) {
        LOG_FATAL("Failed to initialize the display.\n");
        return LV_RESULT_INVALID;
    }
    lv_display_set_default(drm_disp);
    lv_linux_drm_set_file(drm_disp, "/dev/dri/card0", 32);
    lv_display_set_resolution(drm_disp, UI_SCR_WIDTH, UI_SCR_HEIGHT);
    return 0;
}

static int32_t sf_init_touch_screen() {
    touch_scr = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/event1");
    if (touch_scr == NULL) {
        LOG_FATAL("Failed to initialize touch input device");
        return LV_RESULT_INVALID;
    }
    lv_indev_set_display(touch_scr, drm_disp);
    return 0;
}

static void gtimer_handler(lv_timer_t * timer)
{
    lv_tick_inc(UI_LVGL_TIMER_MS);
}

static int32_t main_loop()
{
    uint32_t cnt = 0;

    LOG_INFO("Terminal UI service is running...");
    while (g_run) {
        lv_task_handler();
        usleep(5000);
        if (++cnt == 20) {
            cnt = 0;
            is_task_handler_idle();
        }
    };

    LOG_INFO("Terminal UI service is exiting...");
    return 0;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int32_t main(void) {
    pthread_t task_handler;
    lv_timer_t *task_timer = NULL;
    int32_t ret = 0;
    g_ctx_t *ctx = NULL;

    LOG_INFO("|-----------------------> TERMINAL UI <-----------------------|");
    if (setup_signal_handler()) {
        goto exit_error;
    }

    ret = pthread_create(&task_handler, NULL, main_task_handler, NULL);
    if (ret) {
        LOG_FATAL("Failed to create worker thread: %s", strerror(ret));
        goto exit_error;
    }

    // Prepare eventfd to notify epoll when communicating with a thread
    ret = init_event_file();
    if (ret) {
        LOG_FATAL("Failed to initialize eventfd");
        goto exit_workqueue;
    }

    create_local_simple_task(NON_BLOCK, ENDLESS, OP_START_DBUS);

    ctx = gf_create_app_ctx();
    gf_set_app_ctx(ctx);

    // Initialize LVGL and the associated UI hardware
    lv_init();
    sf_init_drm_display();
    sf_init_touch_screen();

    task_timer = lv_timer_create(gtimer_handler, UI_LVGL_TIMER_MS,  NULL);
    if (task_timer == NULL) {
        LOG_FATAL("Failed to create timer for LVGL task handler");
        goto exit_listener;
    }
    lv_timer_ready(task_timer);

    // Initialize LVGL layers as base components
    gf_register_obj(NULL, lv_layer_sys(), ID_LAYER_SYS);
    gf_register_obj(NULL, lv_layer_top(), ID_LAYER_TOP);
    gf_register_obj(NULL, lv_screen_active(), ID_LAYER_ACT);
    gf_register_obj(NULL, lv_layer_bottom(), ID_LAYER_BOT);

    LOG_INFO("sizeof g: %d", sizeof(g_obj_t));
    create_dynamic_ui();
    // Terminal-UI's primary tasks are executed within a loop
    ret = main_loop();
    if (ret) {
        goto exit_listener;
    }

    pthread_join(task_handler, NULL);

    gf_destroy_app_ctx(gf_get_app_ctx());
    cleanup_event_file();

    LOG_INFO("|-------------> All services stopped. Safe exit <-------------|");
    return 0;

exit_listener:
    event_set(event_fd, SIGUSR1);

exit_workqueue:
    workqueue_stop();
    pthread_join(task_handler, NULL);
    cleanup_event_file();

exit_error:
    return -1;
}
