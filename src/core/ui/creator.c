/**
 * @file creator.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
// #define LOG_LEVEL LOG_LEVEL_TRACE
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
static lv_obj_t *create_gobj(lv_obj_t *par, int32_t type, \
                                     const char *name)
{
    gobj_t *gobj = NULL;
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

    gobj = register_obj(par, lobj, name);
    gobj->type = type;
    gobj->pos.rot = ROTATION_0;
    gobj->aln.align = LV_ALIGN_DEFAULT;
    gobj->aln.flex = FLEX_NONE;
    gobj->scale.ena_w = 0;
    gobj->scale.ena_h = 0;

    LV_ASSERT_NULL(gobj);

    return gobj->obj;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t *create_base(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj = create_gobj(par, OBJ_BASE, name);
    lv_obj_set_style_pad_all(lobj, 0, 0);
    lv_obj_set_style_pad_gap(lobj, 0, 0);
    lv_obj_set_style_border_width(lobj, 0, 0);
    lv_obj_set_style_outline_width(lobj, 0, 0);
    lv_obj_set_style_shadow_width(lobj, 0, 0);
    return lobj;
}

lv_obj_t *create_box(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj = create_gobj(par, OBJ_BOX, name);
    lv_obj_set_style_pad_all(lobj, 0, 0);
    lv_obj_set_style_pad_gap(lobj, 0, 0);
    lv_obj_set_style_border_width(lobj, 0, 0);
    lv_obj_set_style_outline_width(lobj, 0, 0);
    lv_obj_set_style_shadow_width(lobj, 0, 0);
    return lobj;
}

lv_obj_t *create_container(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj = create_gobj(par, OBJ_CONTAINER, name);
    lv_obj_set_style_pad_all(lobj, 0, 0);
    lv_obj_set_style_pad_gap(lobj, 0, 0);
    return lobj;
}

/*
 * Fn: create_text
 *     create_sym
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

/*
 * Text object is automatically sized based on font.
 * Offset is always aligned to the center of the parent object.
 * Ensure parent object size is set before creating a text object.
 */
lv_obj_t *create_text(lv_obj_t *par, const char *name, \
                          const lv_font_t *font, const char *txt_str)
{
    int32_t w, h;
    lv_obj_t *lobj = create_gobj(par, OBJ_LABEL, name);
    LV_ASSERT_NULL(lobj);
    lv_obj_set_style_text_font(lobj, font, 0);
    lv_label_set_text_fmt(lobj, "%s", txt_str);
    lv_obj_update_layout(lobj);
    w = lv_obj_get_width(lobj);
    h = lv_obj_get_height(lobj);
    set_gobj_size(lobj, w, h);
    set_gobj_pos_center(lobj);
    return lobj;
}

