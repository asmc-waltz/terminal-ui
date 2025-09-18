/**
 * @file haptic.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL defined locally will override the global setting in this file"
#endif
#include "log.h"

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <stdbool.h>

#include "ux/ux.h"
#include "comm/dbus_comm.h"
#include "comm/cmd_payload.h"
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
int32_t haptic_feedback(bool en_left, bool en_right)
{
    remote_cmd_t *cmd;
    int32_t ret = 0;

    cmd = create_remote_cmd();
    if (!cmd) {
        return -ENOMEM;
    }

    if (en_left) {
        remote_cmd_init(cmd, COMP_NAME, COMP_ID, OP_LEFT_VIBRATOR, \
                        WORK_PRIO_NORMAL, WORK_DURATION_SHORT);
    }

    if (en_right) {
        remote_cmd_init(cmd, COMP_NAME, COMP_ID, OP_RIGHT_VIBRATOR, \
                        WORK_PRIO_NORMAL, WORK_DURATION_SHORT);
    }

    // NOTE: Command data will be released after the work completes
    ret = create_remote_task(WORK_PRIO_HIGH, cmd);
    return ret;

out:
    delete_remote_cmd(cmd);
    return ret;
}
