/**
 * @file workqueue.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
// #define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL defined locally will override the global setting in this file"
#endif
#include "log.h"

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>

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

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
workqueue_t *workqueue_create(void)
{
    workqueue_t *wq;

    wq = calloc(1, sizeof(*wq));
    if (!wq)
        return NULL;

    INIT_LIST_HEAD(&wq->list);
    pthread_mutex_init(&wq->mutex, NULL);
    pthread_cond_init(&wq->cond, NULL);
    atomic_store(&wq->active_cnt, 0);

    return wq;
}

void workqueue_destroy(workqueue_t *wq)
{
    work_t *pos, *n;

    if (!wq)
        return;

    pthread_mutex_lock(&wq->mutex);
    list_for_each_entry_safe(pos, n, &wq->list, node) {
        list_del(&pos->node);
        free(pos->data);
        free(pos);
    }
    pthread_mutex_unlock(&wq->mutex);

    pthread_mutex_destroy(&wq->mutex);
    pthread_cond_destroy(&wq->cond);
    free(wq);
}

work_t *create_work(uint8_t type, uint8_t priority, uint8_t duration, \
                    uint32_t opcode, void *data)
{
    work_t *w;

    w = calloc(1, sizeof(*w));
    if (!w)
        return NULL;

    w->type = type;
    w->prio = priority;
    w->duration = duration;
    w->opcode = opcode;
    w->data = data;
    INIT_LIST_HEAD(&w->node);

    LOG_TRACE("Created work for opcode: %d", w->opcode);
    return w;
}

void push_work(workqueue_t *wq, work_t *w)
{
    pthread_mutex_lock(&wq->mutex);
    list_add_tail(&w->node, &wq->list);
    atomic_fetch_add(&wq->active_cnt, 1);
    pthread_cond_signal(&wq->cond);
    pthread_mutex_unlock(&wq->mutex);
}

work_t *pop_work_wait_safe(workqueue_t *wq)
{
    work_t *w = NULL;

    pthread_mutex_lock(&wq->mutex);
    while (list_empty(&wq->list) && g_run)
        pthread_cond_wait(&wq->cond, &wq->mutex);

    if (list_empty(&wq->list)) {
        pthread_mutex_unlock(&wq->mutex);
        return NULL;
    }

    w = list_first_entry(&wq->list, work_t, node);
    list_del(&w->node);

    pthread_mutex_unlock(&wq->mutex);
    return w;
}

void workqueue_complete_work(workqueue_t *wq, work_t *w)
{
    if (!w)
        return;

    if (w->data)
        free(w->data);
    free(w);

    atomic_fetch_sub(&wq->active_cnt, 1);
    if (atomic_load(&wq->active_cnt) == 0) {
        pthread_mutex_lock(&wq->mutex);
        pthread_cond_broadcast(&wq->cond);
        pthread_mutex_unlock(&wq->mutex);
    }
}

int32_t workqueue_active_count(workqueue_t *wq)
{
    return atomic_load(&wq->active_cnt);
}

void workqueue_handler_wakeup(workqueue_t *wq)
{
    pthread_mutex_lock(&wq->mutex);
    pthread_cond_broadcast(&wq->cond);
    pthread_mutex_unlock(&wq->mutex);
}
