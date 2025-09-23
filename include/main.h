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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct screen {
    lv_display_t *drm_disp;
    lv_indev_t *touch_event;
    int32_t width;
    int32_t height;
    int32_t rotation;
} screen_t;

typedef struct ui_obj {
    struct list_head obj_lst;           /* List of registered UI objects */
    uint32_t next_id;
} obj_t;

typedef struct op_handler {
    struct list_head handler_lst;       /* List of registered opcode handlers */
    wq_ctx_t *wq_ctxs;                  /* List of registered workqueues */
} op_t;

typedef struct comm_handler {
    DBusConnection *dbus_conn;
    int32_t event_fd;
    sig_atomic_t run;
} comm_t;

typedef struct conf_data {
    int8_t brightness;
    int8_t als_en;
} conf_t;

typedef struct ctx {
    screen_t scr;
    obj_t ui;
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
