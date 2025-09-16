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
#include <log.h>

#include <stdlib.h>
#include <stdint.h>

#include <ui/ui.h>
#include <comm/dbus_comm.h>
#include <comm/cmd_payload.h>
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
int32_t process_opcode_endless(uint32_t opcode, void *data)
{
    int32_t ret = 0;

    switch (opcode) {
    case OP_START_DBUS:
        ret = dbus_fn_thread_handler();
        break;
    case OP_UI_START:
        ret = ui_refresh_thread();
        break;
    case OP_START_IMU:
        break;
    default:
        LOG_ERROR("Opcode [%d] is invalid", opcode);
        break;
    }

    return ret;
}

int32_t process_opcode(uint32_t opcode, void *data)
{
    int32_t ret = 0;

    switch (opcode) {
    case OP_UI_INIT:
        ret = ui_main_init();
        break;
    case OP_UI_STOP:
        break;
    case OP_DBUS_SENT_CMD_DATA:
        ret = dbus_method_call_with_data((remote_cmd_t *)data);
        break;
    case OP_LEFT_VIBRATOR:
        break;
    case OP_RIGHT_VIBRATOR:
        break;
    case OP_STOP_IMU:
        break;
    case OP_READ_IMU:
        break;
    default:
        LOG_ERROR("Opcode [%d] is invalid", opcode);
        break;
    }

    return ret;
}

