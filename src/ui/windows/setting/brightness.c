/**
 * @file brightness.c
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
#include "ui/widget/menu.h"
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
static lv_obj_t *brightness_slider = NULL;
static lv_obj_t *als_switch = NULL;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
static int32_t req_backlight_state()
{
    remote_cmd_t *cmd;

    cmd = create_remote_task_data(WORK_PRIO_NORMAL, WORK_DURATION_SHORT, \
                                  OP_BACKLIGHT_STATE);
    if (!cmd)
        return -ENOMEM;

    // NOTE: Command data will be released after the work completes
    return create_remote_task(WORK_PRIO_HIGH, cmd);
}

static int32_t req_set_brightness(int32_t value)
{
    remote_cmd_t *cmd;

    cmd = create_remote_task_data(WORK_PRIO_NORMAL, WORK_DURATION_SHORT, \
                                  OP_ADJUST_BRIGHTNESS);
    if (!cmd) {
        LOG_ERROR("Failed to create remote command payload");
        return -EINVAL;
    }

    if (remote_cmd_add_int(cmd, "brightness", value)) {
        delete_remote_cmd(cmd);
        return -EIO;
    }

    // NOTE: Command data will be released after the work completes
    return create_remote_task(WORK_PRIO_HIGH, cmd);
}

static void set_brightness_runtime_slider(int32_t value)
{
    if (lv_obj_is_valid(brightness_slider)) {
        lv_slider_set_value(brightness_slider, value, LV_ANIM_ON);
    }
    else
        LOG_TRACE("Brightness configuration page is not available");
}

static void set_brightness_slider_state(bool enable)
{
    lv_color_t color;

    if (!lv_obj_is_valid(brightness_slider))
        return;

    if (enable) {
        lv_obj_add_flag(brightness_slider, LV_OBJ_FLAG_CLICKABLE);
        color = lv_palette_main(LV_PALETTE_BLUE);
    } else {
        lv_obj_clear_flag(brightness_slider, LV_OBJ_FLAG_CLICKABLE);
        color = lv_palette_main(LV_PALETTE_GREY);
    }

    lv_obj_set_style_bg_color(brightness_slider, color, LV_PART_MAIN);
    lv_obj_set_style_bg_color(brightness_slider, color, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(brightness_slider, color, LV_PART_KNOB);
}

/*
 * Enable ALS page update.
 * UI-side handler that syncs ALS switch and brightness slider.
 */
static void enable_als_page_update(int8_t brightness)
{
    if (lv_obj_is_valid(als_switch))
        lv_obj_add_state(als_switch, LV_STATE_CHECKED);

    set_brightness_slider_state(false);
    set_brightness_runtime_slider(brightness);
}

/*
 * Disable ALS page update.
 * Re-enables manual brightness control and updates slider.
 */
static void disable_als_page_update(int8_t brightness)
{
    if (lv_obj_is_valid(als_switch))
        lv_obj_remove_state(als_switch, LV_STATE_CHECKED);

    set_brightness_slider_state(true);
    set_brightness_runtime_slider(brightness);
}

static void switch_auto_brightness_event_handler(lv_event_t *e)
{
    lv_event_code_t code;
    lv_obj_t *lobj;
    bool enable;
    int32_t ret = 0;

    code = lv_event_get_code(e);
    lobj = lv_event_get_target(e);
    if (code != LV_EVENT_VALUE_CHANGED)
        return;

    enable = lv_obj_has_state(lobj, LV_STATE_CHECKED);
    LOG_TRACE("Auto brightness: %s", enable ? "On" : "Off");

    ret = create_remote_simple_task(WORK_PRIO_NORMAL, \
                                    WORK_DURATION_SHORT, \
                                    enable ? OP_ENA_ALS : OP_DIS_ALS);
    if (ret) {
        LOG_ERROR("%s ambient light sensor failed, ret %d", \
                  enable ? "Enable" : "Disable", ret);
        return;
    }

    set_brightness_slider_state(!enable);
}

static void manual_brightness_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *lobj = lv_event_get_target(e);
    int32_t brightness_value, ret;

    brightness_value = (int32_t)lv_slider_get_value(lobj);

    ret = req_set_brightness(brightness_value);
    if (ret)
        LOG_ERROR("Set brightness failed, ret %d", ret);
}

