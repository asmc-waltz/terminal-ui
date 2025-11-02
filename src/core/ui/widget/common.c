/**
 * @file common.c
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
/*
 * Create a flex layout group.
 * The direction (horizontal/vertical) and alignment are configurable.
 */
static lv_obj_t *create_flex_group(lv_obj_t *par, const char *name, \
                                   lv_flex_flow_t flow, \
                                   lv_flex_align_t main_align, \
                                   lv_flex_align_t cross_align, \
                                   lv_flex_align_t track_align)
{
    lv_obj_t *lobj;
    int32_t ret;

    if (!par)
        return NULL;

    lobj = create_flex_layout_object(par, name);
    if (!lobj)
        return NULL;

    set_size(lobj, LV_PCT(100), LV_SIZE_CONTENT);

    ret = set_padding(lobj, 20, 20, 20, 20);
    if (ret)
        LOG_WARN("Group [%s] set padding failed (%d)", get_name(lobj), ret);

    set_flex_layout_flow(lobj, flow);
    set_flex_layout_align(lobj, main_align, cross_align, track_align);

    lv_obj_set_style_width(lobj, 1, LV_PART_SCROLLBAR);
    lv_obj_set_style_pad_all(lobj, 0, LV_PART_SCROLLBAR);
    lv_obj_set_style_margin_all(lobj, 0, LV_PART_SCROLLBAR);

    lv_obj_add_flag(lobj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(lobj, LV_OBJ_FLAG_SCROLLABLE);

    return lobj;
}

/*---------------------------------------------
 * Specialized wrappers for readability
 *--------------------------------------------*/
lv_obj_t *create_vertical_flex_group(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj = NULL;
    int32_t ret;

    lobj = create_flex_group(par, name, \
                             LV_FLEX_FLOW_COLUMN, \
                             LV_FLEX_ALIGN_START, \
                             LV_FLEX_ALIGN_CENTER, \
                             LV_FLEX_ALIGN_CENTER);
    if (!lobj)
        return NULL;

    set_row_padding(lobj, 20);

    return lobj;
}

lv_obj_t *create_horizontal_flex_group(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj = NULL;
    int32_t ret;

    lobj = create_flex_group(par, name, \
                             LV_FLEX_FLOW_ROW, \
                             LV_FLEX_ALIGN_SPACE_BETWEEN, \
                             LV_FLEX_ALIGN_CENTER, \
                             LV_FLEX_ALIGN_CENTER);
    if (!lobj)
        return NULL;

    set_column_padding(lobj, 20);

    return lobj;
}

