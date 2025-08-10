#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <log.h>
#include <terminal-ui.h>
#include <workqueue.h>
#include <task_handler.h>
#include <dbus_comm.h>

extern volatile sig_atomic_t g_run;

void *non_blocking_task_handler(void *arg)
{
    work_t *w = (work_t *)arg;

    local_cmd_t *local_data = NULL;
    remote_cmd_t *remote_data = NULL;

    if (w->type == LOCAL) {
        LOG_TRACE("start LOCAL task: opcode [%d]", \
                  ((local_cmd_t *)w->data)->opcode);
        local_data = (local_cmd_t *)w->data;
    } else if (w->type == REMOTE) {
        LOG_TRACE("start REMOTE task: opcode [%d]", \
                  ((remote_cmd_t *)w->data)->opcode);
        remote_data = (remote_cmd_t *)w->data;
    }

    // The working data structures for ENDLESS task need to be freed
    if (w->duration == ENDLESS) {
        delete_work(w);
    }

    // TESTING: TODO: opcode parser
    if (local_data->opcode == OP_ID_LEFT_VIBRATOR) {
        rumble_trigger(2, 80, 150);
    } else if (local_data->opcode == OP_ID_RIGHT_VIBRATOR) {
        rumble_trigger(3, 80, 150);
    }

    // TODO: Handle work done notification

    // The working data structures for the ENDLESS task are freed.
    if (w->duration != ENDLESS) {
        delete_work(w);
    }
}

int create_non_blocking_task_handler(work_t *w)
{
    pthread_t thread_id;
    int ret;

    ret = pthread_create(&thread_id, NULL, non_blocking_task_handler, w);
    if (ret) {
        LOG_FATAL("Failed to create worker thread: %s", strerror(ret));
        return ret;
    } else {
        pthread_detach(thread_id);
    }

    return EXIT_SUCCESS;
}

int create_blocking_task_handler()
{
    return 0;
}

void *main_task_handler(void* arg)
{
    work_t *w = NULL;

    LOG_INFO("Task handler is running...");
    while (g_run) {
        // remove sleep to handle parallel tasks faster after request
        // usleep(200000);
        LOG_TRACE("[Task handler] --> waiting for new task...");
        w = pop_work_wait();
        /*
         * After a work item is popped from the workqueue, it is no longer linked
         * to the work list. This means:
         *
         * - For non-blocking work: the worker thread can safely free the work
         *   structure after the task completes, since no other references exist.
         *
         * - For serial (single-threaded) work: the work structure can also be freed
         *   immediately after the task is processed, as no other entity can access it.
         *
         * NOTE: Do not free the work item if it is expected to be re-queued or if
         * any other component may still hold a reference to it.
         */
        if (w == NULL) {
            LOG_INFO("Task handler is exiting...");
            break;
        }

        LOG_TRACE("Task type: [%d] - flow [%d] - opcode [%d]", w->type, w->flow, \
                  w->type == REMOTE ? ((remote_cmd_t *)w->data)->opcode : \
                  ((local_cmd_t *)w->data)->opcode);

        if (w->flow == BLOCK) {
            create_blocking_task_handler();
            // the work struct must be deleted after use
            delete_work(w);
        } else if (w->flow == NON_BLOCK) {
            create_non_blocking_task_handler(w);
        }
    };

    LOG_INFO("Task handler thread exiting...");
    return NULL;
}

