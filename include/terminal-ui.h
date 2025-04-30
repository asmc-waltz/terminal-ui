/**
 * @file terminal-ui.h
 *
 */

#ifndef G_TERMINAL_UI_H
#define G_TERMINAL_UI_H
/*********************
 *      INCLUDES
 *********************/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <lvgl.h>
#include <list.h>
#include <fonts.h>
#include <style.h>
#include <layers.h>
#include <gmisc.h>
#include <comm.h>
#include <style.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
	ID_NONE = 0,
	ID_BG,
	ID_SRC_HOME,
	ID_STATUS_BAR,
	ID_STATUS_BAR_SIGNAL_STRENGTH,
	ID_STATUS_BAR_SIGNAL_TYPE,
	ID_STATUS_BAR_ETHERNET,
	ID_STATUS_BAR_WIFI,
	ID_STATUS_BAR_CLOCK,
	ID_STATUS_BAR_ALERT,
	ID_STATUS_BAR_POWER,
	ID_TASK_BAR,
	ID_TASK_BAR_PHONE,
	ID_TASK_BAR_MESSAGE,
	ID_TASK_BAR_TOOLBOX,
	ID_TASK_BAR_SETTING,
	ID_HOME_INDICATOR,
} g_id;

typedef struct {
    struct list_head node;
    g_id id;
    char *name;
    bool visible;
} g_obj;

typedef struct {
    struct list_head node;
    g_id id;
    char *name;
    bool visible;
} g_handler;

typedef struct {
    struct list_head obj_list;
    struct list_head handler_list;
} g_app_data;

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*=====================
 * Setter functions
 *====================*/
/*=====================
 * Getter functions
 *====================*/
/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /* G_TERMINAL_UI_H */
