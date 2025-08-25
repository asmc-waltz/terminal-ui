/**
 * @file cmd_payload.c
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
#include <string.h>

#include <comm/dbus_comm.h>
#include <comm/cmd_payload.h>

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
remote_cmd_t *create_remote_cmd()
{
    remote_cmd_t *cmd;

    cmd = calloc(1, sizeof(*cmd));
    if (!cmd) {
        return NULL;
    }

    return cmd;
}

void delete_remote_cmd(remote_cmd_t *cmd)
{
    if (!cmd) {
        LOG_WARN("Unable to delete cmd: null pointer");
        return;
    }

    free(cmd);
}

local_cmd_t *create_local_cmd()
{
    local_cmd_t *cmd = NULL;

    cmd = calloc(1, sizeof(*cmd));
    if (!cmd) {
        return NULL;
    }

    return cmd;
}

void delete_local_cmd(local_cmd_t *cmd)
{
    if (!cmd) {
        LOG_WARN("Unable to delete cmd: null pointer");
        return;
    }

    free(cmd);
}

void remote_cmd_init(remote_cmd_t *cmd, const char *component_id, \
                     int32_t topic_id, int32_t opcode)
{
    int32_t i;

    cmd->component_id = component_id;
    cmd->topic_id = topic_id;
    cmd->opcode = opcode;
    cmd->entry_count = 0;

    for (i = 0; i < MAX_ENTRIES; i++) {
        cmd->entries[i].key = NULL;
        cmd->entries[i].data_type = 0;
        cmd->entries[i].data_length = 0;
    }
}

int32_t remote_cmd_add_string(remote_cmd_t *cmd, const char *key, const char *value)
{
    payload_t *entry;
    if (cmd->entry_count >= MAX_ENTRIES)
        return -1;

    entry = &cmd->entries[cmd->entry_count++];
    entry->key = key;
    entry->data_type = DBUS_TYPE_STRING;
    entry->data_length = strlen(value) + 1;;
    entry->value.str = value;

    return 0;
}

int32_t remote_cmd_add_int(remote_cmd_t *cmd, const char *key, int32_t value)
{
    payload_t *entry;

    if (cmd->entry_count >= MAX_ENTRIES)
        return -1;

    entry = &cmd->entries[cmd->entry_count++];
    entry->key = key;
    entry->data_type = DBUS_TYPE_INT32;
    entry->data_length = sizeof(int32_t);
    entry->value.i32 = value;

    return 0;
}

/*
 * This function sends a D-Bus method call to the system manager.
 * It operates without a specific callback for the reply message
 * because all responses are processed centrally in the D-Bus
 * listener thread rather than assigning per-call callbacks.
 */
int32_t send_remote_cmd(remote_cmd_t *cmd)
{
    int32_t ret;
    ret = dbus_method_call(SYS_MGR_DBUS_SER, SYS_MGR_DBUS_OBJ_PATH,
                           SYS_MGR_DBUS_IFACE, SYS_MGR_DBUS_METH, cmd);
    // TODO: Option
    // ret = dbus_emit_signal(UI_DBUS_OBJ_PATH, UI_DBUS_IFACE, \
    //                      UI_DBUS_SIG, cmd);
    if (ret) {
        LOG_FATAL("Failed to create method call message");
    }

    return ret;
}

/*
 * The DBus command will be sent by the task handler after the corresponding work
 * item is created and pushed into the workqueue. This work item will hold an
 * operation ID indicating that it is responsible for sending a DBus message.
 * The actual operation ID will be stored inside the associated data structure,
 * which will be encoded and decoded by the DBus communication framework during
 * message transmission and reception.
 */
