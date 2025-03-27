#include <layer.h>

lv_obj_t *top_scr = NULL;
lv_obj_t *glob_kb = NULL;
bool kb_visible = false;

void top_layer_cfg() {
    lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_TRANSP, LV_PART_MAIN);
}

void top_keyboard_create() {
    glob_kb = lv_keyboard_create(lv_layer_top());
    lv_obj_set_size(glob_kb, LV_PCT(95), LV_PCT(50));
    lv_obj_align(glob_kb, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_keyboard_set_mode(glob_kb, LV_KEYBOARD_MODE_TEXT_LOWER);
    lv_keyboard_set_popovers(glob_kb, true);
    lv_obj_add_flag(glob_kb, LV_OBJ_FLAG_HIDDEN);
    top_hidden_keyboard();
}

void top_hidden_keyboard() {
    lv_obj_add_flag(glob_kb, LV_OBJ_FLAG_HIDDEN);
    kb_visible = false;
}

void top_show_keyboard() {
    lv_obj_remove_flag(glob_kb, LV_OBJ_FLAG_HIDDEN);
    kb_visible = true;
}
