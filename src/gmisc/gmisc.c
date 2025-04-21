/**
 * @file gmisc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <gmisc.h>

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
lv_obj_t * g_create_panel(lv_obj_t *parent, lv_style_t *style, int32_t w, int32_t h) {
    lv_obj_t *bg = lv_obj_create(parent);
    lv_obj_set_size(bg, w, h);
    lv_obj_add_style(bg, style, 0);
    return bg;
}

lv_obj_t * g_create_background(lv_obj_t *parent, lv_style_t *style, int32_t w, int32_t h) {
    lv_obj_t *bg = g_create_panel(parent, style, w, h);

    static lv_style_t style_grad;
    lv_style_init(&style_grad);
    lv_style_set_bg_color(&style_grad, lv_color_hex(0x478DDF));
    lv_style_set_bg_grad_color(&style_grad, lv_color_hex(0x66D569));
    lv_style_set_bg_grad_dir(&style_grad, LV_GRAD_DIR_HOR);
    lv_style_set_bg_opa(&style_grad, LV_OPA_COVER);
    lv_obj_add_style(bg, &style_grad, 0);

    return bg;
}

