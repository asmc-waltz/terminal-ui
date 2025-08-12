/**
 * @file plat.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>

#include <lvgl.h>
#include <gmisc.h>
#include <style.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

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
lv_obj_t * gf_create_frame(lv_obj_t *parent, uint32_t id, uint32_t w, uint32_t h) {
    lv_obj_t *bg = gf_create_obj(parent, id);
    lv_obj_set_size(bg, w, h);
    return bg;
}

lv_obj_t * gf_create_background(lv_obj_t *parent, int32_t w, int32_t h) {
    lv_style_t *p_style = NULL;
    lv_obj_t *p_obj = NULL;

    p_obj = gf_create_frame(parent, ID_BG, w, h);
    p_style = gf_get_lv_style(STY_BG);
    lv_obj_add_style(p_obj, p_style, 0);
    return p_obj;
}

lv_obj_t * gf_create_btn_bg(lv_obj_t *par, uint32_t id, lv_style_t *bg_style, uint32_t bg_color)
{
    LV_ASSERT_NULL(par);
    lv_obj_t *icon_bg = lv_btn_create(par);
    gf_register_obj(par, icon_bg, id);
    lv_obj_add_style(icon_bg, bg_style, 0);
    lv_obj_set_style_bg_color(icon_bg, lv_color_hex(bg_color), 0);
    return icon_bg;
}

lv_obj_t * gf_create_icon_bg(lv_obj_t *par, lv_style_t *bg_style, uint32_t bg_color)
{
    LV_ASSERT_NULL(par);
    lv_obj_t *icon_bg = lv_obj_create(par);
    lv_obj_add_style(icon_bg, bg_style, 0);
    lv_obj_set_style_bg_color(icon_bg, lv_color_hex(bg_color), 0);
    lv_obj_remove_flag(icon_bg, LV_OBJ_FLAG_SCROLLABLE);

    return icon_bg;
}

lv_obj_t * gf_create_symbol(lv_obj_t *par, lv_style_t *symbol_style, uint32_t index)
{
    LV_ASSERT_NULL(par);
    lv_obj_t *symbol = lv_label_create(par);
    lv_obj_add_style(symbol, symbol_style, 0);
    lv_label_set_text(symbol, index);
    lv_obj_center(symbol);
    return symbol;
}

lv_obj_t * gf_create_interact_symbol(lv_obj_t *par, uint32_t id, lv_style_t *symbol_style, uint32_t index)
{
    LV_ASSERT_NULL(par);
    lv_obj_t *symbol = lv_label_create(par);
    gf_register_obj(par, symbol, id);
    lv_obj_add_style(symbol, symbol_style, 0);
    lv_label_set_text(symbol, index);
    lv_obj_center(symbol);
    return symbol;
}
