#include <layer.h>

void render_curve_bg_bot_layer() {
    lv_obj_t *bg_l1 = lv_obj_create(lv_layer_bottom());
    lv_obj_set_size(bg_l1 , 1024, 600);
    lv_obj_set_style_bg_color(bg_l1 , lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_border_width(bg_l1 , 0, 0);

    lv_obj_t *bg_l2 = lv_obj_create(lv_layer_bottom());
    lv_obj_set_size(bg_l2, 1024, 600);
    lv_obj_set_style_border_width(bg_l2, 0, 0);
    lv_obj_set_style_bg_color(bg_l2, lv_color_hex(0xFFA725), LV_PART_MAIN);
    lv_obj_set_style_radius(bg_l2, 20, 0);
}
