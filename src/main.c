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
#include <log.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

#include <ui/ui.h>
#include <comm/cmd_payload.h>
#include <comm/f_comm.h>
#include <sched/workqueue.h>
#include <sched/task.h>

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

static int32_t service_startup_flow(void)
{
    int32_t ret;

    ret = create_local_simple_task(NON_BLOCK, ENDLESS, OP_START_DBUS);
    if (ret) {
        LOG_WARN("Failed to create local task: start DBus service");
        // TODO: Show popup or alert to notify user about this issue
    }

    ret = create_local_simple_task(BLOCK, SHORT, OP_UI_INIT);
    if (ret) {
        LOG_FATAL("Failed to create local task: UI init");
        return -EIO;
    }

    /*
     * This non-blocking task runs in the background but still waits for
     * previous tasks to complete. This is expected in the current context.
     * However, with multiple task handlers, a remote task might need to run
     * while a long-running local task is still in progress.
     */
    ret = create_local_simple_task(NON_BLOCK, ENDLESS, OP_UI_START);
    if (ret) {
        LOG_FATAL("Failed to create local task: UI refresh");
        return -EIO;
    }

    ret = create_remote_simple_task(BLOCK, SHORT, OP_BACKLIGHT_ON);
    if (ret) {
        LOG_ERROR("Failed to create remote task: backlight on");
        return -EIO;
    }

    return 0;
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

    /* Step 1: Turn off backlight via remote task */
    ret = create_remote_simple_task(BLOCK, LONG, OP_BACKLIGHT_OFF);
    if (ret) {
        LOG_ERROR("Failed to create remote task: backlight off");
        return;
    }

    /* Step 2: Wait for all normal tasks to complete */
    cnt = normal_task_cnt_get();
    while (cnt) {
        LOG_TRACE("Waiting for normal works, remaining work %d", cnt);
        usleep(100000);
        cnt = normal_task_cnt_get();
    }

    /* Step 3: Stop endless tasks and notify shutdown */
    g_run = 0;                      /* Signal threads to stop */
    workqueue_handler_wakeup();     /* Wake up any waiting workqueue threads */
    event_set(event_fd, SIGINT);    /* Notify DBus/system about shutdown */

    /* Step 4: Wait until workqueue is fully drained */
    cnt = workqueue_active_count();
    while (cnt) {
        LOG_TRACE("Waiting for workqueue to be free, remaining work %d", cnt);
        usleep(100000);
        cnt = workqueue_active_count();
    }

    LOG_INFO("Service shutdown flow completed");
}

static int32_t main_loop()
{
    uint32_t cnt = 0;

    LOG_INFO("Terminal UI service is running...");
    while (g_run) {
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
int32_t main(void)
{
    pthread_t task_handler;
    int32_t ret = 0;

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
        LOG_FATAL("Failed to initialize eventfd, ret %d", ret);
        goto exit_workqueue;
    }

    ret = service_startup_flow();
    if (ret) {
        LOG_FATAL("UI system init flow failed, ret=%d", ret);
        goto exit_listener;
    }

    // Terminal-UI's primary tasks are executed within a loop
    ret = main_loop();
    if (ret) {
        goto exit_listener;
    }

    pthread_join(task_handler, NULL);
    cleanup_event_file();

    LOG_INFO("|-------------> All services stopped. Safe exit <-------------|");
    return 0;

exit_listener:
    event_set(event_fd, SIGUSR1);

exit_workqueue:
    workqueue_handler_wakeup();
    pthread_join(task_handler, NULL);
    cleanup_event_file();

exit_error:
    return -1;
}
