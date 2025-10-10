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
static void menu_item_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    switch (code) {
    case LV_EVENT_PRESSED:
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xFF6633), 0);
        break;
    case LV_EVENT_RELEASED:
        lv_obj_set_style_bg_color(obj, lv_color_hex(bg_color(1)), 0);
        break;
    case LV_EVENT_CLICKED:
        LV_LOG_USER("Box clicked!");
        break;
    default:
        break;
    }
}

/*
 * Menu item: an entry to open a specific setting window.
 * Each menu item consists of a symbol (icon) and a title label.
 */
static lv_obj_t *create_menu_item(lv_obj_t *par, const char *name, \
                                  const char *sym_index, const char *title)
{
    lv_obj_t *item;
    lv_obj_t *sym;
    lv_obj_t *label;

    if (!par)
        return NULL;

    /*----------------------------------
     * Create container (menu item)
     *----------------------------------*/
    item = create_flex_layout_object(par, name);
    if (!item)
        return NULL;

    set_flex_layout_flow(item, LV_FLEX_FLOW_ROW);
    set_flex_cell_type(item);
    set_gobj_size(item, LV_PCT(100), 50);
    lv_obj_set_style_pad_column(item, 8, 0);
    lv_obj_set_style_pad_left(item, 20, 0);
    lv_obj_set_style_pad_right(item, 20, 0);

    /*----------------------------------
     * Style
     *----------------------------------*/
    lv_obj_set_style_border_side(item, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(item, 2, 0);
    lv_obj_set_style_border_color(item, lv_color_black(), 0);

    lv_obj_set_style_bg_color(item, lv_color_hex(bg_color(1)), 0);
    lv_obj_add_flag(item, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(item, menu_item_event_handler, LV_EVENT_ALL, NULL);

    /*----------------------------------
     * Flex alignment
     *----------------------------------*/
    lv_obj_set_style_flex_main_place(item, LV_FLEX_ALIGN_SPACE_BETWEEN, 0);
    lv_obj_set_style_flex_cross_place(item, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(item, LV_FLEX_ALIGN_CENTER, 0);

    /*----------------------------------
     * Create children: symbol + title
     *----------------------------------*/
    sym = create_symbol_box(item, NULL, &terminal_icons_20, sym_index);
    if (sym)
        set_flex_cell_type(sym);

    label = create_text_box(item, NULL, &lv_font_montserrat_24, title);
    if (label)
        set_flex_cell_type(label);

    return item;
}
 
/*
 * Setting group: a child container inside the menu bar.
 * This container uses a flex layout with vertical flow,
 * and its size depends on the number of child elements.
 */
static lv_obj_t *create_menu_group(lv_obj_t *par, const char *name)
{
    lv_obj_t *group;

    if (!par)
        return NULL;

    group = create_flex_layout_object(par, name);
    if (!group)
        return NULL;

    /*------------------------------
     * Layout configuration
     *-----------------------------*/
    set_flex_layout_flow(group, LV_FLEX_FLOW_COLUMN);
    set_flex_cell_type(group);

    /*------------------------------
     * Sizing and spacing
     *-----------------------------*/
    set_gobj_size(group, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(group, 20, 0);

    /*------------------------------
     * Flex alignment
     *-----------------------------*/
    // lv_obj_set_style_flex_main_place(group, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(group, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(group, LV_FLEX_ALIGN_CENTER, 0);

    return group;
}

static int32_t create_menu_bar_items(lv_obj_t *par)
{
    lv_obj_t *group;
    lv_obj_t *item;

    group = create_menu_group(par, "wireless");
    item = create_menu_item(group, "airplane", \
                            ICON_PLANE_SOLID, "Airplane");
    item = create_menu_item(group, "wifi", \
                            ICON_WIFI_SOLID, "Wi-Fi");
    item = create_menu_item(group, "bluetooth", \
                            ICON_TOOLBOX_SOLID, "Bluetooth");
    item = create_menu_item(group, "cellular", \
                            ICON_TOWER_BROADCAST_SOLID, "Cellular");
    item = create_menu_item(group, "hostspot", \
                            ICON_SHARE_NODES_SOLID, "Hostspot");

    group = create_menu_group(par, "net");
    item = create_menu_item(group, "network", \
                            ICON_NETWORK_WIRED_SOLID, "Network");
    item = create_menu_item(group, "ethernet", \
                            ICON_ETHERNET_SOLID, "Ethernet");
    item = create_menu_item(group, "filter", \
                            ICON_FILTER_SOLID, "Filter");
    item = create_menu_item(group, "shield", \
                            ICON_SHIELD_SOLID, "Shield");
    item = create_menu_item(group, "download", \
                            ICON_DOWNLOAD_SOLID, "Download");


    group = create_menu_group(par, "mobile");
    item = create_menu_item(group, "phone", \
                            ICON_PHONE_SOLID, "Phone");
    item = create_menu_item(group, "message", \
                            ICON_COMMENT_SOLID, "Message");

    group = create_menu_group(par, "general");
    item = create_menu_item(group, "brightness", \
                            ICON_CIRCLE_HALF_STROKE_SOLID, "Brightness");
    item = create_menu_item(group, "rotation", \
                            ICON_ROTATE_SOLID, "Rotation");
    item = create_menu_item(group, "appearance ", \
                            ICON_GEAR_SOLID, "Appearance");
    item = create_menu_item(group, "sound", \
                            ICON_VOLUME_HIGH_SOLID, "Sound");
    item = create_menu_item(group, "micro", \
                            ICON_MICROPHONE_SOLID, "Micro");
    item = create_menu_item(group, "alert", \
                            ICON_BELL_SOLID, "Alert");
    item = create_menu_item(group, "datetime", \
                            ICON_CALENDAR_DAYS_SOLID, "Datetime");
    item = create_menu_item(group, "keyboard", \
                            ICON_KEYBOARD, "Keyboard");

    group = create_menu_group(par, "satelite");
    item = create_menu_item(group, "gnss", \
                            ICON_SATELLITE_SOLID, "GNSS");

    group = create_menu_group(par, "system");
    item = create_menu_item(group, "About", \
                            ICON_CIRCLE_INFO_SOLID, "About");
    item = create_menu_item(group, "power", \
                            ICON_POWER_OFF_SOLID, "Power");

    return 0;
}

/*
 * Menu bar: a left-side panel containing system settings.
 * This bar is organized in a vertical flex layout, and itself
 * contains one or more grouped flex containers.
 */
static lv_obj_t *create_menu_bar(lv_obj_t *par, const char *name)
{
    lv_obj_t *menu_bar;

    if (!par)
        return NULL;

    menu_bar = create_flex_layout_object(par, name);
    if (!menu_bar)
        return NULL;

    /*------------------------------
     * Layout configuration
     *-----------------------------*/
    set_flex_layout_flow(menu_bar, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(menu_bar, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(menu_bar, LV_FLEX_ALIGN_CENTER, 0);
    // lv_obj_set_style_flex_main_place(menu_bar, LV_FLEX_ALIGN_CENTER, 0);

    /*------------------------------
     * Padding and spacing
     *-----------------------------*/
    lv_obj_set_style_pad_all(menu_bar, 4, 0);
    lv_obj_set_style_pad_row(menu_bar, 20, 0);
    lv_obj_set_style_pad_column(menu_bar, 20, 0);

    /*------------------------------
     * Visual style
     *-----------------------------*/
    lv_obj_set_style_bg_color(menu_bar, lv_color_hex(bg_color(120)), 0);

    /*------------------------------
     * Scrollbar cleanup
     *-----------------------------*/
    lv_obj_set_style_width(menu_bar, 1, LV_PART_SCROLLBAR);
    lv_obj_set_style_pad_all(menu_bar, 0, LV_PART_SCROLLBAR);
    lv_obj_set_style_margin_all(menu_bar, 0, LV_PART_SCROLLBAR);

    create_menu_bar_items(menu_bar);
    return menu_bar;
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

    lv_obj_set_style_pad_all(setting, 20, 0);

    return setting;
}

int32_t create_setting_content(lv_obj_t *window)
{
    int32_t ret;

    if (!window)
        return -EINVAL;

    set_grid_layout_gap(window, 0, 5, 0, 0);
    set_grid_layout_align(window, \
                          LV_GRID_ALIGN_SPACE_BETWEEN, \
                          LV_GRID_ALIGN_SPACE_BETWEEN);
    lv_obj_set_style_bg_color(window, lv_color_hex(bg_color(80)), 0);
    lv_obj_set_style_pad_all(window, 20, 0);

    /**************************************/
    ret = add_grid_layout_row_dsc(window, LV_GRID_FR(98));
    if (ret) {
        LOG_ERROR("Add descriptor info failed");
    }
    /**************************************/
    ret = add_grid_layout_col_dsc(window, LV_GRID_FR(35));
    if (ret) {
        LOG_ERROR("Add descriptor info failed");
    }

    ret = add_grid_layout_col_dsc(window, LV_GRID_FR(65));
    if (ret) {
        LOG_ERROR("Add descriptor info failed");
    }
    /**************************************/

    apply_grid_layout_config(window);


    lv_obj_t *menu_bar;
    menu_bar = create_menu_bar(window, "menu_bar");
    set_grid_cell_align(menu_bar, LV_GRID_ALIGN_STRETCH, 0, 1, \
                        LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_style_bg_color(menu_bar, lv_color_hex(bg_color(100)), 0);

    lv_obj_t *setting_detail;
    setting_detail = create_box(window, "detail");
    set_grid_cell_align(setting_detail, LV_GRID_ALIGN_STRETCH, 1, 1,
                        LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_style_bg_color(setting_detail, lv_color_hex(bg_color(100)), 0);

    return 0;
}
