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
static int32_t redraw_window_control_bar(lv_obj_t *lobj)
{
    obj_meta_t *meta;
    int32_t scr_rot, cur_rot;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    cur_rot = meta->data.rotation;
    scr_rot = get_scr_rotation();

    if (scr_rot == ROTATION_90 || scr_rot == ROTATION_270) {
        lv_obj_clear_flag(lobj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(lobj, LV_OBJ_FLAG_HIDDEN);
    }

    return 0;
}

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
    set_size(lobj, LV_PCT(100), LV_SIZE_CONTENT);
    /* Padding and spacing */
    set_padding(lobj, 20, 20, 20, 20);
    set_row_padding(lobj, 20);
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
    set_size(lobj, LV_PCT(100), LV_SIZE_CONTENT);
    /* Padding and spacing */
    set_padding(lobj, 20, 20, 20, 20);
    set_row_padding(lobj, 20);
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

/*
 * Create the top control bar for a window.
 * When the new window covers its parent, this bar may show
 * a "Back" button and an optional "More" button.
 */
lv_obj_t *create_window_control_bar(lv_obj_t *par, const char *name, \
                                    bool back_btn_ena, bool more_btn_ena)
{
    lv_obj_t *lobj, *back_btn, *more_btn;

    /** Validate input */
    if (!par)
        return NULL;

    /** Create a horizontal flex group container */
    lobj = create_horizontal_flex_group(par, name);
    if (!lobj)
        return NULL;

    /** Basic configuration */
    set_padding(lobj, 10, 10, 10, 10);
    lv_obj_add_flag(lobj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(lobj, LV_OBJ_FLAG_SCROLLABLE);

    /** Register prerotate callback for redrawing after rotation */
    get_meta(lobj)->data.prerotate_cb = redraw_window_control_bar;
    lv_obj_add_flag(lobj, LV_OBJ_FLAG_HIDDEN);

    /** Create "Back" button */
    if (back_btn_ena) {
        back_btn = create_text_box(lobj, \
                                   NULL, \
                                   &lv_font_montserrat_24, \
                                   "< Back");
        lv_obj_add_flag(back_btn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_text_color(back_btn, lv_color_hex(0x0000FF), 0);
    } else {
        back_btn = create_text_box(lobj, \
                                   NULL, \
                                   &lv_font_montserrat_24, \
                                   " ");
        lv_obj_clear_flag(back_btn, LV_OBJ_FLAG_CLICKABLE);
    }

    if (!back_btn)
        LOG_WARN("Object [%s]: Create BACK button failed", name);

    /** Create "More" button */
    if (more_btn_ena) {
        more_btn = create_text_box(lobj, \
                                   NULL, \
                                   &lv_font_montserrat_24, \
                                   "...");
        lv_obj_add_flag(more_btn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_text_color(more_btn, lv_color_hex(0x0000FF), 0);
    } else {
        more_btn = create_text_box(lobj, \
                                   NULL, \
                                   &lv_font_montserrat_24, \
                                   " ");
        lv_obj_clear_flag(more_btn, LV_OBJ_FLAG_CLICKABLE);
    }

    if (!more_btn)
        LOG_WARN("Object [%s]: Create MORE button failed", name);

    /** Return the completed control bar */
    return lobj;
}
