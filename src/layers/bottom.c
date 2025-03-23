#include <layer.h>

void bot_render_basement_bg() {
    lv_obj_t *background = lv_obj_create(lv_layer_bottom());
    lv_obj_set_size(background, 1024, 600);
    lv_obj_set_style_bg_color(background, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_border_width(background, 0, 0);
}
