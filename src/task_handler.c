#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <log.h>
#include <terminal-ui.h>
#include <workqueue.h>
#include <task_handler.h>
#include <dbus_comm.h>

extern volatile sig_atomic_t g_run;

/*
 * The non-blocking task will be started by the task handler and run in the
 * background. Depending on the type of work, it could have a short, long,
 * or endless duration. All such tasks must be controlled by g_run, which
 * is also known as the common exit flag for the system.
 */
static void *non_blocking_task_thread(void *arg)
{
    work_t *w = (work_t *)arg;
    int rc = 0;

    LOG_TRACE("TASK: [%d:%d:%d:%d] is started", \
              w->type, w->flow, w->duration, w->opcode);

    if (w->duration == ENDLESS) {
        rc = process_opcode_endless(w->opcode, NULL);
    } else {
        rc = process_opcode(w->opcode, w->data);
    }

    // TODO: Handle work done notification
    LOG_TRACE("TASK: [%d:%d:%d:%d] is completed - return %d", \
              w->type, w->flow, w->duration, w->opcode, rc);

    // Free working data structures for non-blocking tasks.
    delete_work(w);
}

static int create_non_blocking_task(work_t *w)
{
    pthread_t thread_id;
    int ret;

    ret = pthread_create(&thread_id, NULL, non_blocking_task_thread, w);
    if (ret) {
        LOG_FATAL("Failed to create worker thread: %s", strerror(ret));
        return ret;
    } else {
        pthread_detach(thread_id);
    }

    return EXIT_SUCCESS;
}

static int create_blocking_task(work_t *w)
{
    int rc = 0;

    LOG_TRACE("TASK: [%d:%d:%d:%d] is started", \
              w->type, w->flow, w->duration, w->opcode);

    rc = process_opcode(w->opcode, w->data);

    // TODO: Handle work done notification
    LOG_TRACE("TASK: [%d:%d:%d:%d] is completed - return %d", \
              w->type, w->flow, w->duration, w->opcode, rc);

    // The working data structures for normal tasks need to be freed
    delete_work(w);

    return rc;
}

void *main_task_handler(void* arg)
{
    work_t *w = NULL;
    int rc = 0;

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

        LOG_TRACE("Task type: [%d] - flow [%d] - opcode [%d]", w->type, \
                  w->flow, w->opcode);

        if (w->flow == BLOCK) {
            // run blocking task; return after it completes
            // other tasks in queue wait until it's done
            rc = create_blocking_task(w);
        } else if (w->flow == NON_BLOCK) {
            // create a thread to handle requests in the background
            // the function returns immediately after the thread is created
            rc = create_non_blocking_task(w);
        }
    };

    LOG_INFO("Task handler thread exiting...");
    return NULL;
}

