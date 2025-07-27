#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <log.h>
#include <terminal-ui.h>
#include <workqueue.h>
#include <task_handler.h>
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

        if (w->type == REMOTE) {
            LOG_TRACE("REMOTE Task: received opcode=%d", ((remote_cmd_t *)w->data)->opcode);
        } else if (w->type == LOCAL) {
            LOG_TRACE("LOCAL Task: received opcode=%d", ((local_cmd_t *)w->data)->opcode);
        }

        if (w->flow == SERIAL) {
            LOG_TRACE("Task type: SERIAL");

// ######################### TEST

            if (((local_cmd_t *)w->data)->opcode == OP_ID_LEFT_VIBRATOR) {
                LOG_TRACE("Task: LEFT VIBR");
                rumble_trigger(2, 80, 150);
            } else if (((local_cmd_t *)w->data)->opcode == OP_ID_RIGHT_VIBRATOR) {
                LOG_TRACE("Task: RIGHT VIBR");
                rumble_trigger(3, 80, 150);
            }

// ######################### TEST
        } else if (w->flow == PARALLEL) {
            LOG_TRACE("Task type: PARALLEL");
        }

        LOG_INFO("#############################################");

        if (w->type == REMOTE) {
            LOG_TRACE("Task done: %d", ((remote_cmd_t *)w->data)->opcode);
        } else if (w->type == LOCAL) {
            LOG_TRACE("Task done: %d", ((local_cmd_t *)w->data)->opcode);
        }
        delete_work(w);
    };

    LOG_INFO("Task handler thread exiting...");
    return NULL;
}

