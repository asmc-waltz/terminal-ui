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
#include "ui/widget.h"

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
static int32_t create_setting_menu_bar_items(lv_obj_t *menu, lv_obj_t *menu_bar)
{
    int32_t ret;
    lv_obj_t *group;
    lv_obj_t *item;

    group = create_menu_group(menu_bar, "wireless");

    item = create_menu_item(group, NULL, ICON_PLANE_SOLID, NULL, "Airplane");
    ret = set_item_menu_page(item, menu, create_airplane_setting);

    item = create_menu_item(group, NULL, ICON_WIFI_SOLID, NULL, "Wi-Fi");
    ret = set_item_menu_page(item, menu, create_wifi_setting);

    item = create_menu_item(group, &lv_font_montserrat_32, \
                            LV_SYMBOL_BLUETOOTH, NULL, "Bluetooth");
    ret = set_item_menu_page(item, menu, create_bluetooth_setting);

    item = create_menu_item(group, NULL, ICON_TOWER_BROADCAST_SOLID, \
                            NULL, "Cellular");
    ret = set_item_menu_page(item, menu, create_cellular_setting);

    item = create_menu_item(group, NULL, ICON_SHARE_NODES_SOLID, \
                            NULL, "Hotspot");
    ret = set_item_menu_page(item, menu, create_hotspot_setting);


    group = create_menu_group(menu_bar, "net");

    item = create_menu_item(group, NULL, ICON_NETWORK_WIRED_SOLID, \
                            NULL, "Network");
    ret = set_item_menu_page(item, menu, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_FILTER_SOLID, \
                            NULL, "Filter");
    ret = set_item_menu_page(item, menu, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_SHIELD_SOLID, \
                            NULL, "Shield");
    ret = set_item_menu_page(item, menu, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_DOWNLOAD_SOLID, \
                            NULL, "Download");
    ret = set_item_menu_page(item, menu, create_brightness_setting);


    group = create_menu_group(menu_bar, "mobile");
    item = create_menu_item(group, NULL, ICON_PHONE_SOLID, \
                            NULL, "Phone");
    ret = set_item_menu_page(item, menu, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_COMMENT_SOLID, \
                            NULL, "Message");
    ret = set_item_menu_page(item, menu, create_brightness_setting);


    group = create_menu_group(menu_bar, "general");
    item = create_menu_item(group, NULL, ICON_CIRCLE_HALF_STROKE_SOLID, \
                            NULL, "Brightness");
    ret = set_item_menu_page(item, menu, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_ROTATE_SOLID, \
                            NULL, "Rotation");
    ret = set_item_menu_page(item, menu, create_rotation_setting);

    item = create_menu_item(group, NULL, ICON_GEAR_SOLID, \
                            NULL, "Appearance");
    ret = set_item_menu_page(item, menu, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_VOLUME_HIGH_SOLID, \
                            NULL, "Sound");
    ret = set_item_menu_page(item, menu, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_MICROPHONE_SOLID, \
                            NULL, "Micro");
    ret = set_item_menu_page(item, menu, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_BELL_SOLID, \
                            NULL, "Alert");
    ret = set_item_menu_page(item, menu, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_CALENDAR_DAYS_SOLID, \
                            NULL, "Datetime");
    ret = set_item_menu_page(item, menu, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_KEYBOARD, \
                            NULL, "Keyboard");
    ret = set_item_menu_page(item, menu, create_brightness_setting);


    group = create_menu_group(menu_bar, "satelite");

    item = create_menu_item(group, NULL, ICON_SATELLITE_SOLID, \
                            NULL, "GNSS");
    ret = set_item_menu_page(item, menu, create_brightness_setting);


    group = create_menu_group(menu_bar, "system");

    item = create_menu_item(group, NULL, ICON_CIRCLE_INFO_SOLID, \
                            NULL, "About");
    ret = set_item_menu_page(item, menu, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_POWER_OFF_SOLID, \
                            NULL, "Power");
    ret = set_item_menu_page(item, menu, create_brightness_setting);

    return 0;
}

lv_obj_t *create_setting_window(lv_obj_t *par, const char *name, \
                                bool split_view)
{
    lv_obj_t *menu, *menu_bar;
    int32_t ret;

    menu = create_menu_view(par, name, split_view);
    if (!menu)
        return NULL;

    // All sub menu component name will depend on the menu base name
    menu_bar = create_menu_bar(menu);
    if (!menu_bar) {
        LOG_ERROR("Menu [%s] create menu bar failed, ret %d", \
                  get_name(menu), ret);
    }

    ret = create_setting_menu_bar_items(menu, menu_bar);
    if (ret) {
        LOG_ERROR("Menu [%s] create menu bar items failed, ret %d", \
                  get_name(menu), ret);
    }

    /* Set the default active page at the first time menu is rendered */
    ret = set_active_menu_page(menu, create_airplane_setting);
    if (ret) {
        LOG_ERROR("Menu [%s] set default page failed, ret %d", \
                  get_name(menu), ret);
    }

    return menu;
}
