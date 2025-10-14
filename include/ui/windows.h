/**
 * @file window.h
 *
 */

#ifndef G_WINDOWS_H
#define G_WINDOWS_H
/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>

#include <lvgl.h>
#include "ui/ui_core.h"
#include "ui/screen.h"

/*********************
 *      DEFINES
 *********************/
#define WINDOW_SETTING                  "window.setting"

/**********************
 *      TYPEDEFS
 **********************/
typedef struct ctx ctx_t;

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
lv_obj_t *create_vertical_flex_group(lv_obj_t *par, const char *name);
lv_obj_t *create_horizontal_flex_group(lv_obj_t *par, const char *name);

lv_obj_t *create_menu(lv_obj_t *par, const char *name);
int32_t load_active_menu_page(lv_obj_t *lobj);
int32_t set_active_menu_page(lv_obj_t *lobj, const char *name, \
                                    lv_obj_t *(*create_page_cb)(lv_obj_t *, \
                                                                const char *));

lv_obj_t *create_brightness_setting(lv_obj_t *par, const char *name);
/**********************
 *      MACROS
 **********************/

#endif /* G_WINDOWS_H */
