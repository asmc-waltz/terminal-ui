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
#include "list.h"
#include "ui/ui_core.h"
#include "ui/fonts.h"
#include "ui/comps.h"
#include "ui/pages.h"

/*********************
 *      DEFINES
 *********************/
#define SETTING_USED_HEIGHT             (TOP_BAR_PAD_TOP + TOP_BAR_HEIGHT + \
                                         TOP_BAR_PAD_BOT + SETTING_PAD_TOP + \
                                         SETTING_PAD_BOT) // %
#define SETTING_CONTAINTER_ALIGN        (TOP_BAR_PAD_TOP + TOP_BAR_HEIGHT + \
                                         TOP_BAR_PAD_BOT + SETTING_PAD_TOP) // %

#define SETTING_MENU_BAR_HOR_WIDTH      30 // %
#define SETTING_MENU_BAR_ALIGN          2  // %
#define SETTING_MENU_BAR_VER_WIDTH      (100 - SETTING_PAD_LEFT - \
                                         SETTING_PAD_RIGHT) // %

#define SETTING_CONT_BG_COLOR           0x636D7A
#define SETTING_MENU_BG_COLOR           0x8F9DB0
#define SETTING_MENU_BTN_BG_COLOR       0xADBACC

#define SETTING_DETAIL_BG_COLOR         SETTING_MENU_BG_COLOR
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
/*
 * The setting detail menu is available only when the screen is in landscape
 * mode.
 */
static void detail_setting_post_rot_resize_adjust_cb(lv_obj_t *lobj)
{
    lv_obj_t *par;
    int32_t obj_w = 0, obj_h = 0;
    int32_t scr_rot;

    if (!lobj)
        return;

    par = lv_obj_get_parent(lobj);
    if (!par)
        return;

    scr_rot = get_scr_rotation();

    if (scr_rot == ROTATION_0 || scr_rot == ROTATION_180) {
        obj_w = calc_pixels_remaining(obj_scale_w(par),
                                      SETTING_MENU_BAR_HOR_WIDTH +
                                      SETTING_PAD_LEFT +
                                      SETTING_PAD_RIGHT +
                                      SETTING_PAD_RIGHT);
        obj_h = calc_pixels_remaining(obj_scale_h(par),
                                      SETTING_MENU_BAR_ALIGN);
        lv_obj_clear_flag(lobj, LV_OBJ_FLAG_HIDDEN);
    } else if (scr_rot == ROTATION_90 || scr_rot == ROTATION_270) {
        obj_w = calc_pixels(obj_scale_w(par),
                            SETTING_MENU_BAR_VER_WIDTH);
        obj_h = calc_pixels_remaining(obj_scale_h(par),
                                      SETTING_MENU_BAR_ALIGN * 2);
        lv_obj_add_flag(lobj, LV_OBJ_FLAG_HIDDEN);
    }

    set_gobj_size(lobj, obj_w, obj_h);
}

static lv_obj_t *create_setting_detail(lv_obj_t *par)
{
    int32_t obj_w, obj_h;
    lv_obj_t *detail_box;

    if (!par)
        return NULL;

    /* Create container box for menu bar */
    detail_box = create_box(par, "pages.setting.detail_box");
    if (!detail_box)
        return NULL;

    /* Calculate menu bar size as percentage of parent size */
    obj_w = calc_pixels_remaining(obj_width(par), SETTING_MENU_BAR_HOR_WIDTH + \
                                  SETTING_PAD_LEFT + SETTING_PAD_RIGHT + \
                                  SETTING_PAD_RIGHT);
    obj_h = calc_pixels_remaining(obj_height(par), SETTING_MENU_BAR_ALIGN);

    set_gobj_size(detail_box, obj_w, obj_h);
    lv_obj_set_style_bg_color(detail_box, \
                              lv_color_hex(SETTING_DETAIL_BG_COLOR), 0);
    align_gobj_to(detail_box, par, LV_ALIGN_TOP_RIGHT, \
                     -calc_pixels(obj_width(par), SETTING_PAD_LEFT), \
                     calc_pixels(obj_height(par), SETTING_PAD_TOP));

    gobj_t *gobj = get_gobj(detail_box);
    gobj->scale.post_rot_resize_adjust_cb = \
        detail_setting_post_rot_resize_adjust_cb;


    /*------------------------------------------------------------------------*/
    lv_obj_t *child;
    child = create_box(detail_box, "child_test");
    set_gobj_size(child, calc_pixels(obj_width(detail_box), 80), \
                     calc_pixels(obj_height(detail_box), 10));
    align_gobj_to(child, detail_box, LV_ALIGN_TOP_LEFT, 10, 10);
    /*------------------------------------------------------------------------*/

    return detail_box;
}

