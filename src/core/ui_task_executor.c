/**
 * @file ui_task_executor.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
// #define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL=" TOSTRING(LOG_LEVEL) ", will take precedence in this file."
#endif
#include <log.h>

#include <stdlib.h>
#include <stdint.h>

#include <dbus_comm.h>
#include <cmd_payload.h>

#include <workqueue.h>
#include <task.h>

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
    int32_t rc = 0;

    switch (opcode) {
    case OP_START_DBUS:
        rc = dbus_fn_thread_handler();
        break;
    case OP_START_IMU:
        break;
    default:
        LOG_ERROR("Opcode [%d] is invalid");
        break;
    }

    return rc;
}

int32_t process_opcode(uint32_t opcode, void *data)
{
    int32_t rc = 0;

    switch (opcode) {
    case OP_DBUS_SENT_CMD_DATA:
        rc = send_remote_cmd((remote_cmd_t *)data);
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
        LOG_ERROR("Opcode [%d] is invalid");
        break;
    }

    return rc;
}

int32_t create_local_simple_task(uint8_t flow, uint8_t duration, uint32_t opcode)
{
    work_t *work = create_work(LOCAL, flow, duration, opcode, NULL);
    if (!work) {
        LOG_ERROR("Failed to create work from cmd");
        return EXIT_FAILURE;
    }

    push_work(work);

    return EXIT_SUCCESS;
}

int32_t create_remote_task(uint8_t flow, void *data)
{
    work_t *work;

    work = create_work(REMOTE, flow, SHORT, OP_DBUS_SENT_CMD_DATA, data);
    if (!work) {
        LOG_ERROR("Failed to create work from cmd");
        return EXIT_FAILURE;
    }

    push_work(work);

    return EXIT_SUCCESS;
}
