/**
 * @file task_exec.c
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

#include "ui/ui.h"
#include "comm/dbus_comm.h"
#include "comm/cmd_payload.h"
#include "sched/workqueue.h"
#include "task.h"

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
int32_t process_opcode(uint32_t opcode, void *data)
{
    int32_t ret = 0;

    switch (opcode) {
    case OP_DBUS_SENT_CMD:
        ret = dbus_method_call_with_data((remote_cmd_t *)data);
        break;
    default:
        LOG_ERROR("Opcode [%d] is invalid", opcode);
        break;
    }

    return ret;
}

