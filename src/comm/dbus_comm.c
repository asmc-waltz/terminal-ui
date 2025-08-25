/**
 * @file dbus_comm.c
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
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/eventfd.h>
#include <inttypes.h>
#include <stdbool.h>
#include <dbus/dbus.h>

#include <dbus_comm.h>
#include <sys_comm.h>
#include <cmd_payload.h>

#include <workqueue.h>
#include <task.h>

/*********************
 *      DEFINES
 *********************/
#define MAX_EVENTS 2

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/
extern volatile sig_atomic_t g_run;
extern int32_t event_fd;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static DBusConnection *dbus_conn = NULL;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
// Encode remote_cmd_t into an existing DBusMessage
static bool encode_data_frame(DBusMessage *msg, const remote_cmd_t *cmd)
{
    DBusMessageIter iter, array_iter, struct_iter, variant_iter;

    dbus_message_iter_init_append(msg, &iter);

    dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &cmd->component_id);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &cmd->topic_id);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &cmd->opcode);

    dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "(siiv)", &array_iter);

    for (int32_t i = 0; i < cmd->entry_count; ++i) {
        const payload_t *entry = &cmd->entries[i];

        dbus_message_iter_open_container(&array_iter, DBUS_TYPE_STRUCT, NULL, &struct_iter);

        dbus_message_iter_append_basic(&struct_iter, DBUS_TYPE_STRING, &entry->key);
        dbus_message_iter_append_basic(&struct_iter, DBUS_TYPE_INT32, &entry->data_type);
        dbus_message_iter_append_basic(&struct_iter, DBUS_TYPE_INT32, &entry->data_length);

        const char *sig = NULL;
        switch (entry->data_type) {
            case DBUS_TYPE_STRING: sig = "s"; break;
            case DBUS_TYPE_INT32:  sig = "i"; break;
            case DBUS_TYPE_UINT32: sig = "u"; break;
            case DBUS_TYPE_DOUBLE: sig = "d"; break;
            default:
                LOG_ERROR("Unsupported data_type %d for key '%s'", entry->data_type, entry->key);
                return false;
        }

        dbus_message_iter_open_container(&struct_iter, DBUS_TYPE_VARIANT, sig, &variant_iter);

        switch (entry->data_type) {
            case DBUS_TYPE_STRING:
                dbus_message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &entry->value.str);
                break;
            case DBUS_TYPE_INT32:
                dbus_message_iter_append_basic(&variant_iter, DBUS_TYPE_INT32, &entry->value.i32);
                break;
            case DBUS_TYPE_UINT32:
                dbus_message_iter_append_basic(&variant_iter, DBUS_TYPE_UINT32, &entry->value.u32);
                break;
            case DBUS_TYPE_DOUBLE:
                dbus_message_iter_append_basic(&variant_iter, DBUS_TYPE_DOUBLE, &entry->value.dbl);
                break;
        }

        dbus_message_iter_close_container(&struct_iter, &variant_iter);
        dbus_message_iter_close_container(&array_iter, &struct_iter);
    }

    dbus_message_iter_close_container(&iter, &array_iter);
    return true;
}

// Decode DBusMessage into remote_cmd_t
static bool decode_data_frame(DBusMessage *msg, remote_cmd_t *out)
{
    DBusMessageIter iter, array_iter, struct_iter, variant_iter;

    if (!dbus_message_iter_init(msg, &iter)) {
        LOG_ERROR("Failed to init DBus iterator");
        return false;
    }

    dbus_message_iter_get_basic(&iter, &out->component_id);
    dbus_message_iter_next(&iter);

    dbus_message_iter_get_basic(&iter, &out->topic_id);
    dbus_message_iter_next(&iter);

    dbus_message_iter_get_basic(&iter, &out->opcode);
    dbus_message_iter_next(&iter);

    dbus_message_iter_recurse(&iter, &array_iter);

    int32_t i = 0;
    while (dbus_message_iter_get_arg_type(&array_iter) == DBUS_TYPE_STRUCT && i < MAX_ENTRIES) {
        payload_t *entry = &out->entries[i];
        dbus_message_iter_recurse(&array_iter, &struct_iter);

        dbus_message_iter_get_basic(&struct_iter, &entry->key);
        dbus_message_iter_next(&struct_iter);

        dbus_message_iter_get_basic(&struct_iter, &entry->data_type);
        dbus_message_iter_next(&struct_iter);

        dbus_message_iter_get_basic(&struct_iter, &entry->data_length);
        dbus_message_iter_next(&struct_iter);

        dbus_message_iter_recurse(&struct_iter, &variant_iter);

        switch (entry->data_type) {
            case DBUS_TYPE_STRING:
                dbus_message_iter_get_basic(&variant_iter, &entry->value.str);
                break;
            case DBUS_TYPE_INT32:
                dbus_message_iter_get_basic(&variant_iter, &entry->value.i32);
                break;
            case DBUS_TYPE_UINT32:
                dbus_message_iter_get_basic(&variant_iter, &entry->value.u32);
                break;
            case DBUS_TYPE_DOUBLE:
                dbus_message_iter_get_basic(&variant_iter, &entry->value.dbl);
                break;
            default:
                LOG_WARN("Unsupported type %d for entry %d", entry->data_type, i);
                break;
        }

        dbus_message_iter_next(&array_iter);
        i++;
    }

    out->entry_count = i;
    return true;
}

