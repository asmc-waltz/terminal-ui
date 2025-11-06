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
#include "comm/net/network.h"

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
static lv_obj_t *enable_wifi_switch = NULL;
static lv_obj_t *ap_holder = NULL;

static char active_ap_ssid[NM_SSID_MAX_LEN];
static wifi_info_t wifi_state;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
static int32_t req_wifi_state()
{
    remote_cmd_t *cmd;

    cmd = create_remote_task_data(WORK_PRIO_NORMAL, WORK_DURATION_SHORT, \
                                  OP_WIFI_STATE);
    if (!cmd)
        return -ENOMEM;

    // NOTE: Command data will be released after the work completes
    return create_remote_task(WORK_PRIO_HIGH, cmd);
}

static int32_t req_cached_ap_list()
{
    remote_cmd_t *cmd;

    cmd = create_remote_task_data(WORK_PRIO_NORMAL, WORK_DURATION_SHORT, \
                                  OP_WIFI_AP_LIST);
    if (!cmd)
        return -ENOMEM;

    // NOTE: Command data will be released after the work completes
    return create_remote_task(WORK_PRIO_HIGH, cmd);
}

/*
 * Enable/Disable Wi-Fi switch update.
 */
static void tongle_wifi_switch_page_update(bool enable)
{
    if (!lv_obj_is_valid(enable_wifi_switch))
        return;

    if (enable)
        lv_obj_add_state(enable_wifi_switch, LV_STATE_CHECKED);
    else
        lv_obj_remove_state(enable_wifi_switch, LV_STATE_CHECKED);
}

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

    enable_wifi_switch = get_box_child(switch_box);
    lv_obj_add_event_cb(enable_wifi_switch, switch_wifi_enable_handler, \
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

    refresh_object_tree_layout(wifi_connected_ap);
    LOG_DEBUG("Wi-Fi connected AP added: %s (%d%%)", ssid, strength);
    return 0;
}

