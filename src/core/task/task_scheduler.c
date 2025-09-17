/**
 * @file task_scheduler.c
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

#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdatomic.h>
#include <stdbool.h>

#include <comm/dbus_comm.h>
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
extern volatile sig_atomic_t g_run;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
static int32_t create_blocking_task(work_t *w)
{
    int32_t ret = 0;

    LOG_TRACE("TASK: [%d:%d:%d:%d] is started", \
              w->type, w->flow, w->duration, w->opcode);

    ret = process_opcode(w->opcode, w->data);

    // TODO: Handle work done notification
    LOG_TRACE("TASK: [%d:%d:%d:%d] is completed - return %d", \
              w->type, w->flow, w->duration, w->opcode, ret);

    return ret;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void *workqueue_handler(void* arg)
{
    work_t *w = NULL;
    int32_t ret = 0;
    pthread_t tid = pthread_self();
    LOG_INFO("Workqueue handler started - thread ID: %lu", (unsigned long)tid);

    // LOG_INFO("Task handler is running...");
    while (g_run) {
        // remove sleep to handle parallel tasks faster after request
        // usleep(200000);
        LOG_TRACE("Workqueue handler ID [%lu] --> waiting for new task...", \
                  (unsigned long)tid);
        w = pop_work_wait_safe();
        /*
         * After a work item is popped from the workqueue, it is no longer
         * linked to the work list. This means:
         *
         *   For serial (single-threaded) work: the work structure can also be
         *   freed immediately after the task is processed, as no other entity
         *   can access it.
         *
         * NOTE: Do not free the work item if it is expected to be re-queued or
         *       if any other component may still hold a reference to it.
         */
        if (w == NULL) {
            LOG_INFO("Workqueue handler ID [%lu] is exiting...", \
                     (unsigned long)tid);
            break;
        }

        LOG_TRACE("Handler ID [%lu] type: [%d] - flow [%d] - opcode [%d]", \
                  (unsigned long)tid, w->type, w->flow, w->opcode);

        if (w->flow == BLOCK) {
            /*
             * Run blocking task; return after it completes other tasks in
             * queue wait until it's done. or will be handled by another handler
             */
            ret = create_blocking_task(w);
            if (ret) {
                LOG_ERROR("Handler ID [%lu] task failed with ret=%d",
                          (unsigned long)tid, ret);
            }
            // The working data structures for any tasks need to be freed
            delete_work(w);
        } else if (w->flow == NON_BLOCK) {
            LOG_WARN("Non-blocking task not supported");
            delete_work(w);
        } else {
            LOG_WARN("Invalid task specification:\n" \
                     "\tOpcode [%d]\n" \
                     "\tFlow [%d]\n" \
                     "\tDuration [%d]", \
                     w->opcode, w->flow, w->duration);
        }
    };

    LOG_INFO("Workqueue handler ID [%lu] exited...", (unsigned long)tid);

    return NULL;
}

