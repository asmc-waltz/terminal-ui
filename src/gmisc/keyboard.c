/**
 * @file keyboard.c
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
bool kb_visible = false;
lv_obj_t *glob_kb = NULL;
lv_obj_t *overlay= NULL;

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
 *   GLOBAL FUNCTIONS
 **********************/
void gf_keyboard_create()
{
    overlay = gf_create_frame(lv_layer_top(), ID_COMM, 1004, 500);
    lv_obj_set_style_bg_opa(overlay, LV_OPA_COVER, 0);
    lv_obj_align(overlay, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(overlay, lv_color_black(), 0);

    lv_obj_t * pwd_ta = lv_textarea_create(overlay);

    lv_obj_set_size(pwd_ta, LV_PCT(100), LV_PCT(30));
    // lv_textarea_set_one_line(user_name, true);
    lv_textarea_set_placeholder_text(pwd_ta, "Your name");
    // lv_obj_add_event_cb(user_name, ta_event_cb, LV_EVENT_ALL, kb);

    glob_kb = lv_keyboard_create(lv_layer_top());
    lv_obj_set_size(glob_kb, 1004, LV_PCT(50));
    lv_obj_align(glob_kb, LV_ALIGN_BOTTOM_MID, 0, -10);

    lv_obj_set_style_radius(glob_kb, 10, 0);
    lv_keyboard_set_mode(glob_kb, LV_KEYBOARD_MODE_TEXT_LOWER);
    lv_keyboard_set_popovers(glob_kb, true);
    lv_obj_add_flag(glob_kb, LV_OBJ_FLAG_HIDDEN);

    lv_keyboard_set_textarea(glob_kb, pwd_ta); /*Focus it on one of the text areas to start*/

    gf_hide_keyboard();
}

void gf_hide_keyboard()
{
    lv_obj_add_flag(glob_kb, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(overlay, LV_OBJ_FLAG_HIDDEN);
    kb_visible = false;
}

void gf_show_keyboard()
{
    lv_obj_remove_flag(glob_kb, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(overlay, LV_OBJ_FLAG_HIDDEN);
    kb_visible = true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

