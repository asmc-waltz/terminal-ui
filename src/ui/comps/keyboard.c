/**
 * @file keyboard.c
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

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <lvgl.h>
#include <list.h>
#include <ui/ui_core.h>

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
lv_obj_t * gf_keyboard_create(lv_obj_t * par, const char *name)
{
    lv_obj_t *lobj = NULL;
    lobj = gf_create_box(par, name);
    if (!lobj) {
        return NULL;
    }

    gf_gobj_set_size(lobj, 1004, 300);
    gf_obj_scale_enable_w(lobj);
    gf_obj_scale_set_pad_w(lobj, 20);
    lv_obj_set_style_bg_color(lobj, lv_color_hex(0xFFFFFF), 0);
    gf_gobj_align_to(lobj, par, LV_ALIGN_BOTTOM_MID, 0, -10);

    return lobj;
}