lv_obj_t *create_text_box(lv_obj_t *par, const char *name, \
                          const lv_font_t *font, const char *str)
{
    lv_obj_t *box, *text;

    box = create_box(par, name);
    if (!box) {
        return NULL;
    }

    lv_obj_clear_flag(box, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(box, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_style_bg_opa(box, LV_OPA_0, 0);
    // lv_obj_set_style_bg_color(box, \
    //                           lv_color_hex(0x00AA00), 0);

    text = create_text(box, NULL, font, str);
    if (!text) {
        remove_obj_and_child_by_name(name, &(get_gobj(par))->child);
        return NULL;
    }

    set_gobj_size(box, obj_width(text), obj_height(text));
    set_gobj_pos_center(text);
    lv_obj_add_flag(text, LV_OBJ_FLAG_EVENT_BUBBLE);

    return box;
}

lv_obj_t *create_sym(lv_obj_t *par, const char *name, \
                        const lv_font_t *font, const char *index)
{

    int32_t w, h;
    lv_obj_t *lobj = create_gobj(par, OBJ_ICON, name);
    LV_ASSERT_NULL(lobj);
    lv_obj_set_style_text_font(lobj, font, 0);
    lv_label_set_text(lobj, index);
    lv_obj_update_layout(lobj);
    w = lv_obj_get_width(lobj);
    h = lv_obj_get_height(lobj);
    set_gobj_size(lobj, w, h);
    set_gobj_pos_center(lobj);
    return lobj;
}

lv_obj_t *create_symbol_box(lv_obj_t *par, const char *name, \
                            const lv_font_t *font, const char *index)
{
    lv_obj_t *box, *icon;

    box = create_box(par, name);
    if (!box) {
        return NULL;
    }

    lv_obj_set_style_bg_opa(box, LV_OPA_0, 0);
    // lv_obj_set_style_bg_color(box, \
    //                           lv_color_hex(0x00AA00), 0);
    lv_obj_clear_flag(box, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(box, LV_OBJ_FLAG_EVENT_BUBBLE);
    set_gobj_size(box, calc_pixels(obj_height(par), 80), \
                     calc_pixels(obj_height(par), 80));

    icon = create_sym(box, NULL, font, index);
    if (!icon) {
        remove_obj_and_child_by_name(name, &(get_gobj(par))->child);
        return NULL;
    }

    set_gobj_size(box, obj_width(icon), obj_height(icon));
    set_gobj_pos_center(icon);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_EVENT_BUBBLE);

    return box;
}

lv_obj_t *create_switch(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj = create_gobj(par, OBJ_SWITCH, name);
    LV_ASSERT_NULL(lobj);
    return lobj;
}

lv_obj_t *create_textarea(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj = create_gobj(par, OBJ_TEXTAREA, name);
    LV_ASSERT_NULL(lobj);
    return lobj;
}

lv_obj_t *create_btn(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj = create_gobj(par, OBJ_BTN, name);
    LV_ASSERT_NULL(lobj);

    lv_obj_set_style_pad_all(lobj, 0, 0);
    lv_obj_set_style_pad_gap(lobj, 0, 0);
    // lv_obj_set_style_border_width(lobj, 0, 0);
    lv_obj_set_style_outline_width(lobj, 0, 0);
    // lv_obj_set_style_shadow_width(lobj, 0, 0);
    return lobj;
}

lv_obj_t *create_slider(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj = create_gobj(par, OBJ_SLIDER, name);
    LV_ASSERT_NULL(lobj);
    return lobj;
}

lv_obj_t *get_box_child(lv_obj_t *lobj)
{
    lv_obj_t *child = lv_obj_get_child(lobj, 0);
    if (child)
        return child;
    else
        return NULL;
}

/******************************************************************************/
void set_gobj_pos(lv_obj_t *lobj, int32_t x_ofs, int32_t y_ofs)
{
    gobj_t *gobj = NULL;
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

void set_gobj_pos_center(lv_obj_t *lobj)
{
    gobj_t *gobj = NULL;
    lv_obj_t *par;
    int32_t x_ofs, y_ofs;

    LV_ASSERT_NULL(lobj);

    // lv_obj_set_pos(lobj, x_ofs, y_ofs);
    par = lv_obj_get_parent(lobj);
    x_ofs = (obj_width(par) - lv_obj_get_width(lobj)) / 2;
    y_ofs = (obj_height(par) - lv_obj_get_height(lobj)) / 2;

    set_gobj_pos(lobj, x_ofs, y_ofs);

    gobj = get_gobj(lobj);
    if (!gobj->pos.w)
        LOG_WARN("Cannot calculate the center x");
    if (!gobj->pos.h)
        LOG_WARN("Cannot calculate the center y");
    gobj->pos.x_mid = x_ofs + (gobj->pos.w / 2);
    gobj->pos.y_mid = y_ofs + (gobj->pos.h / 2);
}

void align_gobj_to(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                      int32_t x_ofs, int32_t y_ofs)
{
    gobj_t *gobj = NULL;
    LV_ASSERT_NULL(lobj);

    lv_obj_align_to(lobj, base, align, x_ofs, y_ofs);

    gobj = lobj->user_data;
    LV_ASSERT_NULL(gobj);
    gobj->aln.align = align;
    gobj->aln.base = base;
    gobj->aln.x = x_ofs;
    gobj->aln.y = y_ofs;
    gobj->aln.scale = DIS_SCALE;
}

void align_gobj_scale(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                      int32_t x_ofs, int32_t y_ofs)
{
    gobj_t *gobj = NULL;
    LV_ASSERT_NULL(lobj);


    gobj = lobj->user_data;
    LV_ASSERT_NULL(gobj);
    gobj->aln.align = align;
    gobj->aln.base = base;
    gobj->aln.x = x_ofs;
    gobj->aln.y = y_ofs;
    gobj->aln.scale = ENA_SCALE;

    lv_obj_align_to(lobj, base, align, \
                        calc_pixels(obj_width((gobj->par)->obj), gobj->aln.x), \
                        calc_pixels(obj_height((gobj->par)->obj), gobj->aln.y));
}
