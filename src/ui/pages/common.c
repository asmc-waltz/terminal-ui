/**
 * @file common.c
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
lv_obj_t *create_scr_page(lv_obj_t *par, const char *name)
{
    int32_t obj_w, obj_h;
    lv_obj_t *page;

    page = gf_create_base(par, name);

    gf_gobj_set_size(page, g_get_scr_width(), g_get_scr_height());
    gf_gobj_set_pos(page, 0, 0);

    // TODO: create background
    lv_obj_set_style_bg_color(page, lv_color_hex(0x000000), 0);


    lv_obj_t *top_bar = create_top_bar(page);
    lv_obj_t *sym_box = add_top_bar_symbol(top_bar, "comps.top_bar.wifi",
                                                ICON_WIFI_SOLID);
    gf_gobj_align_to(sym_box, top_bar, LV_ALIGN_TOP_LEFT, TOP_BAR_SYM_ALN, 0);

    return page;
}
