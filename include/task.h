/**
 * @file task.h
 *
 */

#ifndef G_TASK_H
#define G_TASK_H
/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>

#include "sched/workqueue.h"

/*********************
 *      DEFINES
 *********************/
#define NR_WORKQUEUE                    1
#define WORKERS_PER_QUEUE               2

#define UI_WQ                           0
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  GLOBAL PROTOTYPES
 **********************/
int32_t process_opcode(uint32_t opcode, void *data);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* G_TASK_H */
