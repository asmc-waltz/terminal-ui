#include <layer.h>

lv_obj_t *act_scr = NULL;

void active_layer_cfg() {
    lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_TRANSP, LV_PART_MAIN);
}

void render_curve_bg_act_layer() {
    act_scr = lv_obj_create(lv_screen_active());
    lv_obj_set_size(act_scr , 1024, 600);
    lv_obj_set_style_border_width(act_scr , 0, 0);
    lv_obj_set_style_border_color(act_scr, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_side(act_scr, LV_BORDER_SIDE_FULL, 0);
    lv_obj_set_style_radius(act_scr , 20, 0);
    lv_obj_set_style_bg_opa(act_scr, LV_OPA_50, 0);
}
