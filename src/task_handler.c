#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <log.h>
#include <terminal-ui.h>
#include <workqueue.h>

extern volatile sig_atomic_t g_run;

void * main_task_handler(void* arg)
{
    work_t *w = NULL;

    LOG_INFO("Task handler is running...");
    while (g_run) {
        usleep(200000);
        LOG_DEBUG("Task handler is waiting for new task...");
        w = pop_work_wait();
        if (w == NULL) {
            LOG_INFO("Task handler is exiting...");
            break;
        }
        LOG_TRACE("Task: received opcode=%d, data=%s", w->opcode, w->data);

        sleep(1);
        LOG_TRACE("Task done: %s", w->data);

        free(w);
    };

    LOG_INFO("Task handler thread exiting...");
    return NULL;
}

