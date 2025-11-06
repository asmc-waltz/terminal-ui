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
#include "ui/widget/menu.h"

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

    item = create_menu_item(group, NULL, ICON_PLANE_SOLID, NULL, "Airplane");
    ret = set_item_menu_page(item, view, create_airplane_setting);

    item = create_menu_item(group, NULL, ICON_WIFI_SOLID, NULL, "Wi-Fi");
    ret = set_item_menu_page(item, view, create_wifi_setting);

    item = create_menu_item(group, &lv_font_montserrat_32, \
                            LV_SYMBOL_BLUETOOTH, NULL, "Bluetooth");
    ret = set_item_menu_page(item, view, create_bluetooth_setting);

    item = create_menu_item(group, NULL, ICON_TOWER_BROADCAST_SOLID, \
                            NULL, "Cellular");
    ret = set_item_menu_page(item, view, create_cellular_setting);

    item = create_menu_item(group, NULL, ICON_SHARE_NODES_SOLID, \
                            NULL, "Hotspot");
    ret = set_item_menu_page(item, view, create_hotspot_setting);


    group = create_menu_group(menu, "net");

    item = create_menu_item(group, NULL, ICON_NETWORK_WIRED_SOLID, \
                            NULL, "Network");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_FILTER_SOLID, \
                            NULL, "Filter");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_SHIELD_SOLID, \
                            NULL, "Shield");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_DOWNLOAD_SOLID, \
                            NULL, "Download");
    ret = set_item_menu_page(item, view, create_brightness_setting);


    group = create_menu_group(menu, "mobile");
    item = create_menu_item(group, NULL, ICON_PHONE_SOLID, \
                            NULL, "Phone");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_COMMENT_SOLID, \
                            NULL, "Message");
    ret = set_item_menu_page(item, view, create_brightness_setting);


    group = create_menu_group(menu, "general");
    item = create_menu_item(group, NULL, ICON_CIRCLE_HALF_STROKE_SOLID, \
                            NULL, "Brightness");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_ROTATE_SOLID, \
                            NULL, "Rotation");
    ret = set_item_menu_page(item, view, create_rotation_setting);

    item = create_menu_item(group, NULL, ICON_GEAR_SOLID, \
                            NULL, "Appearance");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_VOLUME_HIGH_SOLID, \
                            NULL, "Sound");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_MICROPHONE_SOLID, \
                            NULL, "Micro");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_BELL_SOLID, \
                            NULL, "Alert");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_CALENDAR_DAYS_SOLID, \
                            NULL, "Datetime");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_KEYBOARD, \
                            NULL, "Keyboard");
    ret = set_item_menu_page(item, view, create_brightness_setting);


    group = create_menu_group(menu, "satelite");

    item = create_menu_item(group, NULL, ICON_SATELLITE_SOLID, \
                            NULL, "GNSS");
    ret = set_item_menu_page(item, view, create_brightness_setting);


    group = create_menu_group(menu, "system");

    item = create_menu_item(group, NULL, ICON_CIRCLE_INFO_SOLID, \
                            NULL, "About");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    item = create_menu_item(group, NULL, ICON_POWER_OFF_SOLID, \
                            NULL, "Power");
    ret = set_item_menu_page(item, view, create_brightness_setting);

    return 0;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
view_ctn_t *create_common_setting_view(lv_obj_t *par, const char *name,
                                       bool root, bool split)
{
    view_ctn_t *v_ctx;
    lv_obj_t *container, *view, *menu;
    int32_t ret = 0;
    char name_buf[64];

    snprintf(name_buf, sizeof(name_buf), "%s_SETTING", name);

    v_ctx = create_menu_view(par, name_buf, root, split);
    if (!v_ctx) {
        LOG_ERROR("[%s] create menu view failed", name);
        return NULL;
    }

    container = get_view_container(v_ctx);
    view = get_view(v_ctx);
    if (!view) {
        LOG_ERROR("[%s] invalid container or view", name);
        goto err_view;
    }

    menu = create_menu(view);
    if (!menu) {
        LOG_ERROR("[%s] create menu bar failed", get_name(view));
        goto err_view;
    }

    return v_ctx;

err_view:
    if (container)
        remove_obj_and_child(get_meta(container)->id, \
                             &get_meta(par)->child);
    else
        remove_obj_and_child(get_meta(view)->id, &get_meta(par)->child);

    free(v_ctx);

    return NULL;
}

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
    lv_obj_t *view, *menu;
    int32_t ret;
    view_ctn_t *v_ctx;

    v_ctx = create_menu_view(par, name, false, true);
    if (!v_ctx)
        return NULL;

    view = v_ctx->view;

    // All sub view component name will depend on the view base name
    menu = create_menu(view);
    if (!menu) {
        LOG_ERROR("view [%s] create menu bar failed, ret %d", \
                  get_name(view), ret);
    }

    ret = create_setting_items(view, menu);
    if (ret) {
        LOG_ERROR("view [%s] create menu bar items failed, ret %d", \
                  get_name(view), ret);
    }

    /* Set the default active page at the first time menu is rendered */
    ret = set_active_window(view, create_airplane_setting);
    if (ret) {
        LOG_ERROR("view [%s] set default page failed, ret %d", \
                  get_name(view), ret);
    }

    return view;
}