static int32_t create_setting_items(lv_obj_t *par)
{
    lv_obj_t *group, *sym, *label, *switch_box;
    int32_t ret;

    const char *desc = "Manual and auto brightness setting";

    if (!par)
        return -EINVAL;

    /* Section: Brightness description */
    group = create_vertical_flex_group(par, "BRIGHTNESS-ABOUT");
    if (!group)
        return -ENOMEM;

    sym = create_symbol_box(group, NULL, &terminal_icons_48, \
                            ICON_CIRCLE_HALF_STROKE_SOLID);
    if (!sym)
        return -EIO;

    label = create_text_box(group, NULL, &lv_font_montserrat_24, desc);
    if (!label)
        return -EIO;

    /* Section: Auto brightness toggle */
    group = create_horizontal_flex_group(par, "BRIGHTNESS-AUTO");
    if (!group)
        return -ENOMEM;

    label = create_text_box(group, NULL, &lv_font_montserrat_24, "Auto");
    if (!label)
        return -EIO;

    switch_box = create_switch_box(group, NULL);
    if (!switch_box)
        return -EIO;

    als_switch = get_box_child(switch_box);
    lv_obj_add_event_cb(als_switch, switch_auto_brightness_event_handler, \
                        LV_EVENT_ALL, NULL);

    /* Section: Auto brightness toggle */
    group = create_horizontal_flex_group(par, "BRIGHTNESS-MANUAL");
    if (!group)
        return -ENOMEM;

    sym = create_symbol_box(group, NULL, &terminal_icons_32, \
                            ICON_CIRCLE_MINUS_SOLID);
    if (!sym)
        return -EIO;

    brightness_slider = create_slider(group, NULL);
    if (!brightness_slider)
        return -EIO;

    set_size(brightness_slider, LV_PCT(80), 20);
    lv_obj_set_style_anim_duration(brightness_slider, 500, 0);
    lv_obj_add_event_cb(brightness_slider, manual_brightness_event_handler, \
                        LV_EVENT_VALUE_CHANGED, NULL);


    sym = create_symbol_box(group, NULL, &terminal_icons_32, \
                            ICON_CIRCLE_PLUS_SOLID);
    if (!sym)
        return -EIO;

    /* Section: Spacer (flex filler) */
    ret = create_setting_filler(par);
    if (ret < 0)
        return ret;

    LOG_DEBUG("Brightness setting items created");
    return 0;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t *create_brightness_setting(lv_obj_t *par, const char *name)
{
    lv_obj_t *container, *view, *menu;
    view_ctn_t *v_ctx;
    char name_buf[64];
    int32_t ret;

    snprintf(name_buf, sizeof(name_buf), "%s_BRIGHTNESS", name);

    v_ctx = create_common_setting_view(par, name_buf, false, false);
    if (!v_ctx) {
        LOG_ERROR("[%s] create menu view failed, ret %d", name, ret);
        goto err_ctx;
    }

    container = get_view_container(v_ctx);
    view = get_view(v_ctx);
    menu = get_menu(v_ctx);

    ret = create_setting_items(menu);
    if (ret) {
        LOG_ERROR("Setting menu bar [%s] create failed, ret %d", \
                  get_name(menu), ret);
        goto err_view;
    }

    ret = req_backlight_state();
    if (ret)
        LOG_WARN("Unable to sync the latest configuration, ret %d", ret);

    if (container)
        return container;
    return view;

err_view:
    if (container)
        remove_obj_and_child(get_meta(container)->id, \
                             &get_meta(par)->child);
    else
        remove_obj_and_child(get_meta(view)->id, &get_meta(par)->child);
    free(v_ctx);
err_ctx:
    return NULL;
}

/*
 * Handle backlight state command.
 * Expected command entries:
 *   [0]: "als_enable"  -> 1 = enabled, 0 = disabled
 *   [1]: "brightness"  -> brightness level
 */
int32_t handle_backlight_state(remote_cmd_t *cmd)
{
    bool als_enabled;
    int32_t brightness;
    int32_t ret = 0;

    if (!cmd)
        return -EINVAL;

    als_enabled = !!cmd->entries[0].value.i32;
    brightness = cmd->entries[1].value.i32;

    LOG_TRACE("ALS status: key=[%s], value=[%d]", \
              cmd->entries[0].key, als_enabled);
    LOG_TRACE("Brightness: key=[%s], value=[%d]", \
              cmd->entries[1].key, brightness);

    /* Async update to avoid blocking UI thread */
    if (als_enabled)
        lv_async_call((lv_async_cb_t)enable_als_page_update, \
                      (void *)(intptr_t)brightness);
    else
        lv_async_call((lv_async_cb_t)disable_als_page_update, \
                      (void *)(intptr_t)brightness);

    return ret;
}
