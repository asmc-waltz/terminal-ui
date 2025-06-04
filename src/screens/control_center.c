/**
 * @file control_center.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <lvgl.h>
#include <screens.h>
#include <fonts.h>
#include <style.h>
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

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void gf_create_control_center(lv_obj_t *par)
{
    lv_style_t *p_style = NULL;
    lv_obj_t *p_ctrl_center_bg = NULL;
    lv_obj_t *p_ctrl_center = NULL;

    p_style = gf_get_lv_style(STY_BG_TRANS);

    p_ctrl_center_bg = gf_create_frame(par, ID_CONTROL_CENTER, 1024, 600);
    lv_obj_add_style(p_ctrl_center_bg , p_style, 0);

    p_style = gf_get_lv_style(STY_CTRL_CENTER);
    p_ctrl_center = gf_create_frame(p_ctrl_center_bg, ID_COMM, 512, 250);
    lv_obj_add_style(p_ctrl_center , p_style, 0);

    lv_obj_align_to(p_ctrl_center, p_ctrl_center_bg, LV_ALIGN_TOP_RIGHT, 0, 50);

    p_style = gf_get_lv_style(STY_BG_ICON_79);
    // TODO: update real button
    lv_obj_t *button = gf_create_btn_bg(p_ctrl_center, 111, p_style, 0x000000);

    p_style = gf_get_lv_style(STY_SYM_48);
    lv_obj_t *setting_symbol = gf_create_symbol(button, p_style, ICON_TOOLBOX_SOLID);

    gf_hide_control_center();
}

void gf_hide_control_center(void)
{
    lv_obj_t *pl_obj = NULL;
    g_obj *pg_obj = NULL;

    pl_obj = gf_get_obj(ID_CONTROL_CENTER, NULL);
    LV_ASSERT_NULL(pl_obj);
    pg_obj = pl_obj->user_data;

    lv_obj_add_flag(pl_obj, LV_OBJ_FLAG_HIDDEN);
    pg_obj->visible = false;
}

void gf_show_control_center(void)
{
    lv_obj_t *pl_obj = NULL;
    g_obj *pg_obj = NULL;

    pl_obj = gf_get_obj(ID_CONTROL_CENTER, NULL);
    LV_ASSERT_NULL(pl_obj);
    pg_obj = pl_obj->user_data;

    lv_obj_remove_flag(pl_obj, LV_OBJ_FLAG_HIDDEN);
    pg_obj->visible = true;
}

void gf_delete_control_center(void)
{
    gf_remove_obj_and_child(ID_CONTROL_CENTER, NULL);
}
