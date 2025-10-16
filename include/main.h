/**
 * @file main.h
 *
 */

#ifndef G_MAIN_H
#define G_MAIN_H
/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <dbus/dbus.h>

#include <lvgl.h>
#include "list.h"
#include "sched/workqueue.h"
#include "ui/screen.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct obj_ctx {
    struct list_head list;              /* List of registered UI objects */
    uint32_t next_id;
} obj_ctx_t;

typedef struct op_handler {
    struct list_head handler_lst;       /* List of registered opcode handlers */
} op_t;

typedef struct comm_handler {
    DBusConnection *dbus_conn;
    int32_t event;
} comm_t;

typedef struct conf_data {
    int8_t brightness;
    int8_t als_en;
} conf_t;

typedef struct ctx {
    sig_atomic_t run;
    scr_ctx_t scr;
    obj_ctx_t objs;
    wq_ctx_t *wqs;
    op_t op;
    comm_t comm;
    conf_t cfg;
} ctx_t;

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  GLOBAL PROTOTYPES
 **********************/
ctx_t *get_ctx();
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

#endif /* G_MAIN_H */
