/**
 * @file home_indicator.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
// #define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL defined locally will override the global setting in this file"
#endif
#include <log.h>

#include <stdint.h>

#include <lvgl.h>
#include <gmisc.h>
#include <style.h>
#include <screens.h>

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
static void gesture_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_indev_t * indev_act = lv_indev_active();
    lv_dir_t dir = lv_indev_get_gesture_dir(indev_act);

    static uint32_t start_x = 0;
    static uint32_t start_y = 0;

    // LV_LOG_USER("event code %d", code);
    lv_point_t point;
    lv_indev_get_point(lv_indev_active(), &point);

    if (code == LV_EVENT_PRESSED) {
        if (point.x > 355 && point.x < 670) {
            if (point.y > 580) {
                // LV_LOG_USER("look like BACK HOME", point.x);
                start_x = point.x;
                start_y = point.y;
                // LV_LOG_USER("start x %d", point.x);
                // LV_LOG_USER("start y %d", point.y);
            }
        }
    }

    if (code == LV_EVENT_PRESSING) {
        if (point.x > 355 && dir == LV_DIR_TOP && point.x < 670) {
            if (start_y > 580 && point.y < 550) {
                gf_show_taskbar();
            }
            if (start_y > 580 && point.y < 500 && dir == LV_DIR_TOP) {
                // LV_LOG_USER("pressing x %d -> back home", point.x);
                gf_hide_setting();
                gf_hide_home_indicator();
                gf_hide_control_center();
                gf_hide_system_status();
                start_x = 0;
                start_y = 0;
            }
        }
    }

    if (code == LV_EVENT_RELEASED) {
        // LV_LOG_USER("release x %d", point.x);
        // LV_LOG_USER("release y %d", point.y);
        start_x = 0;
        start_y = 0;
    }
}
/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t * gf_create_home_indicator(lv_obj_t *parent)
{
    LV_ASSERT_NULL(parent);
    lv_obj_t * p_home_indicator = NULL;
    lv_style_t *p_style = NULL;

    p_home_indicator = gf_create_obj(parent, ID_HOME_INDICATOR);
    LV_ASSERT_NULL(p_home_indicator);

    p_style = gf_get_lv_style(STY_HOME_INDICATOR);
    lv_obj_add_style(p_home_indicator, p_style, 0);
    lv_obj_align_to(p_home_indicator, parent, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_obj_add_event_cb(p_home_indicator, gesture_event_handler, LV_EVENT_PRESSED , NULL);
    lv_obj_add_event_cb(p_home_indicator, gesture_event_handler, LV_EVENT_PRESSING , NULL);
    lv_obj_add_event_cb(p_home_indicator, gesture_event_handler, LV_EVENT_RELEASED, NULL);

    gf_hide_home_indicator();

    return p_home_indicator;
}

void gf_hide_home_indicator(void)
{
    lv_obj_t *pl_obj = NULL;
    g_obj *pg_obj = NULL;

    pl_obj = gf_get_obj(ID_HOME_INDICATOR, NULL);
    LV_ASSERT_NULL(pl_obj);
    pg_obj = pl_obj->user_data;

    lv_obj_add_flag(pl_obj, LV_OBJ_FLAG_HIDDEN);
    pg_obj->visible = false;
}

void gf_show_home_indicator(void)
{
    lv_obj_t *pl_obj = NULL;
    g_obj *pg_obj = NULL;

    pl_obj = gf_get_obj(ID_HOME_INDICATOR, NULL);
    LV_ASSERT_NULL(pl_obj);
    pg_obj = pl_obj->user_data;

    lv_obj_remove_flag(pl_obj, LV_OBJ_FLAG_HIDDEN);
    pg_obj->visible = true;
}

void gf_delete_home_indicator(void)
{
    gf_remove_obj_and_child(ID_HOME_INDICATOR, NULL);
}
