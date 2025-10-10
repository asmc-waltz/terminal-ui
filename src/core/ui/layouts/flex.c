/**
 * @file flex.c
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

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include <lvgl.h>
#include "list.h"
#include "ui/ui_core.h"
#include "ui/flex.h"

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
/*
 * Flex cell object sometimes is not just a normal object — it could be another
 * layout, button, or anything depending on the creator. Instead of creating it
 * in a specific type, we let the creator build it, then attach its flex cell
 * configuration here. This enables rotation-aware properties and layout
 * compatibility.
 */
int32_t set_flex_cell_data(lv_obj_t *lobj)
{
    if (!lobj)
        return -EINVAL;

    gobj_t *gobj = get_gobj(lobj);
    if (!gobj)
        return -EINVAL;

    flex_cell_t *conf = (flex_cell_t *)calloc(1, sizeof(flex_cell_t));
    if (!conf)
        return -ENOMEM;

    gobj->data.sub_data = conf;

    int32_t ret = set_obj_cell_type(lobj, OBJ_FLEX_CELL);
    if (ret) {
        free(conf);
        gobj->data.sub_data = NULL;
        return ret;
    }

    return 0;
}

/*
 * Configure flex cell border side only (does not apply to LVGL yet).
 */
int32_t config_flex_cell_border_side(lv_obj_t *lobj, int32_t value)
{
    flex_cell_t *conf;

    if (!lobj)
        return -EINVAL;

    conf = get_flex_cell_data(lobj);
    if (!conf)
        return -EINVAL;

    conf->border_side = value;

    return 0;
}

/*
 * Apply border side configuration to the LVGL object.
 */
int32_t apply_flex_cell_border_side(lv_obj_t *lobj)
{
    flex_cell_t *conf;

    if (!lobj)
        return -EINVAL;

    conf = get_flex_cell_data(lobj);
    if (!conf)
        return -EINVAL;

    lv_obj_set_style_border_side(lobj, conf->border_side, 0);
    return 0;
}

/*
 * Set and immediately apply border side configuration for a flex cell.
 */
int32_t set_flex_cell_border_side(lv_obj_t *lobj, int32_t value)
{
    int32_t ret;

    ret = config_flex_cell_border_side(lobj, value);
    if (ret)
        return ret;

    return apply_flex_cell_border_side(lobj);
}

int32_t config_flex_cell_pad(lv_obj_t *lobj, int32_t pad_top, int32_t pad_bot, \
                             int32_t pad_left, int32_t pad_right)
{
    flex_cell_t *conf;

    if (!lobj)
        return -EINVAL;

    conf = get_flex_cell_data(lobj);
    if (!conf)
        return -EINVAL;

    conf->pad_top = pad_top;
    conf->pad_bot = pad_bot;
    conf->pad_left = pad_left;
    conf->pad_right = pad_right;

    return 0;
}

/*
 * Apply border side configuration to the LVGL object.
 */
int32_t apply_flex_cell_pad(lv_obj_t *lobj)
{
    flex_cell_t *conf;

    if (!lobj)
        return -EINVAL;

    conf = get_flex_cell_data(lobj);
    if (!conf)
        return -EINVAL;

    lv_obj_set_style_pad_top(lobj, conf->pad_top, 0);
    lv_obj_set_style_pad_bottom(lobj, conf->pad_bot, 0);
    lv_obj_set_style_pad_left(lobj, conf->pad_left, 0);
    lv_obj_set_style_pad_right(lobj, conf->pad_right, 0);

    return 0;
}

int32_t set_flex_cell_pad(lv_obj_t *lobj, int32_t pad_top, int32_t pad_bot, \
                          int32_t pad_left, int32_t pad_right)
{
    int32_t ret;

    ret = set_flex_cell_pad(lobj, pad_top, pad_bot, pad_left, pad_right);
    if (ret)
        return ret;

    return apply_flex_cell_pad(lobj);
}

