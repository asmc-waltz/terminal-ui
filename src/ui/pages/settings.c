/**
 * @file setting.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL defined locally will override the global setting in this file"
#endif
#include <log.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <lvgl.h>
#include <list.h>
#include <ui/ui_core.h>
#include <ui/fonts.h>
#include <ui/comps.h>

#define TEST 1
#if defined(TEST)
#include <time.h>
#endif
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

lv_obj_t *ex_scr = NULL;
lv_obj_t *ex_setting_box = NULL;
lv_obj_t *ex_top_bar = NULL;
lv_obj_t *ex_window = NULL;
lv_obj_t *ex_mid_box = NULL;
lv_obj_t *ex_corner_box = NULL;
lv_obj_t *ex_text_box2 = NULL;
lv_obj_t *ex_text_2 = NULL;
lv_obj_t *ex_comp_cont = NULL;
lv_obj_t *ex_text_box1 = NULL;
lv_obj_t *ex_text1 = NULL;
lv_obj_t *ex_sw_box = NULL;
lv_obj_t *ex_sw1 = NULL;
lv_obj_t *ex_sym_box1 = NULL;
lv_obj_t *ex_sym1 = NULL;
lv_obj_t *ex_sym_box2 = NULL;
lv_obj_t *ex_sym2 = NULL;
lv_obj_t *ex_btn1 = NULL;
lv_obj_t *ex_slider1 = NULL;

void sample_rot(int32_t angle)
{
    g_set_scr_rot_dir(angle);

    gf_rotate_obj_tree(ex_setting_box->user_data);
    gf_rotate_obj_tree(ex_top_bar->user_data);
    gf_rotate_obj_tree(ex_window->user_data);
    gf_rotate_obj_tree(ex_mid_box->user_data);
    gf_rotate_obj_tree(ex_corner_box->user_data);

    int32_t rot_dir = g_get_scr_rot_dir();

    lv_obj_t *keyboard;
    if (rot_dir == ROTATION_0 || rot_dir == ROTATION_180) {
        keyboard = gf_get_obj_by_name("comps.keyboard_v", \
                            &((g_obj *)(ex_scr->user_data))->child);
        if (keyboard)
            lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

        keyboard = gf_get_obj_by_name("comps.keyboard_h", \
                            &((g_obj *)(ex_scr->user_data))->child);
        if (keyboard)
            lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
        else
            keyboard = gf_create_keyboard(ex_scr, "comps.keyboard_h", 1004, 250);
    } else if (rot_dir == ROTATION_90 || rot_dir == ROTATION_270) {
        keyboard = gf_get_obj_by_name("comps.keyboard_h", \
                            &((g_obj *)(ex_scr->user_data))->child);
        if (keyboard)
            lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

        keyboard = gf_get_obj_by_name("comps.keyboard_v", \
                            &((g_obj *)(ex_scr->user_data))->child);
        if (keyboard)
            lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
        else
            keyboard = gf_create_keyboard(ex_scr, "comps.keyboard_v", 580, 300);
    }
    gf_rotate_obj_tree(keyboard->user_data);

    int32_t w, h;
    lv_obj_update_layout(ex_comp_cont);
    w = lv_obj_get_width(ex_comp_cont);
    h = lv_obj_get_height(ex_comp_cont);

    // Adjust screen scroll accordingly.
    if (rot_dir == ROTATION_0) {
        lv_obj_scroll_to(ex_window, 0, 0, LV_ANIM_OFF);
    } else if (rot_dir == ROTATION_90) {
        lv_obj_scroll_to(ex_window, w, 0, LV_ANIM_OFF);
    } else if (rot_dir == ROTATION_180) {
        lv_obj_scroll_to(ex_window, 0, h, LV_ANIM_OFF);
    } else if (rot_dir == ROTATION_270) {
        lv_obj_scroll_to(ex_window, 0, 0, LV_ANIM_OFF);
    }
}

int32_t get_random_0_3(void)
{
    int32_t val;

    val = rand() % 4;
    return val;
}
static void btn_handler(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);  // Get the button object
    lv_obj_t *par = lv_obj_get_parent(btn);
    g_obj *gobj = NULL;

    gobj = btn->user_data;
    LOG_DEBUG("ID %d: Test button clicked", gobj->id);
    sample_rot(get_random_0_3());

}

void create_dynamic_ui()
{
    // Main box as screen background
    ex_scr = gf_create_box(lv_screen_active(), NULL);
    gf_gobj_set_size(ex_scr, 1024, 600);
    gf_gobj_set_pos(ex_scr, 0, 0);
    lv_obj_set_style_bg_color(ex_scr, lv_color_hex(0x000000), 0);

    ex_setting_box = gf_create_box(ex_scr, NULL);
    gf_gobj_set_size(ex_setting_box, 1024, 330);
    gf_obj_scale_enable_w(ex_setting_box);
    gf_obj_scale_set_pad_w(ex_setting_box, 10);
    gf_obj_scale_enable_w(ex_setting_box);
    gf_obj_scale_set_pad_w(ex_setting_box, 10);
    gf_gobj_align_to(ex_setting_box, ex_scr, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_bg_color(ex_setting_box, lv_color_hex(0xDDDDDD), 0);
    lv_obj_clear_flag(ex_setting_box, LV_OBJ_FLAG_SCROLLABLE);
    //--------------------------------------------------------------------------
    ex_top_bar = gf_create_box(ex_scr, NULL);
    gf_gobj_set_size(ex_top_bar, 1014, 40);
    lv_obj_set_style_bg_color(ex_top_bar, lv_color_hex(0xFF9913), 0);
    gf_obj_scale_enable_w(ex_top_bar);
    gf_obj_scale_set_pad_w(ex_top_bar, 10);
    gf_gobj_align_to(ex_top_bar, ex_scr, LV_ALIGN_TOP_MID, 0, 5);
    //--------------------------------------------------------------------------
    ex_sym_box2 = gf_create_box(ex_top_bar, NULL);
    gf_gobj_set_size(ex_sym_box2, 40, 40);
    // lv_obj_set_style_bg_color(ex_sym_box2, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(ex_sym_box2, LV_OPA_0, 0);
    lv_obj_clear_flag(ex_sym_box2, LV_OBJ_FLAG_SCROLLABLE);
    gf_gobj_align_to(ex_sym_box2, ex_sw_box, LV_ALIGN_RIGHT_MID, -20, 0);
    //--------------------------------------------------------------------------
    ex_sym2 = gf_create_sym(ex_sym_box2, NULL, 5, 5, &terminal_icons_20, \
                            ICON_SIGNAL_SOLID, lv_color_hex(0x000000));
    //--------------------------------------------------------------------------
    ex_mid_box = gf_create_box(ex_scr, NULL);
    gf_gobj_set_size(ex_mid_box, 80, 90);
    lv_obj_set_style_bg_color(ex_mid_box, lv_color_hex(0x0000FF), 0);
    gf_gobj_align_to(ex_mid_box, ex_scr, LV_ALIGN_BOTTOM_MID, 0, 0);
    //--------------------------------------------------------------------------
    ex_corner_box = gf_create_box(ex_scr, NULL);
    gf_gobj_set_size(ex_corner_box, 200, 200);
    lv_obj_set_style_bg_color(ex_corner_box, lv_color_hex(0x00FF00), 0);
    gf_gobj_align_to(ex_corner_box, ex_scr, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    //--------------------------------------------------------------------------
    ex_text_box2 = gf_create_box(ex_corner_box, "CORNOR TEXT BOX");
    gf_gobj_set_size(ex_text_box2, 150, 40);
    lv_obj_set_style_bg_color(ex_text_box2, lv_color_hex(0xFFFFFF), 0);
    lv_obj_clear_flag(ex_text_box2, LV_OBJ_FLAG_SCROLLABLE);
    gf_gobj_align_to(ex_text_box2, ex_corner_box, LV_ALIGN_CENTER, 10, 25);
    //--------------------------------------------------------------------------
    ex_text_2 = gf_create_text(ex_text_box2, "CORNOR TEXT", 10, 10, "-CORNER-");
    //--------------------------------------------------------------------------
    // Child box as a menu bar
    ex_window = gf_create_box(ex_setting_box, NULL);
    gf_gobj_set_size(ex_window, 400, 325);
    lv_obj_set_style_bg_color(ex_window, lv_color_hex(0xFFFFFF), 0);
    gf_obj_scale_enable_h(ex_window);
    gf_obj_scale_set_pad_h(ex_window, 10);
    // gf_obj_scale_enable_w(ex_window);
    // gf_obj_scale_set_pad_w(ex_window, 20);
    gf_gobj_align_to(ex_window, ex_setting_box, LV_ALIGN_TOP_LEFT, 5, 50);

    //--------------------------------------------------------------------------
    // Container for all sub components
    ex_comp_cont = gf_create_container(ex_window, NULL);
    gf_gobj_set_size(ex_comp_cont, 370, 1200);
    gf_gobj_set_pos(ex_comp_cont, 10, 10);
    lv_obj_set_style_bg_color(ex_comp_cont, lv_color_hex(0xFFEE8C), 0);
    //--------------------------------------------------------------------------
    ex_text_box1 = gf_create_box(ex_comp_cont, NULL);
    gf_gobj_set_size(ex_text_box1, 150, 40);
    lv_obj_set_style_bg_color(ex_text_box1, lv_color_hex(0xFFFFFF), 0);
    lv_obj_clear_flag(ex_text_box1, LV_OBJ_FLAG_SCROLLABLE);
    gf_gobj_align_to(ex_text_box1, ex_comp_cont, LV_ALIGN_TOP_LEFT, 30, 25);
    //--------------------------------------------------------------------------
    ex_text1 = gf_create_text(ex_text_box1, NULL, 10, 10, "ASMC WALTZ");
    //--------------------------------------------------------------------------
    ex_sw_box = gf_create_box(ex_comp_cont, NULL);
    gf_gobj_set_size(ex_sw_box, 80, 50);
    lv_obj_set_style_bg_color(ex_sw_box, lv_color_hex(0xFFFFFF), 0);
    lv_obj_clear_flag(ex_sw_box, LV_OBJ_FLAG_SCROLLABLE);
    gf_gobj_align_to(ex_sw_box, ex_text_box1, LV_ALIGN_OUT_BOTTOM_MID, 15, 25);
    //--------------------------------------------------------------------------
    ex_sw1 = gf_create_switch(ex_sw_box, NULL);
    gf_gobj_set_size(ex_sw1, 60, 30);
    gf_gobj_set_pos(ex_sw1, 10, 10);
    //--------------------------------------------------------------------------
    ex_sym_box1 = gf_create_box(ex_comp_cont, NULL);
    gf_gobj_set_size(ex_sym_box1, 50, 50);
    lv_obj_set_style_bg_color(ex_sym_box1, lv_color_hex(0xFFFFFF), 0);
    lv_obj_clear_flag(ex_sym_box1, LV_OBJ_FLAG_SCROLLABLE);
    gf_gobj_align_to(ex_sym_box1, ex_sw_box, LV_ALIGN_OUT_BOTTOM_MID, 30, 25);
    //--------------------------------------------------------------------------
    ex_sym1 = gf_create_sym(ex_sym_box1, NULL, 10, 10, &terminal_icons_32, \
                            ICON_TOOLBOX_SOLID, lv_color_hex(0x000000));
    //--------------------------------------------------------------------------
    ex_btn1 = gf_create_btn(ex_comp_cont, NULL);
    gf_gobj_set_size(ex_btn1, 80, 50);
    gf_gobj_align_to(ex_btn1, ex_sym_box1, LV_ALIGN_OUT_BOTTOM_RIGHT, 10, 25);
    lv_obj_add_event_cb(ex_btn1, btn_handler, LV_EVENT_CLICKED, ex_btn1->user_data);
    //--------------------------------------------------------------------------
    ex_slider1 = gf_create_slider(ex_comp_cont, "Meo(^^)");
    gf_gobj_set_size(ex_slider1, 100, 20);
    gf_gobj_align_to(ex_slider1, ex_btn1, LV_ALIGN_OUT_RIGHT_MID, 30, 0);
}
