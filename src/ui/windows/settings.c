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
#include <crobj.h>

#include "ui/fonts.h"
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
static int32_t create_setting_items(lv_obj_t *view, lv_obj_t *menu)
{
    int32_t ret;
    lv_obj_t *group;
    lv_obj_t *item;

    group = create_menu_group(menu, "wireless");

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_PLANE_SOLID, \
                              &lv_font_montserrat_24, \
                              "Airplane");
    ret = set_item_menu_page(item, view, create_airplane_setting);

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_WIFI_SOLID, \
                              &lv_font_montserrat_24, \
                              "Wi-Fi");
    ret = set_item_menu_page(item, view, create_wifi_setting);

    item = create_menu_option(group, \
                              &lv_font_montserrat_32, \
                              LV_SYMBOL_BLUETOOTH, \
                              &lv_font_montserrat_24, \
                              "Bluetooth");
    ret = set_item_menu_page(item, view, create_bluetooth_setting);

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_TOWER_BROADCAST_SOLID, \
                              &lv_font_montserrat_24, \
                              "Cellular");
    ret = set_item_menu_page(item, view, create_cellular_setting);

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_SHARE_NODES_SOLID, \
                              &lv_font_montserrat_24, \
                              "Hotspot");
    ret = set_item_menu_page(item, view, create_hotspot_setting);


    group = create_menu_group(menu, "net");

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_NETWORK_WIRED_SOLID, \
                              &lv_font_montserrat_24, \
                              "Network");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_FILTER_SOLID, \
                              &lv_font_montserrat_24, \
                              "Filter");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_SHIELD_SOLID, \
                              &lv_font_montserrat_24, \
                              "Shield");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_DOWNLOAD_SOLID, \
                              &lv_font_montserrat_24, \
                              "Download");
    ret = set_item_menu_page(item, view, create_brightness_setting);


    group = create_menu_group(menu, "mobile");

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_PHONE_SOLID, \
                              &lv_font_montserrat_24, \
                              "Phone");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_COMMENT_SOLID, \
                              &lv_font_montserrat_24, \
                              "Message");
    ret = set_item_menu_page(item, view, create_brightness_setting);


    group = create_menu_group(menu, "general");

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_CIRCLE_HALF_STROKE_SOLID, \
                              &lv_font_montserrat_24, \
                              "Brightness");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_ROTATE_SOLID, \
                              &lv_font_montserrat_24, \
                              "Rotation");
    ret = set_item_menu_page(item, view, create_rotation_setting);

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_GEAR_SOLID, \
                              &lv_font_montserrat_24, \
                              "Appearance");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_VOLUME_HIGH_SOLID, \
                              &lv_font_montserrat_24, \
                              "Sound");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_MICROPHONE_SOLID, \
                              &lv_font_montserrat_24, \
                              "Micro");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_BELL_SOLID, \
                              &lv_font_montserrat_24, \
                              "Alert");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_CALENDAR_DAYS_SOLID, \
                              &lv_font_montserrat_24, \
                              "Datetime");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_KEYBOARD, \
                              &lv_font_montserrat_24, \
                              "Keyboard");
    ret = set_item_menu_page(item, view, create_brightness_setting);


    group = create_menu_group(menu, "satelite");

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_SATELLITE_SOLID, \
                              &lv_font_montserrat_24, \
                              "GNSS");
    ret = set_item_menu_page(item, view, create_brightness_setting);


    group = create_menu_group(menu, "system");

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_CIRCLE_INFO_SOLID, \
                              &lv_font_montserrat_24, \
                              "About");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_option(group, \
                              &terminal_icons_32, \
                              ICON_POWER_OFF_SOLID, \
                              &lv_font_montserrat_24, \
                              "Power");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    return 0;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int32_t create_setting_filler(lv_obj_t *par)
{
    lv_obj_t *filler;

    filler = create_box(par, "FILLER");
    if (!filler)
        return -EIO;

    set_size(filler, LV_PCT(100), LV_PCT(70));
    return 0;
}

lv_obj_t *create_setting_window(lv_obj_t *par, const char *name)
{
    lv_obj_t *container, *view, *menu;
    view_ctn_t *v_ctx;
    int32_t ret;

    v_ctx = create_common_menu_view(par, name, NULL, true);
    if (!v_ctx) {
        LOG_ERROR("[%s] create menu view failed, ret %d", name, ret);
        goto err_ctx;
    }

    container = get_view_container(v_ctx);
    view = get_view(v_ctx);

    ret = create_setting_items(view, get_menu(v_ctx));
    if (ret) {
        LOG_ERROR("view [%s] create menu bar items failed, ret %d", name, ret);
        goto err_view;
    }

    /* Set the default active page at the first time menu is rendered */
    ret = set_and_load_window(view, create_airplane_setting);
    if (ret) {
        LOG_ERROR("view [%s] set default page failed, ret %d", \
                  get_name(view), ret);
        goto err_view;
    }

    return view;


err_view:
    if (container)
        remove_obj_and_child(get_meta(container)->id, &get_meta(par)->child);
    else if (view)
        remove_obj_and_child(get_meta(view)->id, &get_meta(par)->child);
    free(v_ctx);
err_ctx:
    return NULL;
}
