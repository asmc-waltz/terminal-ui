/**
 * @file status_bar.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <gmisc.h>

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
static lv_obj_t * sf_create_status_icon(lv_obj_t *par, uint32_t symbol, uint32_t id)
{
    lv_obj_t *lbl = lv_label_create(par);

    gf_register_obj(par, lbl, id);
    lv_obj_set_style_text_font(lbl, &terminal_icons_20, 0);
    // lv_label_set_long_mode(lbl, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text_fmt(lbl, "%s", symbol);

    return lbl;
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
    lv_obj_set_size(status_bar, 1024, 50);
    lv_obj_remove_flag(status_bar, LV_OBJ_FLAG_SCROLLABLE);


    ctr = gf_create_obj(status_bar, ID_STATUS_BAR_LEFT_CTR);
    lv_obj_align_to(ctr, status_bar, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_layout(ctr, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(ctr, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ctr, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    icon = sf_create_status_icon(ctr, ICON_SIGNAL_SOLID, ID_STATUS_BAR_SIGNAL_STRENGTH);
    lv_obj_align_to(icon, ctr, LV_ALIGN_LEFT_MID, 25, 0);
    lv_obj_set_style_text_font(icon, &terminal_icons_20, 0);

    icon = sf_create_status_icon(ctr, ICON_CIRCLE_INFO_SOLID, ID_STATUS_BAR_SIGNAL_TYPE);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_22, 0);
    lv_label_set_text(icon, "4G");

    icon = sf_create_status_icon(ctr, ICON_ETHERNET_SOLID, ID_STATUS_BAR_ETHERNET);
    icon = sf_create_status_icon(ctr, ICON_WIFI_SOLID, ID_STATUS_BAR_WIFI);


    ctr = gf_create_obj(status_bar, ID_STATUS_BAR_MID_CTR);
    lv_obj_align_to(ctr, status_bar, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_layout(ctr, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(ctr, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ctr, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    icon = sf_create_status_icon(ctr, ICON_CLOCK_SOLID, ID_STATUS_BAR_CLOCK);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_22, 0);
    lv_label_set_text(icon, "09:09");


    ctr = gf_create_obj(status_bar, ID_STATUS_BAR_RIGHT_CTR);
    lv_obj_align_to(ctr, status_bar, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_layout(ctr, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(ctr, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ctr, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    icon = sf_create_status_icon(ctr, ICON_BELL_SLASH_SOLID, ID_STATUS_BAR_ALERT);
    icon = sf_create_status_icon(ctr, ICON_PLUG_CIRCLE_BOLT_SOLID, ID_STATUS_BAR_POWER);

    return status_bar;
}
