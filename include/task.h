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

#include <log.h>

/*********************
 *      DEFINES
 *********************/

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
void normal_task_cnt_reset();
void normal_task_cnt_inc();
void normal_task_cnt_dec();
int32_t normal_task_cnt_get();
void endless_task_cnt_reset();
void endless_task_cnt_inc();
void endless_task_cnt_dec();
int32_t endless_task_cnt_get();

int process_opcode(uint32_t opcode, void *data);
int create_local_simple_task(uint8_t flow, uint8_t duration, uint32_t opcode);

bool is_task_handler_idle();
void * main_task_handler(void* arg);
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
