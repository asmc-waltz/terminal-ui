/**
 * @file curved_lines.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <style.h>
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
lv_style_t icons_size_20;
lv_style_t icons_size_32;
lv_style_t icons_size_48;
lv_style_t sts_bar_style;

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
 *   STATIC FUNCTIONS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void style_curved_lines_init(lv_style_t *sty) {
    lv_style_init(sty);
    lv_style_set_radius(sty, 20);
    lv_style_set_clip_corner(sty, true);

    lv_style_set_bg_opa(sty, LV_OPA_COVER);
    lv_style_set_bg_color(sty, lv_color_hex(0xffffff));

    lv_style_set_border_color(sty, lv_color_hex(0x000000));
    lv_style_set_border_width(sty, 0);
    lv_style_set_border_post(sty, true);

    lv_style_set_shadow_color(sty, lv_color_hex(0x000000));
    lv_style_set_shadow_width(sty, 50);
    lv_style_set_shadow_spread(sty, 10);

    lv_style_set_outline_color(sty, lv_color_hex(0x000000));
    lv_style_set_outline_width(sty, 2);
    lv_style_set_outline_pad(sty, 5);

    lv_style_set_text_color(sty, lv_palette_darken(LV_PALETTE_GREY, 5));
    lv_style_set_line_color(sty, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_line_width(sty, 1);
}

void icons_20_style_init(void)
{
    lv_style_init(&icons_size_20);
    lv_style_set_text_font(&icons_size_20, &terminal_icons_20);
    lv_style_set_text_color(&icons_size_20, lv_color_hex(0x00FF00));
}

void icons_32_style_init(void)
{
    lv_style_init(&icons_size_32);
    lv_style_set_text_font(&icons_size_32, &terminal_icons_32);
    lv_style_set_text_color(&icons_size_32, lv_color_hex(0x00FF00));
}

void icons_48_style_init(void)
{
    lv_style_init(&icons_size_48);
    lv_style_set_text_font(&icons_size_48, &terminal_icons_48);
    lv_style_set_text_color(&icons_size_48, lv_color_hex(0xffffff));
}

void status_bar_style_init(void)
{
    lv_style_init(&sts_bar_style);
    lv_style_set_radius(&sts_bar_style, 20);
    lv_style_set_bg_opa(&sts_bar_style, LV_OPA_TRANSP);
    lv_style_set_bg_color(&sts_bar_style, lv_color_black());
    lv_style_set_text_font(&sts_bar_style, &terminal_icons_32);
    lv_style_set_text_color(&sts_bar_style, lv_color_black());
    lv_style_set_border_color(&sts_bar_style, lv_color_hex(0x000000));
    lv_style_set_border_width(&sts_bar_style, 0);
    lv_style_set_border_post(&sts_bar_style, true);
    lv_style_set_pad_all(&sts_bar_style, 0);
    lv_style_set_shadow_color(&sts_bar_style, lv_color_hex(0x000000));
    lv_style_set_shadow_width(&sts_bar_style, 0);
    lv_style_set_shadow_spread(&sts_bar_style, 0);
    lv_style_set_outline_color(&sts_bar_style, lv_color_hex(0x000000));
    lv_style_set_outline_width(&sts_bar_style, 0);
    lv_style_set_outline_pad(&sts_bar_style, 0);
}