lv_obj_t *create_flex_layout_object(lv_obj_t *par, const char *name)
{
    lv_obj_t *cont = NULL;
    flex_layout_t *conf = NULL;

    if (!par)
        return NULL;

    conf = (flex_layout_t *)calloc(1, sizeof(flex_layout_t));
    if (!conf)
        return NULL;

    cont = create_box(par, name);
    if (!cont)
        goto out_free_conf;

    get_gobj(cont)->data.internal = conf;

    lv_obj_set_layout(cont, LV_LAYOUT_FLEX);
    lv_obj_center(cont);

    if (set_obj_layout_type(cont, OBJ_LAYOUT_FLEX)) {
        LOG_ERROR("Failed to set object sub type");
        goto out_free_conf;
    }

    return cont;

out_free_conf:
    free(conf);
    return NULL;
}


int32_t config_flex_layout_align(lv_obj_t *lobj, lv_flex_align_t main_place, \
                                 lv_flex_align_t cross_place, \
                                 lv_flex_align_t track_cross_place)
{
    flex_layout_t *conf = NULL;
    int32_t ret;

    conf = lobj ? get_flex_layout_data(lobj) : NULL;
    if (!conf)
        return -EINVAL;

    conf->main_place = main_place;
    conf->cross_place = cross_place;
    conf->track_place = track_cross_place;

    return 0;
}

int32_t apply_flex_layout_align(lv_obj_t *lobj)
{
    flex_layout_t *conf = NULL;
    int32_t ret;

    conf = lobj ? get_flex_layout_data(lobj) : NULL;
    if (!conf)
        return -EINVAL;


    lv_obj_set_flex_align(lobj, conf->main_place, conf->cross_place, \
                          conf->track_place);

    return 0;
}


int32_t set_flex_layout_align(lv_obj_t *lobj, lv_flex_align_t main_place, \
                                 lv_flex_align_t cross_place, \
                                 lv_flex_align_t track_cross_place)
{
    int32_t ret;

    if (!lobj)
        return -EINVAL;

    ret = config_flex_layout_align(lobj, main_place, cross_place, \
                                   track_cross_place);
    if (ret)
        return ret;


    ret = apply_flex_layout_align(lobj);
    if (ret)
        return ret;

    return 0;
}

int32_t config_flex_layout_flow(lv_obj_t *lobj, lv_flex_flow_t flow)
{
    flex_layout_t *conf = NULL;
    int32_t ret;

    conf = lobj ? get_flex_layout_data(lobj) : NULL;
    if (!conf)
        return -EINVAL;

    conf->flow = flow;

    return 0;
}

int32_t apply_flex_layout_flow(lv_obj_t *lobj)
{
    flex_layout_t *conf = NULL;
    int32_t ret;

    conf = lobj ? get_flex_layout_data(lobj) : NULL;
    if (!conf)
        return -EINVAL;

    lv_obj_set_flex_flow(lobj, conf->flow);

    return 0;
}


int32_t set_flex_layout_flow(lv_obj_t *lobj, lv_flex_flow_t flow)
{
    int32_t ret;

    if (!lobj)
        return -EINVAL;

    ret = config_flex_layout_flow(lobj, flow);
    if (ret)
        return ret;


    ret = apply_flex_layout_flow(lobj);
    if (ret)
        return ret;

    return 0;
}
/*
 * Supported:
 * LV_FLEX_FLOW_ROW: Place the children in a row without wrapping
 * LV_FLEX_FLOW_COLUMN: Place the children in a column without wrapping
 * LV_FLEX_FLOW_ROW_REVERSE: Place the children in a row without wrapping but in reversed order
 * LV_FLEX_FLOW_COLUMN_REVERSE: Place the children in a column without wrapping but in reversed order
 * TODO:
 * LV_FLEX_FLOW_ROW_WRAP: Place the children in a row with wrapping
 * LV_FLEX_FLOW_COLUMN_WRAP: Place the children in a column with wrapping
 * LV_FLEX_FLOW_ROW_WRAP_REVERSE: Place the children in a row with wrapping but in reversed order
 * LV_FLEX_FLOW_COLUMN_WRAP_REVERSE: Place the children in a column with wrapping but in reversed order
 */