static void menu_bar_post_rot_resize_adjust_cb(lv_obj_t *lobj)
{
    lv_obj_t *par;
    int32_t obj_w = 0, obj_h = 0;
    int32_t scr_rot;

    if (!lobj)
        return;

    par = lv_obj_get_parent(lobj);
    if (!par)
        return;

    scr_rot = get_scr_rotation();

    if (scr_rot == ROTATION_0 || scr_rot == ROTATION_180) {
        obj_w = calc_pixels(obj_scale_w(par), SETTING_MENU_BAR_HOR_WIDTH);
        obj_h = calc_pixels_remaining(obj_scale_h(par),
                                      SETTING_MENU_BAR_ALIGN);
    } else if (scr_rot == ROTATION_90 || scr_rot == ROTATION_270) {
        obj_w = calc_pixels(obj_scale_w(par), SETTING_MENU_BAR_VER_WIDTH);
        obj_h = calc_pixels_remaining(obj_scale_h(par),
                                      SETTING_MENU_BAR_ALIGN * 2);
    }

    set_gobj_size(lobj, obj_w, obj_h);
}

static lv_obj_t *create_menu_bar_btn(lv_obj_t *par, lv_obj_t *ref, \
                                     const char *name, const char* index, \
                                     const char* str)
{
    lv_obj_t *btn_box, *sym_box, *text_box;

    btn_box = create_btn(par, name);
    if (btn_box == NULL) {
        return NULL;
    }

    sym_box = create_symbol_box(btn_box, NULL, SETTING_SYM_FONTS, index);
    if (sym_box == NULL) {
        remove_obj_and_child_by_name(name, &(get_gobj(par))->child);
        return NULL;
    }

    text_box = create_text_box(btn_box, NULL, &lv_font_montserrat_24, str);
    if (text_box == NULL) {
        remove_obj_and_child_by_name(name, &(get_gobj(par))->child);
        return NULL;
    }

    set_gobj_size(btn_box, calc_pixels(obj_width(par), 96), \
                     calc_pixels(obj_height(sym_box), 200));
    enable_scale_w(btn_box);
    set_obj_scale_pad_w(btn_box, calc_pixels(obj_width(par), 4));

    lv_obj_set_style_bg_color(btn_box, \
                              lv_color_hex(SETTING_MENU_BTN_BG_COLOR), 0);

    if (par == ref) {
        align_gobj_to(btn_box, par, LV_ALIGN_TOP_MID, 0, \
                         calc_pixels(obj_width(par), 2));
    } else {
        align_gobj_to(btn_box, ref, LV_ALIGN_OUT_BOTTOM_MID, 0, \
                         calc_pixels(obj_width(par), 2));
    }

    align_gobj_to(sym_box, btn_box, LV_ALIGN_TOP_LEFT, \
                     calc_pixels(obj_height(btn_box), 50), \
                     (obj_height(btn_box) - lv_obj_get_height(sym_box)) / 2);
    align_gobj_to(text_box, sym_box, LV_ALIGN_OUT_RIGHT_MID, \
                     calc_pixels(obj_height(btn_box), 50), 0);

    return btn_box;
}


/*
 * The setting menu bar is placed on the left side of the screen by default.
 * It will expand to full screen when the screen is rotated to vertical mode.
 */
