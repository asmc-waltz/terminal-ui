/**
 * @file brightness.c
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
#include "ui/comps.h"
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
static int32_t create_brightness_setting_items(lv_obj_t *par)
{
    lv_obj_t *group, *sym, *label, *swit;
    const char *desc = "Manual and auto brightness setting";

    if (!par)
        return -EINVAL;

    /* Section: Brightness description */
    group = create_vertical_flex_group(par, "BRIGHTNESS-ABOUT");
    if (!group)
        return -ENOMEM;
    set_flex_cell_data(group);

    sym = create_symbol_box(group, NULL, &terminal_icons_48, \
                            ICON_CIRCLE_HALF_STROKE_SOLID);
    if (sym)
        set_flex_cell_data(sym);

    label = create_text_box(group, NULL, &lv_font_montserrat_24, desc);
    if (label)
        set_flex_cell_data(label);

    /* Section: Auto brightness toggle */
    group = create_horizontal_flex_group(par, "BRIGHTNESS-AUTO");
    if (!group)
        return -ENOMEM;
    set_flex_cell_data(group);

    label = create_text_box(group, NULL, &lv_font_montserrat_24, "Auto");
    if (label)
        set_flex_cell_data(label);

    swit = create_switch_box(group, NULL);
    if (swit)
        set_flex_cell_data(swit);

    /* Section: Spacer (flex filler) */
    lv_obj_t *filler = create_box(par, "BRIGHTNESS-FILLER");
    if (filler) {
        set_gobj_size(filler, LV_PCT(100), LV_PCT(100));
        set_flex_cell_data(filler);
    }

    LOG_DEBUG("Brightness setting items created");
    return 0;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t *create_brightness_detail_setting(lv_obj_t *par, const char *name)
{
    lv_obj_t *brightness_box;

    if (!par)
        return NULL;

    brightness_box = create_flex_layout_object(par, name);
    if (!brightness_box)
        return NULL;

    /* Visual style */
    lv_obj_set_style_bg_color(brightness_box, lv_color_hex(bg_color(120)), 0);
    set_gobj_size(brightness_box, LV_PCT(98), LV_PCT(98));
    set_gobj_align(brightness_box, par, LV_ALIGN_CENTER, 0, 0);
    /* Padding and spacing */
    set_gobj_padding(brightness_box, 4, 4, 4, 4);
    set_gobj_row_padding(brightness_box, 20);
    /* Scrollbar cleanup */
    lv_obj_set_style_width(brightness_box, 1, LV_PART_SCROLLBAR);
    lv_obj_set_style_pad_all(brightness_box, 0, LV_PART_SCROLLBAR);
    lv_obj_set_style_margin_all(brightness_box, 0, LV_PART_SCROLLBAR);

    create_brightness_setting_items(brightness_box);

    /* Layout configuration */
    set_flex_layout_flow(brightness_box, LV_FLEX_FLOW_COLUMN);
    set_flex_layout_align(brightness_box, \
                          LV_FLEX_ALIGN_START, \
                          LV_FLEX_ALIGN_CENTER, \
                          LV_FLEX_ALIGN_CENTER);

    return brightness_box;
}