int32_t rotate_flex_layout_90(lv_obj_t *lobj)
{
    lv_flex_flow_t pre_flow, next_flow;
    flex_layout_t *conf = NULL;
    int32_t ret;

    conf = lobj ? get_flex_layout_data(lobj) : NULL;
    if (!conf)
        return -EINVAL;

    pre_flow = conf->flow;

    switch(pre_flow) {
        case LV_FLEX_FLOW_ROW:
            next_flow = LV_FLEX_FLOW_COLUMN;
            break;
        case LV_FLEX_FLOW_COLUMN:
            next_flow = LV_FLEX_FLOW_ROW_REVERSE;
            break;
        case LV_FLEX_FLOW_ROW_REVERSE:
            next_flow = LV_FLEX_FLOW_COLUMN_REVERSE;
            break;
        case LV_FLEX_FLOW_COLUMN_REVERSE:
            next_flow = LV_FLEX_FLOW_ROW;
            break;
        default:
            LOG_WARN("Flex box flow is invalid");
            break;
    }

    ret = config_flex_layout_flow(lobj, next_flow);
    LOG_TRACE("Flex layout %s rotated 90 deg: flow %d -> %d", \
              get_gobj(lobj)->name, pre_flow, next_flow);

    return 0;
}

/*
 * Rotate flex cell border sides by 90° clockwise.
 * Uses a lookup table for all 16 combinations of LV_BORDER_SIDE_* flags.
 */
int32_t rotate_flex_cell_border_side_90(lv_obj_t *lobj)
{
    if (!lobj)
        return -EINVAL;

    flex_cell_t *conf = get_flex_cell_data(lobj);
    if (!conf)
        return -EINVAL;

    static const lv_border_side_t border_rot_table[16] = {
        [LV_BORDER_SIDE_NONE] = LV_BORDER_SIDE_NONE,
        [LV_BORDER_SIDE_TOP] = LV_BORDER_SIDE_RIGHT,
        [LV_BORDER_SIDE_BOTTOM] = LV_BORDER_SIDE_LEFT,
        [LV_BORDER_SIDE_LEFT] = LV_BORDER_SIDE_TOP,
        [LV_BORDER_SIDE_RIGHT] = LV_BORDER_SIDE_BOTTOM,
        [LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT] =
                                LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM,
        [LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM] =
                                LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT,
        [LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_RIGHT] =
                                LV_BORDER_SIDE_RIGHT | LV_BORDER_SIDE_BOTTOM,
        [LV_BORDER_SIDE_RIGHT | LV_BORDER_SIDE_BOTTOM] =
                                LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_BOTTOM,
        [LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_BOTTOM] =
                                LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_TOP,
        [LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_TOP] =
                                LV_BORDER_SIDE_RIGHT | LV_BORDER_SIDE_TOP,
        [LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_RIGHT | LV_BORDER_SIDE_BOTTOM] =
                                LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_BOTTOM |
                                LV_BORDER_SIDE_RIGHT,
        [LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_RIGHT] =
                                LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_RIGHT |
                                LV_BORDER_SIDE_BOTTOM,
        [LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT | LV_BORDER_SIDE_BOTTOM] =
                                LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_LEFT |
                                LV_BORDER_SIDE_BOTTOM,
        [LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_BOTTOM] =
                                LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_TOP |
                                LV_BORDER_SIDE_RIGHT,
        [LV_BORDER_SIDE_FULL] = LV_BORDER_SIDE_FULL,
    };

    lv_border_side_t current = conf->border_side;
    if (current >= 16) {
        LOG_ERROR("Flex cell %s has invalid border mask %d",
              get_name(lobj), current);
        return -EINVAL;
    }

    conf->border_side = border_rot_table[current];

    return 0;
}

int32_t rotate_flex_cell_90(lv_obj_t *lobj)
{
    int32_t ret;

    if (!lobj )
        return -EINVAL;

    set_gobj_size(lobj, get_h(lobj), get_w(lobj));

    ret = rotate_flex_cell_border_side_90(lobj);
    if (ret)
        return ret;

    return 0;
}

int32_t apply_flex_cell_config(lv_obj_t *lobj)
{
    int32_t ret = 0;

    ret = apply_flex_cell_border_side(lobj);
    if (ret)
        return ret;

    return 0;
}
