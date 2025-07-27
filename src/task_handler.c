#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <log.h>
#include <terminal-ui.h>
#include <workqueue.h>
#include <dbus_comm.h>

extern volatile sig_atomic_t g_run;

void * main_task_handler(void* arg)
{
    work_t *w = NULL;

    LOG_INFO("Task handler is running...");
    while (g_run) {
        usleep(200000);
        LOG_DEBUG("[Task handler] --> waiting for new task...");
        w = pop_work_wait();
        if (w == NULL) {
            LOG_INFO("Task handler is exiting...");
            break;
        }

        if (w->type == REMOTE_WORK) {
            LOG_TRACE("Task: received opcode=%d", ((remote_cmd_t *)w->data)->opcode);
        }

        LOG_INFO("#############################################");

        if (w->type == REMOTE_WORK) {
            LOG_TRACE("Task done: %d", ((remote_cmd_t *)w->data)->opcode);
        }
        delete_work(w);
    };

    LOG_INFO("Task handler thread exiting...");
    return NULL;
}

