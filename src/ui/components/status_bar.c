/**
 * @file status_bar.c
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
#include <ui/ui_plat.h>
#include <ui/fonts.h>
#include <ui/style.h>
#include <ui/screens.h>

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
static lv_obj_t * sf_create_status_icon(lv_obj_t *par, const char *symbol, \
                                        uint32_t id)
{
    lv_obj_t *lbl = lv_label_create(par);

    gf_register_obj(par, lbl, id);
    // lv_obj_set_style_text_font(lbl, &terminal_icons_20, 0);
    lv_label_set_text_fmt(lbl, "%s", symbol);

    return lbl;
}

static void gesture_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_indev_t * indev_act = lv_indev_active();
    lv_dir_t dir = lv_indev_get_gesture_dir(indev_act);
    static uint32_t start_x = 0;
    static uint32_t start_y = 0;

    lv_point_t point;
    lv_indev_get_point(lv_indev_active(), &point);

    LOG_TRACE("Status bar event code %d", code);
    if (code == LV_EVENT_PRESSED) {
        // Store the starting point only if it appears to be a gesture
        if (point.y < 20) {
            start_x = point.x;
            start_y = point.y;
        }
    }

    if (code == LV_EVENT_PRESSING) {
        if (point.x >= 0 && dir == LV_DIR_BOTTOM && point.x <= 340) {
            LOG_TRACE("Swipe down from the top-left corner of the screen");
            if (start_y < 20 && point.y < 300 && dir == LV_DIR_BOTTOM) {
                gf_show_system_status();
                gf_show_home_indicator();
                start_x = 0;
                start_y = 0;
            }
        }

        if (point.x > 340 && dir == LV_DIR_BOTTOM && point.x <= 680) {
            LOG_TRACE("Swipe down from the top-mid of the screen");
            if (start_y < 20 && point.y < 300 && dir == LV_DIR_BOTTOM) {
                start_x = 0;
                start_y = 0;
            }
        }

        if (point.x > 680 && dir == LV_DIR_BOTTOM && point.x <= 1024) {
            LOG_TRACE("Swipe down from the top-right corner of the screen");
            if (start_y < 20 && point.y < 600 && dir == LV_DIR_BOTTOM) {
                gf_show_control_center(point.y);
                gf_show_home_indicator();
                start_x = 0;
                start_y = 0;
            }
        }
    }

    if (code == LV_EVENT_RELEASED) {
        start_x = 0;
        start_y = 0;
    }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t * gf_create_status_bar(lv_obj_t *par) {
    lv_obj_t *status_bar = gf_create_obj(par, ID_STATUS_BAR);
    lv_style_t *p_style = gf_get_lv_style(STY_STATUS_BAR);
    lv_obj_t *ctr = NULL;
    lv_obj_t *icon = NULL;

    lv_obj_add_style(status_bar, p_style, 0);
    lv_obj_set_size(status_bar, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_remove_flag(status_bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_flex_main_place(status_bar, LV_FLEX_ALIGN_SPACE_BETWEEN, 0);


    ctr = gf_create_obj(status_bar, ID_STATUS_BAR_LEFT_CTR);
    lv_obj_add_style(ctr, p_style, 0);
    lv_obj_set_style_pad_column(ctr, 15, 0);
    lv_obj_set_size(ctr, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    icon = sf_create_status_icon(ctr, ICON_SIGNAL_SOLID, ID_STATUS_BAR_SIGNAL_STRENGTH);
    icon = sf_create_status_icon(ctr, ICON_CIRCLE_INFO_SOLID, ID_STATUS_BAR_SIGNAL_TYPE);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_22, 0);
    lv_label_set_text(icon, "4G");
    icon = sf_create_status_icon(ctr, ICON_ETHERNET_SOLID, ID_STATUS_BAR_ETHERNET);
    icon = sf_create_status_icon(ctr, ICON_WIFI_SOLID, ID_STATUS_BAR_WIFI);


    ctr = gf_create_obj(status_bar, ID_STATUS_BAR_MID_CTR);
    lv_obj_add_style(ctr, p_style, 0);
    lv_obj_set_style_pad_column(ctr, 15, 0);

    icon = sf_create_status_icon(ctr, ICON_CLOCK_SOLID, ID_STATUS_BAR_CLOCK);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_22, 0);
    lv_label_set_text(icon, "09:09");


    ctr = gf_create_obj(status_bar, ID_STATUS_BAR_RIGHT_CTR);
    lv_obj_add_style(ctr, p_style, 0);
    lv_obj_set_style_pad_column(ctr, 16, 0);

    icon = sf_create_status_icon(ctr, ICON_BELL_SLASH_SOLID, ID_STATUS_BAR_ALERT);
    icon = sf_create_status_icon(ctr, ICON_PLUG_CIRCLE_BOLT_SOLID, ID_STATUS_BAR_POWER);

    lv_obj_add_flag(status_bar, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_event_cb(status_bar, gesture_event_handler, LV_EVENT_PRESSED , NULL);
    lv_obj_add_event_cb(status_bar, gesture_event_handler, LV_EVENT_PRESSING , NULL);
    lv_obj_add_event_cb(status_bar, gesture_event_handler, LV_EVENT_RELEASED, NULL);

    return status_bar;
}

void gf_hide_status_bar(void)
{
    lv_obj_t *pl_obj = NULL;
    g_obj_t *pg_obj = NULL;

    pl_obj = gf_get_obj(ID_STATUS_BAR, NULL);
    LV_ASSERT_NULL(pl_obj);
    pg_obj = pl_obj->user_data;

    lv_obj_add_flag(pl_obj, LV_OBJ_FLAG_HIDDEN);
    pg_obj->visible = false;
}

void gf_show_status_bar(void)
{
    lv_obj_t *pl_obj = NULL;
    g_obj_t *pg_obj = NULL;

    pl_obj = gf_get_obj(ID_STATUS_BAR, NULL);
    LV_ASSERT_NULL(pl_obj);
    pg_obj = pl_obj->user_data;

    lv_obj_remove_flag(pl_obj, LV_OBJ_FLAG_HIDDEN);
    pg_obj->visible = true;
}

void gf_delete_status_bar(void)
{
    gf_remove_obj_and_child(ID_STATUS_BAR, NULL);
}
