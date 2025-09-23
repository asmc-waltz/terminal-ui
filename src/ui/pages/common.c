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
#include "ui/comps.h"
#include "ui/pages.h"

/*********************
 *      DEFINES
 *********************/
#define SCREEN_BG_COLOR                 0x292D33

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
#define TEST 1
#if defined(TEST)
#include <time.h>

int32_t get_random_3(void)
{
    int32_t val;

    val = rand() % 4;
    return val;
}

static void rotate_key_handler(lv_event_t *event)
{
    lv_obj_t *lobj = get_obj_by_name("screens.common", \
                            &((g_obj *)(lv_screen_active()->user_data))->child);

    set_scr_rotation(get_random_3());
    refresh_obj_tree_layout(lobj->user_data);
}

static void create_keyboard_handler(lv_event_t *event)
{
    lv_obj_t *comm_page;
    comm_page = get_obj_by_name("screens.common", &get_gobj(lv_screen_active())->child);
    if (!comm_page) {
        LOG_ERROR("Screen [%s] not found", "screens.common");
    }

    create_keyboard(comm_page);
}

#endif

lv_obj_t *create_scr_page(lv_obj_t *par, const char *name)
{
    int32_t obj_w, obj_h;
    lv_obj_t *page;

    page = create_base(par, name);

    set_gobj_size(page, get_scr_width(), get_scr_height());
    set_gobj_pos(page, 0, 0);

    // TODO: create background
    lv_obj_set_style_bg_color(page, lv_color_hex(SCREEN_BG_COLOR), 0);


    lv_obj_t *top_space = create_top_bar(page);
    lv_obj_t *sym_box = add_top_bar_symbol(top_space, TOP_BAR_NAME".wifi",
                                                ICON_WIFI_SOLID);
    align_gobj_to(sym_box, top_space, LV_ALIGN_TOP_LEFT, TOP_BAR_SYM_ALN, 0);

    // TODO: debug setting page
    create_setting_page(page);


#if defined(TEST)
    lv_obj_t *btn = create_btn(lv_layer_top(), "common.rotate_btn");
    lv_obj_add_event_cb(btn, rotate_key_handler, LV_EVENT_CLICKED, btn->user_data);
    set_gobj_size(btn, 54, 54);
    align_gobj_to(btn, lv_layer_top(), LV_ALIGN_CENTER, -50, 0);

    lv_obj_t *icon = create_sym(btn, NULL, TOP_BAR_SYM_FONTS, ICON_ROTATE_SOLID);
    lv_obj_set_style_text_color(icon, lv_color_hex(0xFFFFFF), 0);


    btn = create_btn(lv_layer_top(), "create keyboard");
    set_gobj_size(btn, 54, 54);
    align_gobj_to(btn, lv_layer_top(), LV_ALIGN_CENTER, 50, 0);
    lv_obj_add_event_cb(btn, create_keyboard_handler, LV_EVENT_CLICKED, btn->user_data);

    icon = create_sym(btn, NULL, TOP_BAR_SYM_FONTS, ICON_KEYBOARD);
    lv_obj_set_style_text_color(icon, lv_color_hex(0xFFFFFF), 0);
#endif

    return page;
}
