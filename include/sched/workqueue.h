/**
 * @file workqueue.h
 *
 */

#ifndef G_WORKQUEUE_H
#define G_WORKQUEUE_H
/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <pthread.h>
#include <stdatomic.h>

#include "list.h"

/*********************
 *      DEFINES
 *********************/
typedef enum {
    WORK_TYPE_LOCAL = 0,
    WORK_TYPE_REMOTE,
} work_type_t;

typedef enum {
    WORK_PRIO_LOW = 0,
    WORK_PRIO_NORMAL,
    WORK_PRIO_HIGH,
    WORK_PRIO_URGENT,
} work_priority_t;

typedef enum {
    WORK_DURATION_SHORT = 0,
    WORK_DURATION_LONG,
} work_duration_t;

/**********************
 *      TYPEDEFS
 **********************/
typedef struct work {
    struct list_head node;
    work_type_t type;
    work_priority_t prio;
    work_duration_t duration;
    uint32_t opcode;
    void *data;
} work_t;

typedef struct workqueue {
    struct list_head list;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    atomic_int active_cnt;
} workqueue_t;

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*=====================
 * Setter functions
 *====================*/

/*=====================
 * Getter functions
 *====================*/

/*=====================
 * Other functions
 *====================*/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
workqueue_t *workqueue_create(void);
void workqueue_destroy(workqueue_t *wq);
work_t *create_work(uint8_t type, uint8_t priority, uint8_t duration, \
                    uint32_t opcode, void *data);
void workqueue_complete_work(workqueue_t *wq, work_t *w);
void push_work(workqueue_t *wq, work_t *w);
work_t *pop_work_wait_safe(workqueue_t *wq);
void workqueue_handler_wakeup(workqueue_t *wq);
int32_t workqueue_active_count(workqueue_t *wq);

void *workqueue_handler(void* arg);

workqueue_t *get_wq(int32_t index);
void set_wq(workqueue_t *wq, int32_t index);

int32_t workqueue_init();
int32_t workqueue_deinit();
/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* G_WORKQUEUE_H */
