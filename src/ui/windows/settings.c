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
typedef struct {
    lv_obj_t *menu_ctn;
    lv_obj_t *act_menu_item;
    lv_obj_t *page_ctn;
    lv_obj_t *act_page;
    bool hid_detail;            // Hide detail setting in vertical screen;
} menu_ctx_t;

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
        lv_obj_t *win_setting = get_obj_by_name(WINDOW_SETTING, \
                                       &get_meta(lv_screen_active())->child);
        // lv_obj_t *set = create_brightness_setting(win_setting, "NEW");
        //
        // refresh_object_tree_layout(set);

        menu_ctx_t *menu_ctx = get_internal_data(win_setting);
        set_menu_page_active(win_setting, WINDOW_SETTING".detail.brightness", create_brightness_setting);

        // refresh_object_tree_layout(brightness_win);
        refresh_object_tree_layout(menu_ctx->act_page);
        break;
    case LV_EVENT_CLICKED:
        LV_LOG_USER("Box clicked!");
        break;
    default:
        break;
    }
}

int32_t show_and_hide_detail_cb(lv_obj_t *lobj)
{
    int32_t ret = 0;
    static bool detail_visible = true;
    static bool pending_detail_create = false;
    menu_ctx_t *menu_ctx = get_internal_data(lobj);

    int32_t scr_rot = get_scr_rotation();

    if (scr_rot == ROTATION_90 || scr_rot == ROTATION_270) {
        if (detail_visible) {
            ret = remove_grid_layout_last_row_dsc(lobj);
            if (ret) {
                LOG_ERROR("Remove detail layout failed, ret %d", ret);
            } else {
                detail_visible = false;
                pending_detail_create = true;
                menu_ctx->page_ctn = NULL;
                menu_ctx->act_page = NULL;
            }
        }
    } else if (scr_rot == ROTATION_0 || scr_rot == ROTATION_180) {
        if (!detail_visible) {
            ret = add_grid_layout_col_dsc(lobj, LV_GRID_FR(65));
            if (ret) {
                LOG_ERROR("Add detail layout failed, ret %d", ret);
            } else {
                detail_visible = true;
            }

            // User access menu or rotate from 0/180
            // TODO: remove before rotation -> fix parent size issue
            if (menu_ctx->act_page) {
                remove_obj_and_child_by_name(get_name(menu_ctx->act_page), \
                                             &get_meta(lobj)->child);
            }
            menu_ctx->act_page = NULL;
        }
    }

    apply_grid_layout_config(lobj);

    if (detail_visible && pending_detail_create) {
        lv_obj_t *detail = create_box(lobj, WINDOW_SETTING ".detail");
        if (detail) {
            menu_ctx->page_ctn = detail;
        }

        if (scr_rot == ROTATION_0)
            set_grid_cell_align(detail,
                                LV_GRID_ALIGN_STRETCH, 1, 1,
                                LV_GRID_ALIGN_STRETCH, 0, 1);
        else {
            set_grid_cell_align(detail,
                                LV_GRID_ALIGN_STRETCH, 0, 1,
                                LV_GRID_ALIGN_STRETCH, 0, 1);
            get_meta(detail)->data.rotation = ROTATION_180;
        }

        lv_obj_set_style_bg_color(detail, lv_color_hex(bg_color(100)), 0);

        // lv_obj_t *brightness_win = create_brightness_setting(detail,
        //                                  WINDOW_SETTING ".detail.brightness");
        //
        set_menu_page_active(lobj, WINDOW_SETTING".detail.brightness", create_brightness_setting);

        // refresh_object_tree_layout(brightness_win);
        refresh_object_tree_layout(menu_ctx->act_page);
        pending_detail_create = false;
    }

    return 0;
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
    lv_obj_t *first_child;

    if (!par)
        return NULL;

    /* Create container (menu item) */
    item = create_flex_layout_object(par, name);
    if (!item)
        return NULL;

    set_flex_layout_flow(item, LV_FLEX_FLOW_ROW);

    set_size(item, LV_PCT(100), 50);
    set_padding(item, 0, 0, 20, 20);
    lv_obj_set_style_bg_color(item, lv_color_hex(bg_color(1)), 0);

    lv_obj_add_flag(item, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(item, menu_item_event_handler, LV_EVENT_ALL, NULL);

    /* Style */
    first_child = lv_obj_get_child(par, 0);
    if (first_child != item) {
        set_border_side(item, LV_BORDER_SIDE_TOP);
        lv_obj_set_style_border_width(item, 2, 0);
        lv_obj_set_style_border_color(item, lv_color_black(), 0);
    }

    /* Flex alignment */
    set_flex_layout_align(item, \
                          LV_FLEX_ALIGN_SPACE_BETWEEN, \
                          LV_FLEX_ALIGN_CENTER, \
                          LV_FLEX_ALIGN_CENTER);

    /* Create children: symbol + title */
    sym = create_symbol_box(item, NULL, &terminal_icons_32, sym_index);
    if (!sym)
        LOG_ERROR("Create symbol failed");

    label = create_text_box(item, NULL, &lv_font_montserrat_24, title);
    if (!label)
        LOG_ERROR("Create label failed");

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

    /* Visual style */
    set_size(group, LV_PCT(100), LV_SIZE_CONTENT);
    /* Padding and spacing */
    set_padding(group, 20, 20, 20, 20);
    /* Layout configuration */
    set_flex_layout_flow(group, LV_FLEX_FLOW_COLUMN);


    /*------------------------------
     * Flex alignment
     *-----------------------------*/
    set_flex_layout_align(group, \
                          LV_FLEX_ALIGN_START, \
                          LV_FLEX_ALIGN_CENTER, \
                          LV_FLEX_ALIGN_CENTER);

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

    /* Visual style */
    lv_obj_set_style_bg_color(menu_bar, lv_color_hex(bg_color(120)), 0);
    set_size(menu_bar, LV_PCT(98), LV_PCT(98));
    set_align(menu_bar, par, LV_ALIGN_CENTER, 0, 0);
    /* Padding and spacing */
    set_padding(menu_bar, 4, 4, 4, 4);
    set_row_padding(menu_bar, 20);
    /* Scrollbar cleanup */
    lv_obj_set_style_width(menu_bar, 1, LV_PART_SCROLLBAR);
    lv_obj_set_style_pad_all(menu_bar, 0, LV_PART_SCROLLBAR);
    lv_obj_set_style_margin_all(menu_bar, 0, LV_PART_SCROLLBAR);

    /* Layout configuration */
    set_flex_layout_flow(menu_bar, LV_FLEX_FLOW_COLUMN);

    /* Flex alignment */
    set_flex_layout_align(menu_bar, \
                          LV_FLEX_ALIGN_START, \
                          LV_FLEX_ALIGN_CENTER, \
                          LV_FLEX_ALIGN_CENTER);

    create_menu_bar_items(menu_bar);

    return menu_bar;
}

static int32_t create_menu_content(lv_obj_t *window, menu_ctx_t *menu_ctx)
{
    int32_t ret;
    lv_obj_t *menu_ctn, *page_ctn;
    lv_obj_t *menu, *page;

    if (!window)
        return -EINVAL;

    /* Create the default menu bar as the left side colum */

    menu_ctn = create_box(window, WINDOW_SETTING".menu_cnt");
    if (!menu_ctn)
        return -EINVAL;

    menu_ctx->menu_ctn = menu_ctn;
    set_grid_cell_align(menu_ctn, \
                        LV_GRID_ALIGN_STRETCH, 0, 1, \
                        LV_GRID_ALIGN_STRETCH, 0, 1);
    // lv_obj_set_style_bg_color(menu_ctn, lv_color_hex(bg_color(100)), 0);

    /* Create the default page_ctn space as the right side colum */
    page_ctn = create_box(window, WINDOW_SETTING".page_ctn");
    if (!page_ctn)
        return -EINVAL;

    menu_ctx->page_ctn = page_ctn;
    set_grid_cell_align(page_ctn, \
                        LV_GRID_ALIGN_STRETCH, 1, 1,
                        LV_GRID_ALIGN_STRETCH, 0, 1);
    // lv_obj_set_style_bg_color(page_ctn, lv_color_hex(bg_color(100)), 0);





    menu = create_menu_bar(menu_ctn, WINDOW_SETTING".menu");
    if (!menu)
        return -EINVAL;
    return 0;
}


/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t *create_menu(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj;
    int32_t ret;
    menu_ctx_t *menu_ctx;

    if (!par)
        return NULL;

    menu_ctx = (menu_ctx_t *)calloc(1, sizeof(menu_ctx_t));
    if (!menu_ctx)
        return NULL;

    /*-----------------------------------------
     * Create menu container using grid layout
     *----------------------------------------*/
    lobj = create_grid_layout_object(par, name);
    if (!lobj)
        return NULL;

    ret = add_grid_layout_row_dsc(lobj, LV_GRID_FR(98));
    if (ret)
        LOG_ERROR("Layout [%s] Row descriptor failed", get_name(lobj));

    ret = add_grid_layout_col_dsc(lobj, LV_GRID_FR(35));
    if (ret)
        LOG_ERROR("Layout [%s] Column descriptor failed", get_name(lobj));

    ret = add_grid_layout_col_dsc(lobj, LV_GRID_FR(65));
    if (ret)
        LOG_ERROR("Layout [%s] Column descriptor failed", get_name(lobj));

    apply_grid_layout_config(lobj);

    set_grid_layout_align(lobj, \
                          LV_GRID_ALIGN_SPACE_BETWEEN, \
                          LV_GRID_ALIGN_SPACE_BETWEEN);

    /*-----------------------------------------
     * Base style setup
     *----------------------------------------*/
    lv_obj_set_style_bg_color(lobj, lv_color_hex(bg_color(80)), 0);
    lv_obj_set_style_radius(lobj, 16, 0);
    set_column_padding(lobj, 8);
    ret = set_padding(lobj, 20, 20, 20, 20);
    if (ret)
        LOG_WARN("Layout [%s] set padding failed, ret %d", \
                 get_name(lobj), ret);

    create_menu_content(lobj, menu_ctx);

    set_internal_data(lobj, menu_ctx);
    get_meta(lobj)->data.post_children_rotate_cb = show_and_hide_detail_cb;

    return lobj;
}

int32_t set_menu_page_active(lv_obj_t *lobj, const char *name, \
                                    lv_obj_t *(*create_page_cb)(lv_obj_t *, \
                                                                const char *))
{
    menu_ctx_t *menu_ctx;
    lv_obj_t *page;

    if (!lobj)
        return -EINVAL;

    menu_ctx = get_internal_data(lobj);
    if (!menu_ctx)
        return -EIO;

    if (menu_ctx->act_page) {
        LOG_WARN("Menu page [%s] is activating. ignore the current request", \
                 get_name(menu_ctx->act_page));
        return 0;
    }

    if (menu_ctx->page_ctn) {
        page = create_page_cb(menu_ctx->page_ctn, name);
        if (!page)
            return -EIO;
    } else {
        LOG_WARN("Reuse the menu bar space");
        page = create_page_cb(menu_ctx->menu_ctn, name);
        if (!page)
            return -EIO;
    }

    menu_ctx->act_page = page;

    return 0;
}