static lv_obj_t *create_menu_bar(lv_obj_t *par)
{
    int32_t obj_w, obj_h;
    lv_obj_t *menu_bar;

    if (!par)
        return NULL;

    /* Create container box for menu bar */
    menu_bar = create_box(par, "pages.setting.menu_bar");
    if (!menu_bar)
        return NULL;

    /* Calculate menu bar size as percentage of parent size */
    obj_w = calc_pixels(obj_width(par), SETTING_MENU_BAR_HOR_WIDTH);
    obj_h = calc_pixels_remaining(obj_height(par), SETTING_MENU_BAR_ALIGN);

    set_gobj_size(menu_bar, obj_w, obj_h);
    lv_obj_set_style_bg_color(menu_bar, lv_color_hex(SETTING_MENU_BG_COLOR), 0);
    align_gobj_to(menu_bar, par, LV_ALIGN_TOP_LEFT, \
                     calc_pixels(obj_width(par), SETTING_PAD_LEFT), \
                     calc_pixels(obj_height(par), SETTING_PAD_TOP));

    /*
     * In case of the menu bar, the layout changes based on the rotation.
     * So we don't need to expand the width and height. The object size will
     * change according to the callback when the object is rotated.
     */
    gobj_t *gobj = get_gobj(menu_bar);
    gobj->scale.post_rot_resize_adjust_cb = menu_bar_post_rot_resize_adjust_cb;

    /*------------------------------------------------------------------------*/
    lv_obj_t *setting_btn;

    setting_btn = create_menu_bar_btn(menu_bar, menu_bar, \
                                      "pages.setting.menu_bar.airplane", \
                                      ICON_PLANE_SOLID, \
                                      "Airplane");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.wifi", \
                                      ICON_WIFI_SOLID, \
                                      "Wi-Fi");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.bluetooth", \
                                      ICON_SHIELD_SOLID, \
                                      "Bluetooth");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.cellular", \
                                      ICON_TOWER_CELL_SOLID, \
                                      "Cellular");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.ethernet", \
                                      ICON_ETHERNET_SOLID, \
                                      "Ethernet");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.network", \
                                      ICON_NETWORK_WIRED_SOLID, \
                                      "Network");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.hostspot", \
                                      ICON_SHARE_NODES_SOLID, \
                                      "Hostspot");

    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.brightness", \
                                      ICON_CIRCLE_HALF_STROKE_SOLID, \
                                      "Brightness");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.rotation", \
                                      ICON_ROTATE_SOLID, \
                                      "Rotation");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.alert", \
                                      ICON_BELL_SOLID, \
                                      "Alert");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.keyboard", \
                                      ICON_KEYBOARD, \
                                      "Keyboard");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.gnss", \
                                      ICON_SATELLITE_DISH_SOLID, \
                                      "GNSS");

    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.power", \
                                      ICON_PLUG_SOLID, \
                                      "Power");

    /*------------------------------------------------------------------------*/

    return menu_bar;
}

static lv_obj_t *create_setting_container(lv_obj_t *par)
{
    lv_obj_t *cont;
    int32_t obj_w, obj_h;

    if (!par)
        return NULL;

    /* Create container box for the setting menu bar and detail setting */
    cont = create_box(par, SETTING_PAGE_NAME);
    if (!cont)
        return NULL;

    /* Calculate setting container size as percentage of parent size */
    obj_w = calc_pixels(obj_width(par), SETTING_WIDTH);
    obj_h = calc_pixels_remaining(obj_height(par), SETTING_USED_HEIGHT);

    set_gobj_size(cont, obj_w, obj_h);
    lv_obj_set_style_bg_color(cont, lv_color_hex(SETTING_CONT_BG_COLOR), 0);
    align_gobj_to(cont, par, LV_ALIGN_TOP_MID, 0,\
                     calc_pixels(obj_height(par), SETTING_CONTAINTER_ALIGN));
    enable_scale_w(cont);
    set_obj_scale_pad_w(cont, calc_pixels(obj_width(par), (SETTING_PAD_RIGHT
                                             + SETTING_PAD_LEFT)));
    enable_scale_h(cont);
    set_obj_scale_pad_h(cont, calc_pixels(obj_height(par), SETTING_PAD_BOT));

    return cont;
}
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *create_setting_page(lv_obj_t *par)
{
    lv_obj_t *page;
    lv_obj_t *menu_bar;
    lv_obj_t *detail;

    page = create_setting_container(par);
    menu_bar = create_menu_bar(page);
    detail = create_setting_detail(page);
    return page;
}
