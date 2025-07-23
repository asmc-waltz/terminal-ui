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

    // p_ctrl_center_bg = gf_create_frame(lv_layer_top(), ID_CONTROL_CENTER, 1024, 600);
    // lv_obj_add_style(p_ctrl_center_bg , p_style, 0);

    // p_style = gf_get_lv_style(STY_CTRL_CENTER);
    p_ctrl_center = gf_create_frame(lv_layer_top(), ID_CONTROL_CENTER, 340, 500);
    lv_obj_add_style(p_ctrl_center , p_style, 0);

    lv_obj_align_to(p_ctrl_center, lv_layer_top(), LV_ALIGN_TOP_RIGHT, 0, -500);

    p_style = gf_get_lv_style(STY_BG_ICON_79);
    // TODO: update real button
    lv_obj_t *button = gf_create_btn_bg(p_ctrl_center, 111, p_style, 0x000000);

    p_style = gf_get_lv_style(STY_SYM_48);
    lv_obj_t *setting_symbol = gf_create_symbol(button, p_style, ICON_TOOLBOX_SOLID);
}

void gf_hide_control_center(void)
{
    lv_obj_t *pl_obj = NULL;
    g_obj *pg_obj = NULL;

    pl_obj = gf_get_obj(ID_CONTROL_CENTER, NULL);


    lv_obj_align_to(pl_obj, lv_layer_top(), LV_ALIGN_TOP_RIGHT, 0, -500);

    LV_ASSERT_NULL(pl_obj);
    pg_obj = pl_obj->user_data;

    lv_obj_add_flag(pl_obj, LV_OBJ_FLAG_HIDDEN);
    pg_obj->visible = false;
}


void scroll_down_anim(lv_obj_t *obj, lv_coord_t distance, uint32_t duration_ms)
{
	lv_anim_t a;
    static lv_coord_t previous_y = 0;

	lv_coord_t start_y = 0;
	lv_coord_t end_y = 0;



	lv_anim_init(&a);
	lv_anim_set_var(&a, obj);
	lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
	lv_anim_set_time(&a, duration_ms);

    lv_coord_t obj_height = lv_obj_get_height(obj);
    // lv_coord_t obj_width = lv_obj_get_width(obj);

    if (previous_y == 0) {
        start_y = -obj_height;
        end_y = -obj_height + distance;
        previous_y = end_y;
    } else if (distance <= obj_height) {
        start_y = previous_y;
        // end_y = distance;
        end_y = -obj_height + distance;
        previous_y = end_y;
    } else {
        start_y = previous_y;
        end_y = 0;
        previous_y = -1;
    }





	// lv_coord_t start_y = lv_obj_get_y(obj);
	// lv_coord_t end_y = start_y + distance;

	// lv_anim_set_values(&a, lv_obj_get_y(obj), lv_obj_get_y(obj) - distance);
    // lv_anim_set_values(&a, start_y, end_y);
    // lv_anim_set_values(&a, -obj_height, -obj_height + distance);
    if (previous_y == 0) {
        lv_anim_set_values(&a, -obj_height, -obj_height + distance);
    } else {
        lv_anim_set_values(&a, start_y, end_y);
    }
	lv_anim_start(&a);
}

void gf_show_control_center(lv_coord_t cur_y)
{
    lv_obj_t *pl_obj = NULL;
    g_obj *pg_obj = NULL;

    pl_obj = gf_get_obj(ID_CONTROL_CENTER, NULL);
    LV_ASSERT_NULL(pl_obj);
    pg_obj = pl_obj->user_data;
    
    lv_obj_remove_flag(pl_obj, LV_OBJ_FLAG_HIDDEN);
    pg_obj->visible = true;

    scroll_down_anim(pl_obj, cur_y, 50);

}

void gf_delete_control_center(void)
{
    gf_remove_obj_and_child(ID_CONTROL_CENTER, NULL);
}
