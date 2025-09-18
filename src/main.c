/**
 * @file main.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL defined locally will override the global setting in this file"
#endif
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

#include "ui/ui.h"
#include "comm/cmd_payload.h"
#include "comm/f_comm.h"
#include "comm/dbus_comm.h"
#include "sched/workqueue.h"
#include "sched/task.h"

/*********************
 *      DEFINES
 *********************/

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
static void service_shutdown_flow();

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void sig_handler(int32_t sig)
{
    switch (sig) {
        case SIGINT:
            LOG_WARN("[+] Received SIGINT (Ctrl+C). Exiting...");
            service_shutdown_flow();
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
        return -EIO;
    }

    if (signal(SIGTERM, sig_handler) == SIG_ERR) {
        LOG_ERROR("Error registering signal SIGTERM handler");
        return -EIO;
    }

    if (signal(SIGABRT, sig_handler) == SIG_ERR) {
        LOG_ERROR("Error registering signal SIGABRT handler");
        return -EIO;
    }

    return 0;
}

// TODO: create thread pool
    pthread_t task_pool_0;
    pthread_t task_pool_1;

static int32_t service_startup_flow(void)
{
    int32_t ret;
    pthread_t dbus_handler;
    workqueue_t *wq = NULL;

    /* Initialize UI */
    ret = ui_main_init();
    if (ret) {
        LOG_FATAL("Failed to initialize UI, ret=%d", ret);
        return ret;
    }

    /* Prepare eventfd to notify epoll when communicating with threads */
    ret = init_event_file();
    if (ret) {
        LOG_FATAL("Failed to initialize eventfd, ret=%d", ret);
        goto exit_ui;
    }

    wq = workqueue_create();
    if (!wq) {
        LOG_FATAL("Unable to create workqueue");
        return -ENOMEM;
    }

    set_ui_wq(wq);

    /* Create main task handler thread */
    ret = pthread_create(&task_pool_0, NULL, workqueue_handler, get_ui_wq());
    if (ret) {
        LOG_FATAL("Failed to create worker thread: %s", strerror(ret));
        goto exit_event;
    }

    /* Create main task handler thread */
    ret = pthread_create(&task_pool_1, NULL, workqueue_handler, get_ui_wq());
    if (ret) {
        LOG_FATAL("Failed to create worker thread: %s", strerror(ret));
        goto exit_event;
    }

    /* Create DBus listener thread */
    ret = pthread_create(&dbus_handler, NULL, dbus_fn_thread_handler, NULL);
    if (ret) {
        LOG_FATAL("Failed to create DBus listener thread: %s", strerror(ret));
        goto exit_workqueue;
    }

    /* Optional delay before sending remote task */
    usleep(200000);

    /* Turn on backlight via remote task */
    ret = create_remote_simple_task(WORK_PRIO_NORMAL, WORK_DURATION_SHORT, \
                                    OP_BACKLIGHT_ON);
    if (ret) {
        LOG_ERROR("Failed to create remote task: backlight on");
        goto exit_dbus;
    }

    LOG_INFO("Terminal UI initialization completed");
    return 0;

/* Cleanup sequence in case of failure */
exit_dbus:
    event_set(event_fd, SIGUSR1);

exit_workqueue:
    workqueue_handler_wakeup(get_ui_wq());
    pthread_join(task_pool_0, NULL);
    pthread_join(task_pool_1, NULL);

exit_event:
    cleanup_event_file();

exit_ui:
    ui_main_deinit();
    return ret;
}

/**
 * Gracefully shutdown the system services
 *
 * This function ensures all normal tasks are finished, then stops
 * endless tasks and notifies system and DBus about shutdown.
 */
static void service_shutdown_flow(void)
{
    int32_t ret;
    int32_t cnt;

    /* Turn off backlight via remote task */
    ret = create_remote_simple_task(WORK_PRIO_NORMAL, WORK_DURATION_LONG, \
                                    OP_BACKLIGHT_OFF);
    if (ret) {
        LOG_ERROR("Failed to create remote task: backlight off");
        return;
    }

    /* Wait until workqueue is fully drained */
    cnt = workqueue_active_count(get_ui_wq());
    while (cnt) {
        LOG_TRACE("Waiting for workqueue to be free, remaining work %d", cnt);
        usleep(100000);
        cnt = workqueue_active_count(get_ui_wq());
    }

    /* Stop background threads and notify shutdown */
    g_run = 0;                      /* Signal threads to stop */
    event_set(event_fd, SIGINT);    /* Notify DBus/system about shutdown */
    workqueue_handler_wakeup(get_ui_wq());     /* Wake up any waiting workqueue threads */

    // TODO:
    pthread_join(task_pool_0, NULL);
    pthread_join(task_pool_1, NULL);

    workqueue_destroy(get_ui_wq());

    cleanup_event_file();
    ui_main_deinit();

    LOG_INFO("Service shutdown flow completed");
}

static int32_t main_loop()
{
    LOG_INFO("Terminal UI service is running...");
    while (g_run) {
        lv_task_handler();
        usleep(5000);
    };

    LOG_INFO("Terminal UI service is exiting...");
    return 0;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int32_t main(void)
{
    int32_t ret = 0;

    LOG_INFO("|-----------------------> TERMINAL UI <-----------------------|");
    ret = setup_signal_handler();
    if (ret) {
        return ret;
    }

    ret = service_startup_flow();
    if (ret) {
        LOG_FATAL("UI system init flow failed, ret=%d", ret);
        return ret;
    }

    ret = main_loop();
    if (ret) {
        return ret;
    }

    LOG_INFO("|-------------> All services stopped. Safe exit <-------------|");
    return 0;
}
