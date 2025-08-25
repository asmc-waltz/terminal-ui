/**
 * @file backlight.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
// #define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL=" TOSTRING(LOG_LEVEL) ", will take precedence in this file."
#endif
#include <log.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include <lvgl.h>
#include <gmisc.h>
#include <fonts.h>
#include <style.h>
#include <dbus_comm.h>
#include <cmd_payload.h>

#include <task.h>

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
static int32_t create_set_brightness_cmd(int32_t value)
{
    remote_cmd_t *cmd;
    int32_t ret = 0;

    cmd = create_remote_cmd();
    if (!cmd) {
		return -ENOMEM;
    }

    remote_cmd_init(cmd, "terminal-ui", 1001, OP_SET_BRIGHTNESS);

    /* Add parameters */
    if (remote_cmd_add_string(cmd, "backlight", "max") || \
        remote_cmd_add_int(cmd, "brightness", value)) {
        ret = -EINVAL;
        goto out;
    }

    // NOTE: Command data will be released after the work completes
    ret = create_remote_task(BLOCK, cmd);
    return ret;

out:
    delete_remote_cmd(cmd);
    return ret;
}

static void sf_backlight_slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    int32_t brightness_percent = (int)lv_slider_get_value(slider);
    int32_t ret;

    // TODO: workaround when backlight cannot be set to zero from kernel
    if (brightness_percent <= 0) {
        brightness_percent = 1;
    } else if (brightness_percent >= 100) {
        brightness_percent = 100;
    }

    ret = create_set_brightness_cmd(brightness_percent);
    if (ret) {
        LOG_ERROR("Set brightness failed: ret %d", ret);
    }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void gf_create_backlight_control_slider(lv_obj_t *par)
{
    lv_obj_t * slider = lv_slider_create(par);

    gf_register_obj(par, slider, ID_SETTING_BRIGHTNESS_BAR);

    lv_obj_set_size(slider, LV_PCT(80), LV_PCT(5));
    lv_obj_align_to(slider, par, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(slider, sf_backlight_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_style_anim_duration(slider, 2000, 0);
}

void gf_create_backlight_setting_container(lv_obj_t *par)
{
    lv_obj_t *ctr = gf_create_obj(par, ID_SETTING_BRIGHTNESS_CTR);
    lv_style_t *p_style = gf_get_lv_style(STY_SETTING_SUB_CHILD_CTR);

    lv_obj_add_style(ctr, p_style, 0);
    gf_create_backlight_control_slider(ctr);
}

void gf_hide_brighness_setting()
{

    lv_obj_t *pl_obj = NULL;
    g_obj *pg_obj = NULL;

    pl_obj = gf_get_obj(ID_SETTING_BRIGHTNESS_CTR, NULL);

    LV_ASSERT_NULL(pl_obj);
    pg_obj = pl_obj->user_data;

    lv_obj_add_flag(pl_obj, LV_OBJ_FLAG_HIDDEN);
    pg_obj->visible = false;
}

void gf_show_brighness_setting()
{

    lv_obj_t *pl_obj = NULL;
    g_obj *pg_obj = NULL;

    pl_obj = gf_get_obj(ID_SETTING_BRIGHTNESS_CTR, NULL);

    LV_ASSERT_NULL(pl_obj);
    pg_obj = pl_obj->user_data;

    lv_obj_remove_flag(pl_obj, LV_OBJ_FLAG_HIDDEN);
    pg_obj->visible = true;
}
