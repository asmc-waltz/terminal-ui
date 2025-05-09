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
    // lv_obj_set_style_text_font(lbl, &terminal_icons_20, 0);
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

    return status_bar;
}

void gf_hide_status_bar(void)
{
    lv_obj_t *pl_obj = NULL;
    g_obj *pg_obj = NULL;

    pl_obj = gf_get_obj(ID_STATUS_BAR, NULL);
    LV_ASSERT_NULL(pl_obj);
    pg_obj = pl_obj->user_data;

    lv_obj_add_flag(pl_obj, LV_OBJ_FLAG_HIDDEN);
    pg_obj->visible = false;
}

void gf_show_status_bar(void)
{
    lv_obj_t *pl_obj = NULL;
    g_obj *pg_obj = NULL;

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
