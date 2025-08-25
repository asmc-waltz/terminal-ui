/**
 * @file keyboard.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
// #define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL=" TOSTRING(LOG_LEVEL) ", will take precedence in this file."
#endif
#include <log.h>

#include <lvgl.h>
#include <gmisc.h>
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
bool kb_visible = false;
lv_obj_t *glob_kb = NULL;

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
    glob_kb = lv_keyboard_create(lv_layer_top());
    lv_obj_set_style_text_font(glob_kb, &lv_font_montserrat_22, 0);
    lv_obj_set_size(glob_kb, 1014, LV_PCT(50));
    lv_obj_align(glob_kb, LV_ALIGN_BOTTOM_MID, 0, -5);

    lv_obj_set_style_bg_opa(glob_kb, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(glob_kb, lv_color_hex(0xCBD1D9), 0);

    lv_obj_set_style_radius(glob_kb, 16, 0);
    lv_keyboard_set_mode(glob_kb, LV_KEYBOARD_MODE_TEXT_LOWER);
    lv_keyboard_set_popovers(glob_kb, true);
    lv_obj_add_flag(glob_kb, LV_OBJ_FLAG_HIDDEN);

    gf_hide_keyboard();
}

void gf_hide_keyboard()
{
    lv_obj_add_flag(glob_kb, LV_OBJ_FLAG_HIDDEN);
    kb_visible = false;
}

void gf_show_keyboard()
{
    lv_obj_remove_flag(glob_kb, LV_OBJ_FLAG_HIDDEN);
    kb_visible = true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

