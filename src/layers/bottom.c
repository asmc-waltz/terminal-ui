#include <layers.h>

void render_curve_bg_bot_layer() {
    lv_obj_t *bg_01 = lv_obj_create(lv_layer_bottom());
    lv_obj_set_size(bg_01, 1024, 600);
    lv_obj_set_style_border_width(bg_01, 0, 0);
    lv_obj_set_style_bg_color(bg_01, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_set_style_radius(bg_01, 20, 0);
    lv_obj_set_style_clip_corner(bg_01, true, 0);

    lv_obj_set_style_shadow_color(bg_01, lv_color_hex(0x000000), 0);
    lv_obj_set_style_shadow_width(bg_01, 50, 0);
    lv_obj_set_style_shadow_spread(bg_01, 10, 0);
    lv_obj_set_style_outline_color(bg_01, lv_color_hex(0x000000), 0);
    lv_obj_set_style_outline_width(bg_01, 2, 0);
    lv_obj_set_style_outline_pad(bg_01, 5, 0);
}
