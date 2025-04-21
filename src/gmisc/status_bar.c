/**
 * @file status_bar.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <gmisc.h>
#include <style.h>
#include <fonts.h>

/*********************
 *      DEFINES
 *********************/
#define STATUS_ICONS_Y_POS      -

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

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t * g_create_status_bar(lv_obj_t *parent) {
    lv_obj_t *status_bar = lv_obj_create(parent);
    lv_obj_set_size(status_bar, 1024, 50);
    lv_obj_add_style(status_bar, &sts_bar_style, 0);
    lv_obj_remove_flag(status_bar, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * signal_icon = lv_label_create(status_bar);
    lv_obj_align_to(signal_icon, status_bar, LV_ALIGN_LEFT_MID, 25, 0);
    lv_label_set_text(signal_icon, ICON_SIGNAL_SOLID);

    lv_obj_t *lte_icon= lv_label_create(status_bar);
    lv_obj_set_style_text_font(lte_icon, &lv_font_montserrat_14, 0);
    lv_obj_align_to(lte_icon, status_bar, LV_ALIGN_LEFT_MID, 75, 0);
    lv_label_set_text(lte_icon, "4G");

    lv_obj_t * ether_icon= lv_label_create(status_bar);
    lv_obj_align_to(ether_icon, status_bar, LV_ALIGN_LEFT_MID, 125, 0);
    lv_label_set_text(ether_icon, ICON_ETHERNET_SOLID);

    lv_obj_t * wifi_icon= lv_label_create(status_bar);
    lv_obj_align_to(wifi_icon, status_bar, LV_ALIGN_LEFT_MID, 175, 0);
    lv_label_set_text(wifi_icon, ICON_WIFI_SOLID);

    lv_obj_t *small_clock= lv_label_create(status_bar);
    lv_obj_set_style_text_font(small_clock, &lv_font_montserrat_14, 0);
    lv_obj_align_to(small_clock, status_bar, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(small_clock, "09:09");

    lv_obj_t *alert_icon= lv_label_create(status_bar);
    lv_obj_align_to(alert_icon, status_bar, LV_ALIGN_RIGHT_MID, -50, 0);
    lv_label_set_text(alert_icon, ICON_BELL_SLASH_SOLID);

    lv_obj_t *plug_icon= lv_label_create(status_bar);
    lv_obj_align_to(plug_icon, status_bar, LV_ALIGN_RIGHT_MID, 10, 0);
    lv_label_set_text(plug_icon, ICON_PLUG_CIRCLE_BOLT_SOLID);

    return status_bar;
}
