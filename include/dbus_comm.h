/**
 * @file dbus_comm.h
 *
 */

#ifndef G_DBUS_COMM_H
#define G_DBUS_COMM_H
/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>
#include <dbus/dbus.h>

#include <cmd_payload.h>

#include <workqueue.h>
#include <task.h>
#include <log.h>

/*********************
 *      DEFINES
 *********************/
#define SYS_MGR_DBUS_SER                "com.SystemManager.Service"
#define SYS_MGR_DBUS_OBJ_PATH           "/com/SystemManager/Obj/SysCmd"
#define SYS_MGR_DBUS_IFACE              "com.SystemManager.Interface"
#define SYS_MGR_DBUS_METH               "SysMeth"
#define SYS_MGR_DBUS_SIG                "SysSig"

#define UI_DBUS_SER                     "com.TerminalUI.Service"
#define UI_DBUS_OBJ_PATH                "/com/TerminalUI/Obj/UsrCmd"
#define UI_DBUS_IFACE                   "com.TerminalUI.Interface"
#define UI_DBUS_METH                    "UIMeth"
#define UI_DBUS_SIG                     "UISig"

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
int32_t add_dbus_match_rule(DBusConnection *conn, const char *rule);
int32_t dbus_fn_thread_handler();

int32_t dbus_method_call(const char *destination, const char *path, \
                         const char *iface, const char *method, \
                         remote_cmd_t *cmd);
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

#endif /* G_DBUS_COMM_H */
