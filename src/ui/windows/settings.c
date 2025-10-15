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
    lv_obj_t *group;
    lv_obj_t *item;

    group = create_menu_group(menu_bar, "wireless");
    item = create_menu_item(menu, group, \
                            ICON_PLANE_SOLID, "Airplane", \
                            create_brightness2_setting \
                            );
    item = create_menu_item(menu, group, \
                            ICON_WIFI_SOLID, "Wi-Fi", \
                            create_brightness_setting \
                            );
    item = create_menu_item(menu, group, \
                            ICON_TOOLBOX_SOLID, "Bluetooth", \
                            create_brightness_setting \
                            );
    item = create_menu_item(menu, group, \
                            ICON_TOWER_BROADCAST_SOLID, "Cellular", \
                            create_brightness_setting \
                            );
    item = create_menu_item(menu, group, \
                            ICON_SHARE_NODES_SOLID, "Hostspot", \
                            create_brightness_setting \
                            );

    group = create_menu_group(menu_bar, "net");
    item = create_menu_item(menu, group, \
                            ICON_NETWORK_WIRED_SOLID, "Network", \
                            create_brightness_setting \
                            );
    item = create_menu_item(menu, group, \
                            ICON_ETHERNET_SOLID, "Ethernet", \
                            create_brightness_setting \
                            );
    item = create_menu_item(menu, group, \
                            ICON_FILTER_SOLID, "Filter", \
                            create_brightness_setting \
                            );
    item = create_menu_item(menu, group, \
                            ICON_SHIELD_SOLID, "Shield", \
                            create_brightness_setting \
                            );
    item = create_menu_item(menu, group, \
                            ICON_DOWNLOAD_SOLID, "Download", \
                            create_brightness_setting \
                            );


    group = create_menu_group(menu_bar, "mobile");
    item = create_menu_item(menu, group, \
                            ICON_PHONE_SOLID, "Phone", \
                            create_brightness_setting \
                            );
    item = create_menu_item(menu, group, \
                            ICON_COMMENT_SOLID, "Message", \
                            create_brightness_setting \
                            );

    group = create_menu_group(menu_bar, "general");
    item = create_menu_item(menu, group, \
                            ICON_CIRCLE_HALF_STROKE_SOLID, "Brightness", \
                            create_brightness_setting \
                            );
    item = create_menu_item(menu, group, \
                            ICON_ROTATE_SOLID, "Rotation", \
                            create_brightness_setting \
                            );
    item = create_menu_item(menu, group, \
                            ICON_GEAR_SOLID, "Appearance", \
                            create_brightness_setting \
                            );
    item = create_menu_item(menu, group, \
                            ICON_VOLUME_HIGH_SOLID, "Sound", \
                            create_brightness_setting \
                            );
    item = create_menu_item(menu, group, \
                            ICON_MICROPHONE_SOLID, "Micro", \
                            create_brightness_setting \
                            );
    item = create_menu_item(menu, group, \
                            ICON_BELL_SOLID, "Alert", \
                            create_brightness_setting \
                            );
    item = create_menu_item(menu, group, \
                            ICON_CALENDAR_DAYS_SOLID, "Datetime", \
                            create_brightness_setting \
                            );
    item = create_menu_item(menu, group, \
                            ICON_KEYBOARD, "Keyboard", \
                            create_brightness_setting \
                            );

    group = create_menu_group(menu_bar, "satelite");
    item = create_menu_item(menu, group, \
                            ICON_SATELLITE_SOLID, "GNSS", \
                            create_brightness_setting \
                            );

    group = create_menu_group(menu_bar, "system");
    item = create_menu_item(menu, group, \
                            ICON_CIRCLE_INFO_SOLID, "About", \
                            create_brightness_setting \
                            );
    item = create_menu_item(menu, group, \
                            ICON_POWER_OFF_SOLID, "Power", \
                            create_brightness_setting \
                            );

    return 0;
}

lv_obj_t *create_setting_window(lv_obj_t *par, const char *name)
{
    lv_obj_t *menu, *menu_bar;
    int32_t ret;

    menu = create_menu(par, name);
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
    ret = set_active_menu_page(menu, create_brightness_setting);
    if (ret) {
        LOG_ERROR("Menu [%s] set default page failed, ret %d", \
                  get_name(menu), ret);
    }

    return menu;
}
