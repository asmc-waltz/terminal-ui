/**
 * @file backlight.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <lvgl.h>
#include <gmisc.h>
#include <fonts.h>
#include <style.h>
#include <log.h>

/*********************
 *      DEFINES
 *********************/
#define FS_BRIGHTNESS   "/sys/class/backlight/backlight/brightness"

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
static void sf_backlight_slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    char str_percent[10];
    int brightness_percent = (int)lv_slider_get_value(slider);

    // TODO: workaround when backlight cannot be set to zero from kernel
    if (brightness_percent == 0) {
        brightness_percent = 1;
    }

    sprintf(str_percent, "%d", brightness_percent);
    gf_fs_write_file(FS_BRIGHTNESS, str_percent, sizeof(str_percent));

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
