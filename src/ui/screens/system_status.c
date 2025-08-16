/**
 * @file system_status.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <lvgl.h>
#include <screens.h>
#include <gmisc.h>
#include <style.h>

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
void gf_create_system_status(lv_obj_t *par)
{
    lv_style_t *p_style = NULL;
    lv_obj_t *p_system_status_bg = NULL;
    lv_obj_t *p_system_status = NULL;

    p_style = gf_get_lv_style(STY_BG_TRANS);

    p_system_status_bg = gf_create_frame(par, ID_SYSTEM_STATUS, 1024, 600);
    lv_obj_add_style(p_system_status_bg , p_style, 0);

    p_style = gf_get_lv_style(STY_CTRL_CENTER);
    p_system_status = gf_create_frame(p_system_status_bg, ID_COMM, 512, 250);
    lv_obj_add_style(p_system_status , p_style, 0);

    lv_obj_align_to(p_system_status, p_system_status_bg, LV_ALIGN_TOP_LEFT, 0, 50);

    gf_hide_system_status();
}

void gf_hide_system_status(void)
{
    lv_obj_t *pl_obj = NULL;
    g_obj *pg_obj = NULL;

    pl_obj = gf_get_obj(ID_SYSTEM_STATUS, NULL);
    LV_ASSERT_NULL(pl_obj);
    pg_obj = pl_obj->user_data;

    lv_obj_add_flag(pl_obj, LV_OBJ_FLAG_HIDDEN);
    pg_obj->visible = false;
}

void gf_show_system_status(void)
{
    lv_obj_t *pl_obj = NULL;
    g_obj *pg_obj = NULL;

    pl_obj = gf_get_obj(ID_SYSTEM_STATUS, NULL);
    LV_ASSERT_NULL(pl_obj);
    pg_obj = pl_obj->user_data;

    lv_obj_remove_flag(pl_obj, LV_OBJ_FLAG_HIDDEN);
    pg_obj->visible = true;
}

void gf_delete_system_status(void)
{
    gf_remove_obj_and_child(ID_SYSTEM_STATUS, NULL);
}
