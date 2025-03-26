#include <layer.h>

lv_obj_t *top_scr = NULL;
lv_obj_t * keyboard = NULL;

void top_layer_cfg() {
    lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_TRANSP, LV_PART_MAIN);
}

void top_render_keyboard() {
    keyboard  = lv_keyboard_create(lv_layer_top());
    lv_obj_set_size(keyboard, LV_PCT(95), LV_PCT(50));
    lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);
    lv_keyboard_set_popovers(keyboard, true);
}

void top_delete_keyboard() {
    lv_obj_del(keyboard);
    keyboard = NULL;
}
