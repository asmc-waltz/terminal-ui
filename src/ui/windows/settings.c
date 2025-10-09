/**
 * @file setting.c
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
#include "ui/ui_core.h"
#include "ui/fonts.h"
#include "ui/windows.h"
#include "ui/flex.h"
#include "ui/grid.h"

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
static void menu_bar_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    switch (code) {
    case LV_EVENT_PRESSED:
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xFF6633), 0);
        break;
    case LV_EVENT_RELEASED:
        lv_obj_set_style_bg_color(obj, lv_color_hex(bg_color(140)), 0);
        break;
    case LV_EVENT_CLICKED:
        LV_LOG_USER("Box clicked!");
        break;
    default:
        break;
    }
}

// static int32_t create_menu_bar_btn(lv_obj_t *par,)

static lv_obj_t *sett_subwin_create_menu_bar(lv_obj_t *par)
{
    lv_obj_t * cont = create_flex_layout_object(par, "FLEX LAYOUT");
    set_flex_layout_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_color(cont, lv_color_hex(bg_color(120)), 0);
    // lv_obj_center(cont);
    lv_obj_set_style_pad_row(cont, 4, 0);
    lv_obj_set_style_pad_column(cont, 4, 0);
    lv_obj_set_style_pad_all(cont, 4, 0);
    lv_obj_set_style_width(cont, 1, LV_PART_SCROLLBAR);
    lv_obj_set_style_pad_all(cont, 0, LV_PART_SCROLLBAR);
    lv_obj_set_style_margin_all(cont, 0, LV_PART_SCROLLBAR);
    // lv_obj_set_style_flex_main_place(cont, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(cont, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(cont,  LV_FLEX_ALIGN_CENTER, 0);

    uint32_t i;
    for(i = 0; i < 25; i++) {
        // lv_obj_t * box = create_box(cont, NULL);
        lv_obj_t * box = create_flex_layout_object(cont, NULL);
        set_flex_layout_flow(box, LV_FLEX_FLOW_ROW);
        set_flex_cell_type(box);
        set_gobj_size(box, LV_PCT(100), 50);
        lv_obj_set_style_pad_column(box, 8, 0);

        lv_obj_set_style_bg_color(box, lv_color_hex(bg_color(140)), 0);
        lv_obj_add_flag(box, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(box, menu_bar_event_handler, LV_EVENT_ALL, NULL);

        lv_obj_set_style_flex_main_place(box, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
        lv_obj_set_style_flex_cross_place(box, LV_FLEX_ALIGN_CENTER, 0);
        lv_obj_set_style_flex_track_place(box,  LV_FLEX_ALIGN_CENTER, 0);


        lv_obj_t *sym;
        sym = create_symbol_box(box, NULL, &terminal_icons_20, ICON_CIRCLE_HALF_STROKE_SOLID);
        set_flex_cell_type(sym);

        lv_obj_t *label;
        label = create_text_box(box, NULL, &lv_font_montserrat_24, "Brightness");
        set_flex_cell_type(label);
    }

    return cont;
}
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *create_setting_window(lv_obj_t *par)
{
    lv_obj_t *setting;

    if (!par)
        return -EINVAL;

    setting = create_grid_layout_object(par, WINDOW_SETTING);
    if (!setting)
        return -EIO;

    lv_obj_set_style_pad_all(setting, 5, 0);

    return setting;
}

int32_t create_setting_content(lv_obj_t *cont)
{
    int32_t ret;

    if (!cont)
        return -EINVAL;

    set_grid_layout_gap(cont, 0, 5, 0, 0);
    set_grid_layout_align(cont, \
                          LV_GRID_ALIGN_SPACE_BETWEEN, \
                          LV_GRID_ALIGN_SPACE_BETWEEN);
    lv_obj_set_style_bg_color(cont, lv_color_hex(bg_color(80)), 0);
    lv_obj_set_style_pad_all(cont, 6, 0);

    /**************************************/
    ret = add_grid_layout_row_dsc(cont, LV_GRID_FR(98));
    if (ret) {
        LOG_ERROR("Add descriptor info failed");
    }
    /**************************************/
    ret = add_grid_layout_col_dsc(cont, LV_GRID_FR(35));
    if (ret) {
        LOG_ERROR("Add descriptor info failed");
    }

    ret = add_grid_layout_col_dsc(cont, LV_GRID_FR(65));
    if (ret) {
        LOG_ERROR("Add descriptor info failed");
    }
    /**************************************/

    apply_grid_layout_config(cont);


    lv_obj_t *setting_menu;
    setting_menu = sett_subwin_create_menu_bar(cont);
    set_grid_cell_align(setting_menu, LV_GRID_ALIGN_STRETCH, 0, 1, \
                        LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_style_bg_color(setting_menu, lv_color_hex(bg_color(100)), 0);

    lv_obj_t *setting_detail;
    setting_detail = create_box(cont, "detail");
    set_grid_cell_align(setting_detail, LV_GRID_ALIGN_STRETCH, 1, 1,
                        LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_style_bg_color(setting_detail, lv_color_hex(bg_color(100)), 0);

    return 0;
}
