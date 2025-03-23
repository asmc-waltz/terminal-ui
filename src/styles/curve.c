#include <style.h>

void style_init_curve(lv_style_t *style) {
    lv_style_init(style);
    lv_style_set_text_color(style, lv_palette_darken(LV_PALETTE_GREEN, 3));
    lv_style_set_line_width(style, 2); /*Tick width*/
    lv_style_set_line_color(style, lv_color_hex(0x2f3237));
    lv_style_set_radius(style, 12);
    lv_style_set_bg_color(style,  lv_color_hex(0xffd15c));

}

