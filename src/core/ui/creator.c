/**
 * @file creator.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL defined locally will override the global setting in this file"
#endif
#include "log.h"

#include <stdint.h>
#include <errno.h>

#include <lvgl.h>
#include "list.h"
#include "ui/ui_core.h"


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
static lv_obj_t *gf_create_gobj_type(lv_obj_t *par, int32_t type, \
                                     const char *name)
{
    g_obj *gobj = NULL;
    lv_obj_t *lobj = NULL;

    LV_ASSERT_NULL(par);

    switch (type) {
        case OBJ_BASE:
        case OBJ_BOX:
        case OBJ_CONTAINER:
            lobj = lv_obj_create(par);
            break;
        case OBJ_BTN:
            lobj = lv_btn_create(par);
            break;
        case OBJ_SLIDER:
            lobj = lv_slider_create(par);
            break;
        case OBJ_LABEL:
        case OBJ_ICON:
            lobj = lv_label_create(par);
            break;
        case OBJ_SWITCH:
            lobj = lv_switch_create(par);
            break;
        case OBJ_TEXTAREA:
            lobj = lv_textarea_create(par);
            break;
        default:
            LOG_ERROR("G Object type %d - name %s invalid", type, name);
            lobj = NULL;
            break;

    }

    LV_ASSERT_NULL(lobj);

    gobj = gf_register_obj(par, lobj, name);
    gobj->type = type;
    gobj->pos.rot = ROTATION_0;
    gobj->aln.align = LV_ALIGN_DEFAULT;
    gobj->scale.ena_w = 0;
    gobj->scale.ena_h = 0;

    LV_ASSERT_NULL(gobj);

    return gobj->obj;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t * gf_create_base(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj = gf_create_gobj_type(par, OBJ_BASE, name);
    lv_obj_set_style_pad_all(lobj, 0, 0);
    lv_obj_set_style_pad_gap(lobj, 0, 0);
    lv_obj_set_style_border_width(lobj, 0, 0);
    lv_obj_set_style_outline_width(lobj, 0, 0);
    lv_obj_set_style_shadow_width(lobj, 0, 0);
    return lobj;
}

lv_obj_t * gf_create_box(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj = gf_create_gobj_type(par, OBJ_BOX, name);
    lv_obj_set_style_pad_all(lobj, 0, 0);
    lv_obj_set_style_pad_gap(lobj, 0, 0);
    lv_obj_set_style_border_width(lobj, 0, 0);
    lv_obj_set_style_outline_width(lobj, 0, 0);
    lv_obj_set_style_shadow_width(lobj, 0, 0);
    return lobj;
}

lv_obj_t * gf_create_container(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj = gf_create_gobj_type(par, OBJ_CONTAINER, name);
    lv_obj_set_style_pad_all(lobj, 0, 0);
    lv_obj_set_style_pad_gap(lobj, 0, 0);
    return lobj;
}

/*
 * Fn: gf_create_text
 *     gf_create_sym
 * The textbox uses transform rotation instead of layout change like other
 * components, so its root coordinate changes. This makes it difficult to
 * apply normal object alignment as with other components. Additionally, the
 * transform affects the logical size, impacting its parent. The workaround is
 * to create a textbox container with scrolling disabled. Inside this box, we
 * manually align the text or symbol, while the container itself can align to
 * another object as normal.
 *
 * WARNING: Transform rotation objects are aligned and moved using pixel data
 * from g_obj. Therefore, LV_SIZE_CONTENT must not be stored as pixel values
 * in data. Doing so may cause unexpected rotation behavior.
 * Always use explicit pixel values for such objects when applying rotation.
 *
 */
lv_obj_t * gf_create_text(lv_obj_t *par, const char *name, int32_t x, \
                          int32_t y, const char *txt_str)
{
    int32_t w, h;
    lv_obj_t *lobj = gf_create_gobj_type(par, OBJ_LABEL, name);
    LV_ASSERT_NULL(lobj);
    lv_obj_set_style_text_font(lobj, &lv_font_montserrat_18, 0);
    lv_label_set_text_fmt(lobj, "%s", txt_str);
    lv_obj_update_layout(lobj);
    w = lv_obj_get_width(lobj);
    h = lv_obj_get_height(lobj);
    gf_gobj_set_size(lobj, w, h);
    gf_gobj_set_pos(lobj, x, y);
    return lobj;
}

lv_obj_t * gf_create_sym(lv_obj_t *par, const char *name, int32_t x, \
                         int32_t y, const lv_font_t *font, const char *index, \
                         lv_color_t color)
{
    lv_obj_t *lobj = gf_create_gobj_type(par, OBJ_ICON, name);
    LV_ASSERT_NULL(lobj);
    lv_obj_set_style_text_font(lobj, font, 0);
    lv_obj_set_style_text_color(lobj, color, 0);
    lv_label_set_text(lobj, index);
    gf_gobj_get_size(lobj);
    gf_gobj_set_pos(lobj, x, y);
    return lobj;
}

lv_obj_t * gf_create_switch(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj = gf_create_gobj_type(par, OBJ_SWITCH, name);
    LV_ASSERT_NULL(lobj);
    return lobj;
}

lv_obj_t * gf_create_textarea(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj = gf_create_gobj_type(par, OBJ_TEXTAREA, name);
    LV_ASSERT_NULL(lobj);
    return lobj;
}

lv_obj_t * gf_create_btn(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj = gf_create_gobj_type(par, OBJ_BTN, name);
    LV_ASSERT_NULL(lobj);

    lv_obj_set_style_pad_all(lobj, 0, 0);
    lv_obj_set_style_pad_gap(lobj, 0, 0);
    // lv_obj_set_style_border_width(lobj, 0, 0);
    lv_obj_set_style_outline_width(lobj, 0, 0);
    // lv_obj_set_style_shadow_width(lobj, 0, 0);
    return lobj;
}

lv_obj_t * gf_create_slider(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj = gf_create_gobj_type(par, OBJ_SLIDER, name);
    LV_ASSERT_NULL(lobj);
    return lobj;
}

/******************************************************************************/
void gf_gobj_set_pos(lv_obj_t *lobj, int32_t x_ofs, int32_t y_ofs)
{
    g_obj *gobj = NULL;
    LV_ASSERT_NULL(lobj);

    lv_obj_set_pos(lobj, x_ofs, y_ofs);

    gobj = lobj->user_data;
    if (!gobj->pos.w)
        LOG_WARN("Cannot calculate the center x");
    if (!gobj->pos.h)
        LOG_WARN("Cannot calculate the center y");
    gobj->pos.x_mid = x_ofs + (gobj->pos.w / 2);
    gobj->pos.y_mid = y_ofs + (gobj->pos.h / 2);
}

void gf_gobj_align_to(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                      int32_t x_ofs, int32_t y_ofs)
{
    g_obj *gobj = NULL;
    LV_ASSERT_NULL(lobj);

    lv_obj_align_to(lobj, base, align, x_ofs, y_ofs);

    gobj = lobj->user_data;
    LV_ASSERT_NULL(gobj);
    gobj->aln.align = align;
    gobj->aln.base = base;
    gobj->aln.x = x_ofs;
    gobj->aln.y = y_ofs;
}