static int32_t dispatch_cmd_from_message(DBusMessage *msg)
{
    remote_cmd_t *cmd;
    work_t *work;
    int32_t i;

    cmd = create_remote_cmd();
    if (!cmd) {
        LOG_ERROR("Failed to allocate memory for cmd_data");
        return -ENOMEM;
    }

    if (!decode_data_frame(msg, cmd)) {
        LOG_ERROR("Failed to decode DBus message");
        delete_remote_cmd(cmd);
        return -EINVAL;
    }

    LOG_INFO("Received frame from component: %s", cmd->component_id);
    LOG_INFO("Topic: %d, Opcode: %d", cmd->topic_id, cmd->opcode);

    for (i = 0; i < cmd->entry_count; ++i) {
        payload_t *entry = &cmd->entries[i];
        LOG_INFO("Key: %s, Type: %c", entry->key, entry->data_type);
        switch (entry->data_type) {
        case DBUS_TYPE_STRING:
            LOG_INFO("Value (string): %s", entry->value.str);
            break;
        case DBUS_TYPE_INT32:
            LOG_INFO("Value (int): %d", entry->value.i32);
            break;
        default:
            LOG_WARN("Unsupported type: %c", entry->data_type);
            break;
        }
    }

    work = create_work(REMOTE, BLOCK, SHORT, cmd->opcode, (void *)cmd);
    if (!work) {
        LOG_ERROR("Failed to create work from cmd");
        delete_remote_cmd(cmd);
        return -ENOMEM;
    }

    push_work(work);
    return 0;
}

static int32_t dbus_event_handler(DBusConnection *conn)
{
    DBusMessage *msg;
    DBusMessage *reply;
    DBusMessageIter args;
    const char *reply_str = "Method reply OK";
    int32_t ret;

    msg = NULL;
    reply = NULL;

    while (dbus_connection_read_write_dispatch(conn, 0)) {
        msg = dbus_connection_pop_message(conn);
        if (!msg) {
            usleep(10000);
            continue;
        }

        if (dbus_message_get_type(msg) == DBUS_MESSAGE_TYPE_METHOD_CALL) {
            const char *iface = dbus_message_get_interface(msg);
            const char *member = dbus_message_get_member(msg);

            if (iface && member && \
                strcmp(iface, UI_DBUS_IFACE) == 0 && \
                strcmp(member, UI_DBUS_METH) == 0) {
                ret = dispatch_cmd_from_message(msg);
                if (ret < 0) {
                    LOG_ERROR("Dispatch failed: iface=%s, meth=%s",
                          iface, member);
                    reply = dbus_message_new_error(msg,
                                       DBUS_ERROR_FAILED,
                                       "Dispatch failed");
                } else {
                    reply = dbus_message_new_method_return(msg);
                    dbus_message_iter_init_append(reply, &args);
                    dbus_message_iter_append_basic(&args,
                                       DBUS_TYPE_STRING,
                                       &reply_str);
                }

                if (reply) {
                    dbus_connection_send(conn, reply, NULL);
                    dbus_message_unref(reply);
                }
            }

        } else if (dbus_message_get_type(msg) == DBUS_MESSAGE_TYPE_SIGNAL) {
            const char *iface = dbus_message_get_interface(msg);
            const char *member = dbus_message_get_member(msg);

            if (iface && member && \
                strcmp(iface, SYS_MGR_DBUS_IFACE) == 0 && \
                strcmp(member, SYS_MGR_DBUS_SIG) == 0) {
                ret = dispatch_cmd_from_message(msg);
                if (ret < 0)
                    LOG_ERROR("Dispatch signal failed: %s.%s",
                          iface, member);
            }
        } else if (dbus_message_get_type(msg) == DBUS_MESSAGE_TYPE_METHOD_RETURN) {
            LOG_TRACE("Dbus method return is detected");
        }


        dbus_message_unref(msg);
        break;
    }

    return 0;
}

static DBusConnection * setup_dbus()
{
    DBusConnection *conn = NULL;
    DBusError err;
    int32_t ret = 0;

    dbus_error_init(&err);
    conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if (dbus_error_is_set(&err)) {
        LOG_ERROR("DBus connection Error: %s", err.message);
        dbus_error_free(&err);
    }

    if (NULL == conn) {
        return NULL;
    }

    ret = dbus_bus_request_name(conn, \
                                UI_DBUS_SER, \
                                DBUS_NAME_FLAG_REPLACE_EXISTING, \
                                &err);
    if (dbus_error_is_set(&err)) {
        LOG_FATAL("Dbus request name error: %s", err.message);
        dbus_error_free(&err);
        return NULL;
    }

    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        return NULL;
    }

    char* match_rule = (char*)calloc(256, sizeof(char));
    if (!match_rule) {
        LOG_ERROR("Failed to allocate memory");
        return NULL;
    }
    sprintf(match_rule, "type='signal',interface='%s',member='%s',path='%s'",
            SYS_MGR_DBUS_IFACE, SYS_MGR_DBUS_SIG, SYS_MGR_DBUS_OBJ_PATH);

    ret = add_dbus_match_rule(conn, match_rule);
    if (ret) {
        return NULL;
    }

    free(match_rule);

    return conn;
}

