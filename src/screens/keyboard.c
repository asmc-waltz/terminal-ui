/**
 * @file keyboard.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <screens.h>

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
void keyboard_create() {
    glob_kb = lv_keyboard_create(lv_layer_top());
    lv_obj_set_size(glob_kb, LV_PCT(95), LV_PCT(50));
    lv_obj_align(glob_kb, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_keyboard_set_mode(glob_kb, LV_KEYBOARD_MODE_TEXT_LOWER);
    lv_keyboard_set_popovers(glob_kb, true);
    lv_obj_add_flag(glob_kb, LV_OBJ_FLAG_HIDDEN);
    hidden_keyboard();
}

void hidden_keyboard() {
    lv_obj_add_flag(glob_kb, LV_OBJ_FLAG_HIDDEN);
    kb_visible = false;
}

void show_keyboard() {
    lv_obj_remove_flag(glob_kb, LV_OBJ_FLAG_HIDDEN);
    kb_visible = true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

