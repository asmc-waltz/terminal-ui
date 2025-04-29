/**
 * @file home_indicator.c
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
static lv_obj_t *sp_home_indicator;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void home_indicator_handler(lv_event_t *event)
{
    lv_obj_t *home_indicator = lv_event_get_target(event);  // Get the button object
    // lv_obj_t *par = lv_obj_get_parent(btn);
    id_data *p_data = home_indicator->user_data;
    LV_LOG_USER("Home bar was clicked!");
    gf_refresh_all_layer();

    if (p_data->visible)
        gf_hide_home_indicator();

    gf_show_taskbar();

}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t * gf_create_home_indicator(lv_obj_t *parent)
{
    LV_ASSERT_NULL(parent);
    sp_home_indicator = lv_btn_create(parent);
    id_data *id_home_indicator_data = gf_init_user_data(sp_home_indicator);
    LV_ASSERT_NULL(id_home_indicator_data);
    lv_obj_set_style_bg_color(sp_home_indicator, lv_color_hex(0x000000), 0);
    lv_obj_set_style_radius(sp_home_indicator, 5, 0);
    lv_obj_set_size(sp_home_indicator, 315, 10);
    lv_obj_clear_flag(sp_home_indicator, LV_OBJ_FLAG_SCROLLABLE);

    // Align it to bottom-middle AFTER children are added
    lv_obj_align_to(sp_home_indicator, parent, LV_ALIGN_BOTTOM_MID, 0, -10);

    lv_obj_add_event_cb(sp_home_indicator, home_indicator_handler, LV_EVENT_CLICKED, NULL);
    gf_hide_home_indicator();

    return sp_home_indicator;
}

void gf_hide_home_indicator(void)
{
    id_data *home_indicator_data = sp_home_indicator->user_data;

    lv_obj_add_flag(sp_home_indicator, LV_OBJ_FLAG_HIDDEN);
    home_indicator_data->visible = false;
}

void gf_show_home_indicator(void)
{
    id_data *home_indicator_data = sp_home_indicator->user_data;

    lv_obj_remove_flag(sp_home_indicator, LV_OBJ_FLAG_HIDDEN);
    home_indicator_data->visible = true;
}

void gf_delete_home_indicator(void)
{
    if(lv_obj_is_valid(sp_home_indicator)) {
        gf_free_user_data(sp_home_indicator);
        lv_obj_delete(sp_home_indicator);
    }
}
