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

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void home_indicator_handler(lv_event_t *e)
{
    g_obj *pg_obj = lv_event_get_user_data(e);

    LV_LOG_USER("Home bar was clicked!");
    gf_refresh_all_layer();

    if (pg_obj->visible) {
        gf_hide_home_indicator();
        gf_show_taskbar();
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
    lv_obj_align_to(p_home_indicator, parent, LV_ALIGN_BOTTOM_MID, 0, -10);

    gf_register_handler(ID_HOME_INDICATOR, home_indicator_handler, LV_EVENT_CLICKED);

    gf_hide_home_indicator();

    return p_home_indicator;
}

void gf_hide_home_indicator(void)
{
    lv_obj_t *pl_obj = NULL;
    g_obj *pg_obj = NULL;

    pl_obj = gf_get_obj(ID_HOME_INDICATOR);
    LV_ASSERT_NULL(pl_obj);
    pg_obj = pl_obj->user_data;

    lv_obj_add_flag(pl_obj, LV_OBJ_FLAG_HIDDEN);
    pg_obj->visible = false;
}

void gf_show_home_indicator(void)
{
    lv_obj_t *pl_obj = NULL;
    g_obj *pg_obj = NULL;

    pl_obj = gf_get_obj(ID_HOME_INDICATOR);
    LV_ASSERT_NULL(pl_obj);
    pg_obj = pl_obj->user_data;

    lv_obj_remove_flag(pl_obj, LV_OBJ_FLAG_HIDDEN);
    pg_obj->visible = true;
}

void gf_delete_home_indicator(void)
{
    gf_remove_obj(ID_HOME_INDICATOR);
}
