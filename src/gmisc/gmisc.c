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
lv_obj_t * gf_create_panel(lv_obj_t *parent, lv_style_t *style, int32_t w, int32_t h) {
    lv_obj_t *bg = lv_obj_create(parent);
    lv_obj_set_size(bg, w, h);
    lv_obj_add_style(bg, style, 0);
    return bg;
}

lv_obj_t * gf_create_background(lv_obj_t *parent, lv_style_t *style, int32_t w, int32_t h) {
    lv_obj_t *bg = gf_create_panel(parent, style, w, h);

    static lv_style_t style_grad;
    lv_style_init(&style_grad);
    lv_style_set_bg_color(&style_grad, lv_color_hex(0x478DDF));
    lv_style_set_bg_grad_color(&style_grad, lv_color_hex(0x66D569));
    lv_style_set_bg_grad_dir(&style_grad, LV_GRAD_DIR_HOR);
    lv_style_set_bg_opa(&style_grad, LV_OPA_COVER);
    lv_obj_add_style(bg, &style_grad, 0);

    return bg;
}

void gf_refresh_all_layer(void)
{
    lv_obj_invalidate(lv_layer_sys());
    lv_obj_invalidate(lv_layer_top());
    lv_obj_invalidate(lv_screen_active());
    lv_obj_invalidate(lv_layer_bottom());
}

id_data * gf_init_user_data(lv_obj_t *obj)
{
    LV_ASSERT_NULL(obj);
    id_data *p_data = NULL;
    p_data = lv_malloc_zeroed(sizeof(id_data));
    LV_ASSERT_MALLOC(p_data);
    lv_obj_set_user_data(obj, (void *)p_data);
}

void gf_free_user_data(lv_obj_t *obj)
{
    LV_ASSERT_NULL(obj);
    id_data *p_data = obj->user_data;
    lv_free(p_data);
}

char *gf_set_name(lv_obj_t *obj, char *name)
{
    LV_ASSERT_NULL(obj);
    id_data *p_data = obj->user_data;
    LV_ASSERT_NULL(p_data);
    p_data->name = name;
    return p_data->name;
}

char *gf_get_name(lv_obj_t *obj)
{
    LV_ASSERT_NULL(obj);
    id_data *p_data = obj->user_data
    LV_ASSERT_NULL(p_data);
    return p_data->name;
}

lv_obj_t * gf_create_icon_bg(lv_obj_t *par, lv_style_t *bg_style, uint32_t bg_color)
{
    LV_ASSERT_NULL(par);
    lv_obj_t *icon_bg = lv_btn_create(par);
    lv_obj_add_style(icon_bg, bg_style, 0);
    lv_obj_set_style_bg_color(icon_bg, lv_color_hex(bg_color), 0);
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