static void remove_wifi_connected_access_point(lv_obj_t *act_ap)
{
    lv_obj_t *par = lv_obj_get_parent(act_ap);

    remove_obj_and_child(get_meta(act_ap)->id, &get_meta(par)->child);
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

static void remove_all_wifi_access_point(lv_obj_t *holder)
{
    int32_t ret = 0;
    ret = remove_children(holder);
    if (ret < 0) {
        LOG_ERROR("Clean Wi-Fi AP holder old data failed, ret=%d", ret);
    }
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
static int32_t create_setting_items(lv_obj_t *par)
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
lv_obj_t *create_wifi_setting(lv_obj_t *par, const char *name)
{
    int32_t ret;
    lv_obj_t *container, *view;
    lv_obj_t *menu;
    menu_view_t *v_ctx;
    char name_buf[64];

    snprintf(name_buf, sizeof(name_buf), "%s_WIFI", name);
    v_ctx = create_menu_view(par, name_buf, true, false);
    if (!v_ctx)
        goto err_view;

    container = v_ctx->container;
    view = v_ctx->view;
    if (!container || !view)
        return NULL;

    menu = create_menu(view);
    if (!menu) {
        LOG_ERROR("[%s] create menu bar failed, ret %d", get_name(view), ret);
        goto err_view;
    }

    lv_obj_add_flag(menu, LV_OBJ_FLAG_SCROLLABLE);

    ret = create_setting_items(menu);
    if (ret) {
        LOG_ERROR("Setting menu bar [%s] create failed, ret %d", \
                  get_name(menu), ret);
        goto err_view;
    }

    ret = req_wifi_state();
    if (ret)
        LOG_WARN("Unable to sync the latest configuration, ret %d", ret);

    ret = req_cached_ap_list();
    if (ret)
        LOG_WARN("Unable to request cached AP list, ret %d", ret);

    return container;

err_view:
    remove_obj_and_child(get_meta(container)->id, &get_meta(par)->child);
    return NULL;
}

int32_t runtime_add_wifi_connected_ap(int8_t strength)
{
    return add_wifi_connected_ap(active_ap_ssid, strength);
}

/*
 * Handle Wi-Fi state command.
 */
int32_t handle_wifi_state(remote_cmd_t *cmd)
{
    bool wifi_enabled;
    const char *active_ap;
    int32_t signal_strength;
    int32_t ret = 0;

    if (!cmd)
        return -EINVAL;

    wifi_enabled = !!cmd->entries[0].value.i32;
    active_ap = cmd->entries[1].key;
    signal_strength = cmd->entries[1].value.i32;

    LOG_INFO("Wi-Fi status: key=[%s], value=[%d]", \
             cmd->entries[0].key, wifi_enabled);

    if (wifi_enabled)
        LOG_INFO("Wi-Fi connected AP: SSID [%s] - Strength [%d]", \
                 active_ap ? active_ap : "(null)", signal_strength);

    /* Async update to avoid blocking UI thread */
    lv_async_call((lv_async_cb_t)tongle_wifi_switch_page_update, \
                  (void *)(intptr_t)wifi_enabled);

    if (wifi_enabled && active_ap && *active_ap) {
        strncpy(active_ap_ssid, active_ap, sizeof(active_ap_ssid) - 1);
        active_ap_ssid[sizeof(active_ap_ssid) - 1] = '\0';

        lv_async_call((lv_async_cb_t)runtime_add_wifi_connected_ap, \
                      (void *)(intptr_t)signal_strength);
    } else {
        if (lv_obj_is_valid(wifi_connected_ap)) {
            lv_async_call(remove_wifi_connected_access_point, \
                          wifi_connected_ap);
        }

        if (lv_obj_is_valid(ap_holder)) {
            lv_async_call(remove_all_wifi_access_point, ap_holder);
        }
    }

    return ret;
}

/*
 * Refresh Wi-Fi available access point holder.
 */
void refresh_available_access_point_holder(void *unused)
{
    int32_t ret;
    const char *ssid;
    int8_t strength;
    int32_t i;

    LOG_DEBUG("Create [%d] Wi-Fi AP", wifi_state.ap_count);

    if (!lv_obj_is_valid(ap_holder)) {
        LOG_ERROR("Wi-Fi access point holder is not available");
        return;
    }

    ret = remove_children(ap_holder);
    if (ret < 0) {
        LOG_ERROR("Unable to clean Wi-Fi AP holder old data, ret=%d", ret);
        return;
    }

    for (i = 0; i < wifi_state.ap_count; ++i) {
        ssid = wifi_state.cached_ap[i].ssid;
        strength = wifi_state.cached_ap[i].strength;

        LOG_TRACE("Wi-Fi AP holder add: SSID [%s] - strength [%d]", \
                  ssid && *ssid ? ssid : "Unknown", strength);

        ret = add_available_wifi_ap(ssid, strength);
        if (ret)
            LOG_WARN("Failed to add AP [%s], ret=%d", \
                     ssid ? ssid : "Unknown", ret);
    }

    ret = refresh_object_tree_layout(ap_holder);
    if (ret < 0) {
        LOG_ERROR("List: Object [%s] rotation failed", get_name(ap_holder));
    }
}

/*
 * Handle Wi-Fi access point command.
 */
int32_t handle_wifi_access_point(remote_cmd_t *cmd)
{
    int32_t ret = 0;
    int32_t i;

    if (!cmd)
        return -EINVAL;

    wifi_state.ap_count = cmd->entry_count;
    LOG_DEBUG("Available Wi-Fi Access point: Count=[%d]", wifi_state.ap_count);

    for (i = 0; i < cmd->entry_count; ++i) {
        const payload_t *entry = &cmd->entries[i];
        const char *key = entry->key ? entry->key : "";

        memset(wifi_state.cached_ap[i].ssid, 0, \
               sizeof(wifi_state.cached_ap[i].ssid));

        strncpy(wifi_state.cached_ap[i].ssid, key, \
                sizeof(wifi_state.cached_ap[i].ssid) - 1);

        wifi_state.cached_ap[i].strength = entry->value.i32;

        LOG_TRACE("Clone Wi-Fi AP: SSID [%s] - strength [%d]", \
                  key[0] ? key : "UNKNOWN", entry->value.i32);
    }

    /* Async refresh UI after state updated */
    lv_async_call(refresh_available_access_point_holder, NULL);

    return ret;
}
