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
