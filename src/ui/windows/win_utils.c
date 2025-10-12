/**
 * @file win_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
// #define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL defined locally will override the global setting in this file"
#endif
#include "log.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <lvgl.h>
#include "list.h"
#include "ui/ui_core.h"
#include "ui/fonts.h"
#include "ui/flex.h"
#include "ui/grid.h"
#include "ui/windows.h"

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
lv_obj_t *create_vertical_flex_group(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj;

    lobj = create_flex_layout_object(par, name);
    if (!lobj)
        return NULL;

    /* Visual style */
    set_gobj_size(lobj, LV_PCT(100), LV_SIZE_CONTENT);
    /* Padding and spacing */
    set_gobj_padding(lobj, 20, 20, 20, 20);
    set_gobj_row_padding(lobj, 20);
    /* Scrollbar cleanup */
    lv_obj_set_style_width(lobj, 1, LV_PART_SCROLLBAR);
    lv_obj_set_style_pad_all(lobj, 0, LV_PART_SCROLLBAR);
    lv_obj_set_style_margin_all(lobj, 0, LV_PART_SCROLLBAR);

    /* Layout configuration */
    set_flex_layout_flow(lobj, LV_FLEX_FLOW_COLUMN);
    set_flex_layout_align(lobj, \
                          LV_FLEX_ALIGN_START, \
                          LV_FLEX_ALIGN_CENTER, \
                          LV_FLEX_ALIGN_CENTER);

    lv_obj_add_flag(lobj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(lobj, LV_OBJ_FLAG_SCROLLABLE);

    return lobj;
}

lv_obj_t *create_horizontal_flex_group(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj;

    lobj = create_flex_layout_object(par, name);
    if (!lobj)
        return NULL;

    /* Visual style */
    set_gobj_size(lobj, LV_PCT(100), LV_SIZE_CONTENT);
    /* Padding and spacing */
    set_gobj_padding(lobj, 20, 20, 20, 20);
    set_gobj_row_padding(lobj, 20);
    /* Scrollbar cleanup */
    lv_obj_set_style_width(lobj, 1, LV_PART_SCROLLBAR);
    lv_obj_set_style_pad_all(lobj, 0, LV_PART_SCROLLBAR);
    lv_obj_set_style_margin_all(lobj, 0, LV_PART_SCROLLBAR);

    /* Layout configuration */
    set_flex_layout_flow(lobj, LV_FLEX_FLOW_ROW);
    set_flex_layout_align(lobj, \
                          LV_FLEX_ALIGN_SPACE_BETWEEN, \
                          LV_FLEX_ALIGN_CENTER, \
                          LV_FLEX_ALIGN_CENTER);

    lv_obj_add_flag(lobj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(lobj, LV_OBJ_FLAG_SCROLLABLE);

    return lobj;
}

