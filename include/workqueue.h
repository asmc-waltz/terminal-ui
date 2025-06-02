/**
 * @file comm.h
 *
 */

#ifndef G_WORKQUEUE_H
#define G_WORKQUEUE_H
/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct work {
    int opcode;
    char data[256];
    struct work *next;
} work_t;

typedef struct workqueue {
    work_t *head;
    work_t *tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
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
void push_work(work_t *work);
work_t* pop_work_wait();
void workqueue_stop();

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* G_WORKQUEUE_H */
