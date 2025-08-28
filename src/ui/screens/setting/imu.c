/**
 * @file imu.c
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include <lvgl.h>
#include <ui/ui_plat.h>
#include <ui/fonts.h>
#include <ui/style.h>
#include <comm/dbus_comm.h>
#include <comm/cmd_payload.h>
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
int32_t create_imu_comm_cmd(int32_t opcode)
{
    remote_cmd_t *cmd;
    int32_t ret = 0;

    cmd = create_remote_cmd();
    if (!cmd) {
        return -ENOMEM;
    }

    if (opcode == OP_START_IMU) {
        remote_cmd_init(cmd, COMP_NAME, 1001, opcode, NON_BLOCK, ENDLESS);
    } else {
        remote_cmd_init(cmd, COMP_NAME, 1001, opcode, NON_BLOCK, SHORT);
    }

    /* Add parameters */
    if (remote_cmd_add_string(cmd, "imu", "REQ")) {
        ret = -EINVAL;
        goto out;
    }

    // NOTE: Command data will be released after the work completes
    ret = create_remote_task(BLOCK, cmd);
    return ret;

out:
    delete_remote_cmd(cmd);
    return ret;
}