static int32_t dbus_listener(DBusConnection *conn)
{
    int32_t dbus_fd;
    int32_t epoll_fd;
    struct epoll_event ev;
    struct epoll_event events_detected[MAX_EVENTS];
    int32_t n_ready;
    int32_t ready_fd;

    // Get DBus file desc
    dbus_connection_get_unix_fd(conn, &dbus_fd);
    if (dbus_fd < 0) {
        LOG_ERROR("Failed to get dbus fd");
        return -1;
    }

    // Create epoll file desc
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        LOG_ERROR("Failed to create epoll fd");
        return -1;
    }

    ev.events = EPOLLIN;

    // Add DBus file desc
    ev.data.fd = dbus_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, dbus_fd, &ev) == -1) {
        LOG_ERROR("Failed to add fd to epoll_ctl");
        close(epoll_fd);
        return -1;
    }

    // Add Event file desc
    ev.data.fd = event_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_fd, &ev) == -1) {
        LOG_ERROR("Failed to add fd to epoll_ctl");
        close(epoll_fd);
        return -1;
    }

    LOG_INFO("System manager DBus communication is running...");
    while (g_run) {
        LOG_TRACE("[DBus]--> Waiting for next DBus message...");
        n_ready = epoll_wait(epoll_fd, events_detected, MAX_EVENTS, -1);
        for (int32_t cnt = 0; cnt < n_ready; cnt++) {
            ready_fd = events_detected[cnt].data.fd;
            if (ready_fd == dbus_fd) {
                dbus_event_handler(conn);
            } else if (ready_fd == event_fd) {
                uint64_t event_id = 0;
                if (!event_get(event_fd, &event_id)) {
                    LOG_INFO("Received event ID [%" PRIu64 "], stopping DBus listener...", \
                             event_id);
                }
            }
        }
    }

    close(epoll_fd);
    LOG_INFO("The DBus handler thread exited successfully");

    return 0;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
DBusConnection *get_dbus_conn()
{
    return dbus_conn;
}

bool set_dbus_conn(DBusConnection *ptr)
{
    if (!ptr) {
        return false;
    }

    dbus_conn = ptr;
    return true;
}

int32_t add_dbus_match_rule(DBusConnection *conn, const char *rule)
{
    DBusError err;
    if (NULL == conn) {
        return EINVAL;
    }

    LOG_INFO("Adds a match rule: [%s]", rule);
    dbus_error_init(&err);
    dbus_bus_add_match(conn, rule, &err);
    if (dbus_error_is_set(&err)) {
        LOG_ERROR("Add failed (error: %s)", err.message);
        dbus_error_free(&err);
        return EINVAL;
    }

    dbus_connection_flush(conn);
    LOG_TRACE("Add match rule succeeded");
    return 0;
}

int32_t dbus_fn_thread_handler()
{
    DBusConnection *conn;
    int32_t ret;

    conn = setup_dbus();
    if (!conn) {
        LOG_FATAL("Unable to establish connection with DBus");
        return -1;
    }

    if (!set_dbus_conn(conn)) {
        LOG_FATAL("Unable to save connection with DBus");
        return -1;
    }


    // This thread processes DBus messages
    ret = dbus_listener(conn);
    if (ret) {
        LOG_FATAL("Failed to create DBus listener");
    }

    dbus_connection_unref(conn);
    return ret;
}

/*
 * This function sends a D-Bus method call to the dbus client.
 * It operates without a specific callback for the reply message
 * because all responses are processed centrally in the D-Bus
 * listener thread rather than assigning per-call callbacks.
 */
int32_t dbus_method_call(const char *destination, const char *path, \
                         const char *iface, const char *method, \
                         remote_cmd_t *cmd)
{
    DBusConnection *conn;
    DBusMessage *msg;

    if (!destination || !path || !iface || !method || !cmd) {
        LOG_ERROR("Invalid argument");
        return -EINVAL;
    }

    conn = get_dbus_conn();
    if (!conn) {
        LOG_ERROR("Failed to get dbus connection");
        return -EIO;
    }

    msg = dbus_message_new_method_call(destination, path, iface, method);
    if (!msg) {
        LOG_FATAL("Failed to create method call message");
        return -ENOMEM;
    }

    if (!encode_data_frame(msg, cmd)) {
        LOG_ERROR("Failed to encode data frame");
        dbus_message_unref(msg);
        return -EIO;
    }

    if (!dbus_connection_send(conn, msg, NULL)) {
        LOG_ERROR("Out of memory while sending method call");
        dbus_message_unref(msg);
        return -ENOMEM;
    }

    dbus_connection_flush(conn);
    dbus_message_unref(msg);

    return 0;
}
