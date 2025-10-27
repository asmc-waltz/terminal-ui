/**
 * @file wifi.c
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
#include "ui/windows.h"
#include "ui/widget.h"
#include "sched/workqueue.h"
#include "comm/cmd_payload.h"

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
static lv_obj_t *wifi_general_group = NULL;
static lv_obj_t *wifi_connected_ap = NULL;
static lv_obj_t *ap_holder = NULL;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void switch_wifi_enable_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *lobj = lv_event_get_target(e);
    bool enable;
    int32_t ret;

    if (code != LV_EVENT_VALUE_CHANGED)
        return;

    enable = lv_obj_has_state(lobj, LV_STATE_CHECKED);
    LOG_TRACE("Wi-Fi: %s", enable ? "On" : "Off");

    ret = create_remote_simple_task(WORK_PRIO_NORMAL, WORK_DURATION_SHORT, \
                                    enable ? OP_WIFI_ENABLE : OP_WIFI_DISABLE);
    if (ret)
        LOG_ERROR("%s Wi-Fi failed, ret %d", \
                  enable ? "Enable" : "Disable", ret);
}

static lv_obj_t *create_wifi_about_section(lv_obj_t *par)
{
    lv_obj_t *group, *sym, *label;
    const char *desc = "Connect to available wireless networks\n"
               "or manage saved connections";

    group = create_vertical_flex_group(par, "WIFI-ABOUT");
    if (!group)
        return NULL;

    sym = create_symbol_box(group, NULL, &terminal_icons_48, ICON_WIFI_SOLID);
    if (!sym)
        return NULL;

    label = create_text_box(group, NULL, &lv_font_montserrat_24, desc);
    if (!label)
        return NULL;

    return group;
}

static int32_t create_wifi_toggle_section(lv_obj_t *par)
{
    lv_obj_t *group, *label, *switch_box;

    group = create_horizontal_flex_group(par, "WIFI-SWITCH");
    if (!group)
        return -ENOMEM;

    set_padding(group, 10, 10, 10, 10);

    label = create_text_box(group, NULL, &lv_font_montserrat_24, "Wi-Fi");
    if (!label)
        return -EIO;

    switch_box = create_switch_box(group, NULL);
    if (!switch_box)
        return -EIO;

    lv_obj_add_event_cb(get_box_child(switch_box), \
                switch_wifi_enable_handler, \
                LV_EVENT_ALL, NULL);

    return 0;
}

static lv_obj_t *create_wifi_ap_item(lv_obj_t *parent, \
                     const char *ssid, \
                     int8_t strength)
{
    lv_obj_t *group, *label;
    char str_buf[10];

    group = create_horizontal_flex_group(parent, ssid);
    if (!group)
        return NULL;

    set_padding(group, 10, 10, 10, 10);

    label = create_text_box(group, NULL, &lv_font_montserrat_24, ssid);
    if (!label)
        return NULL;

    snprintf(str_buf, sizeof(str_buf), "%d%%", strength);
    label = create_text_box(group, NULL, &lv_font_montserrat_24, str_buf);
    if (!label)
        return NULL;

    return group;
}

/*
 *  Update or add current connected Wi-Fi AP
 */
static int32_t add_wifi_connected_ap(const char *ssid, int8_t strength)
{
    lv_obj_t *group;
    const char *old_name = NULL;

    if (!ssid)
        return -EINVAL;

    if (!lv_obj_is_valid(wifi_general_group))
        return -EIO;

    /* Remove previous connected AP if exists */
    if (lv_obj_is_valid(wifi_connected_ap)) {
        old_name = get_name(wifi_connected_ap);

        LOG_INFO("Wi-Fi connected AP changed [%s] -> [%s - %d%%]", \
             old_name ? old_name : "(unknown)", ssid, strength);

        remove_obj_and_child_by_name(old_name, \
                         &get_meta(wifi_general_group)->child);

        wifi_connected_ap = NULL;
    }

    group = create_wifi_ap_item(wifi_general_group, ssid, strength);
    if (!group)
        return -ENOMEM;

    wifi_connected_ap = group;

    set_border_side(group, LV_BORDER_SIDE_TOP);
    lv_obj_set_style_border_width(group, 2, 0);
    lv_obj_set_style_border_color(group, lv_color_black(), 0);

    LOG_DEBUG("Wi-Fi connected AP added: %s (%d%%)", ssid, strength);
    return 0;
}

static int32_t add_available_wifi_ap(const char *ssid, int8_t strength)
{
    lv_obj_t *group;

    if (!ssid)
        return -EINVAL;

    if (!lv_obj_is_valid(ap_holder))
        return -EIO;

    group = create_wifi_ap_item(ap_holder, ssid, strength);
    if (!group)
        return -ENOMEM;

    /* Add border only if not the first AP */
    if (lv_obj_get_child(ap_holder, 0) != group) {
        set_border_side(group, LV_BORDER_SIDE_TOP);
        lv_obj_set_style_border_width(group, 2, 0);
        lv_obj_set_style_border_color(group, lv_color_black(), 0);
    }

    return 0;
}

static int32_t create_wifi_filler(lv_obj_t *par)
{
    lv_obj_t *filler;

    filler = create_box(par, "FILLER");
    if (!filler)
        return -EIO;

    set_size(filler, LV_PCT(100), LV_PCT(70));
    return 0;
}

/*---------------------------------------------*
 *  Main Wi-Fi settings item builder
 *---------------------------------------------*/
static int32_t create_wifi_setting_items(lv_obj_t *par)
{
    lv_obj_t *about_group, *general_group;
    int32_t ret;

    if (!par)
        return -EINVAL;

    /* Section: About */
    about_group = create_wifi_about_section(par);
    if (!about_group)
        return -EIO;

    /* Section: General */
    general_group = create_vertical_flex_group(about_group, "WIFI-GENERAL");
    if (!general_group)
        return -ENOMEM;

    wifi_general_group = general_group;

    set_padding(general_group, 0, 0, 20, 0);
    set_row_padding(general_group, 0);
    set_border_side(general_group, LV_BORDER_SIDE_TOP);
    lv_obj_set_style_border_width(general_group, 2, 0);
    lv_obj_set_style_border_color(general_group, lv_color_black(), 0);

    ret = create_wifi_toggle_section(general_group);
    if (ret < 0)
        return ret;

    ap_holder = create_vertical_flex_group(par, "WIFI-AP-HOLDER");
    if (!ap_holder)
        return -ENOMEM;

    set_padding(ap_holder, 0, 0, 40, 20);
    set_row_padding(ap_holder, 0);

    ret = create_wifi_filler(par);
    if (ret < 0)
        return ret;

    LOG_DEBUG("Wi-Fi setting items created successfully");
    return 0;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t *create_wifi_setting(lv_obj_t *menu, lv_obj_t *par, const char *name)
{
    int32_t ret;
    lv_obj_t *page;

    page = create_menu_page(menu, par, name);
    if (!page)
        return NULL;

    ret = create_wifi_setting_items(page);
    if (ret) {
        LOG_ERROR("Setting page [%s] create failed, ret %d", \
                  get_name(page), ret);
        remove_obj_and_child(get_meta(page)->id, &get_meta(par)->child);
        return NULL;
    }

    return page;
}
