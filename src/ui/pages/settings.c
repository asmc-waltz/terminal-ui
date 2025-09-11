/**
 * @file setting.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL defined locally will override the global setting in this file"
#endif
#include <log.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <lvgl.h>
#include <list.h>
#include <ui/ui_core.h>
#include <ui/fonts.h>
#include <ui/comps.h>
#include <ui/pages.h>

/*********************
 *      DEFINES
 *********************/
#define SETTING_USED_HEIGHT             (TOP_BAR_PAD_TOP + TOP_BAR_HEIGHT + \
                                         TOP_BAR_PAD_BOT + SETTING_PAD_TOP + \
                                         SETTING_PAD_BOT)
#define SETTING_CONTAINTER_ALIGN        (TOP_BAR_PAD_TOP + TOP_BAR_HEIGHT + \
                                         TOP_BAR_PAD_BOT + SETTING_PAD_TOP)

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
lv_obj_t *create_setting_container(lv_obj_t *par)
{
    lv_obj_t *cont;
    int32_t obj_w, obj_h;

    if (!par)
        return NULL;

    /* Create container box for the setting menu bar and detail setting */
    cont = gf_create_box(par, "pages.setting.container");
    if (!cont)
        return NULL;

    /* Calculate setting container size as percentage of parent size */
    obj_w = calc_pixels(par_width(par), SETTING_WIDTH);
    obj_h = calc_pixels_remaining(par_height(par), SETTING_USED_HEIGHT);

    gf_gobj_set_size(cont, obj_w, obj_h);

    return cont;
}

lv_obj_t *create_setting_page(lv_obj_t *par)
{
    lv_obj_t *lobj_c;
    int32_t obj_w, obj_h, ofs_x, ofs_y;

    lv_obj_t *page;
    page = gf_create_base(par, "pages.setting");
    gf_gobj_set_size(page, par_width(par), par_height(par));
    gf_gobj_set_pos(page, 0, 0);
    lv_obj_set_style_bg_color(page, lv_color_hex(0x000000), 0);


    lv_obj_t *top_bar = create_top_bar(page);
    lv_obj_t *sym_box = add_top_bar_symbol(top_bar, "comps.top_bar.wifi",
                                                ICON_WIFI_SOLID);
    gf_gobj_align_to(sym_box, top_bar, LV_ALIGN_TOP_LEFT, TOP_BAR_SYM_ALN, 0);

    // lobj_c = create_keyboard(lobj);

    lv_obj_t *sett_cont = create_setting_container(page);
    lv_obj_set_style_bg_color(sett_cont, lv_color_hex(0xDDDDDD), 0);
    gf_gobj_align_to(sett_cont, page, LV_ALIGN_TOP_MID, 0,\
                     calc_pixels(par_height(page), SETTING_CONTAINTER_ALIGN));

    return page;
}
