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
        case OBJ_BOX:
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
    gobj->data.obj_type = type;
    gobj->data.rotation = ROTATION_0;
    gobj->align.value = LV_ALIGN_DEFAULT;
    gobj->align.list = LIST_NONE;

    LV_ASSERT_NULL(gobj);

    return get_lobj(gobj);
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
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

    lv_obj_clear_flag(box, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(box, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_style_bg_opa(box, LV_OPA_0, 0);
    // lv_obj_set_style_bg_color(box, \
    //                           lv_color_hex(0x00AA00), 0);

    text = create_text(box, NULL, font, str);
    if (!text) {
        remove_obj_and_child_by_name(name, &get_gobj(par)->child);
        return NULL;
    }

    set_gobj_size(box, get_w(text), get_h(text));
    set_gobj_pos_center(text);
    lv_obj_add_flag(text, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_clear_flag(text, LV_OBJ_FLAG_CLICKABLE);

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
    lv_obj_clear_flag(box, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(box, LV_OBJ_FLAG_EVENT_BUBBLE);
    set_gobj_size(box, pct_to_px(get_h(par), 80), \
                     pct_to_px(get_h(par), 80));

    icon = create_sym(box, NULL, font, index);
    if (!icon) {
        remove_obj_and_child_by_name(name, &get_gobj(par)->child);
        return NULL;
    }

    set_gobj_size(box, get_w(icon), get_h(icon));
    set_gobj_pos_center(icon);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_clear_flag(icon, LV_OBJ_FLAG_CLICKABLE);

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

int32_t redraw_slider_layout(lv_obj_t *lobj)
{
    gobj_t *gobj;
    int32_t scr_rot, cur_rot;
    int32_t min, max;

    gobj = lobj ? get_gobj(lobj) : NULL;
    if (!gobj)
        return -EINVAL;

    cur_rot = gobj->data.rotation;
    scr_rot = get_scr_rotation();

    if (((scr_rot == ROTATION_0 || scr_rot == ROTATION_270) &&
         (cur_rot == ROTATION_90 || cur_rot == ROTATION_180)) ||
        ((scr_rot == ROTATION_90 || scr_rot == ROTATION_180) &&
         (cur_rot == ROTATION_0 || cur_rot == ROTATION_270))) {

        min = lv_slider_get_max_value(lobj);
        max = lv_slider_get_min_value(lobj);
        lv_slider_set_range(lobj, min, max);
    }

    return 0;
}

lv_obj_t *create_slider(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj = create_gobj(par, OBJ_SLIDER, name);
    LV_ASSERT_NULL(lobj);

    get_gobj(lobj)->data.pre_rot_redraw_cb = redraw_slider_layout;

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
