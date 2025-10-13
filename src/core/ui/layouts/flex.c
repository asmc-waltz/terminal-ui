/**
 * @file flex.c
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

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <stdbool.h>

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
static bool is_rotation_same_group(lv_obj_t *lobj)
{
    gobj_t *gobj;
    int32_t scr_rot, cur_rot;

    gobj = lobj ? get_gobj(lobj) : NULL;
    if (!gobj)
        return false;

    cur_rot = gobj->data.rotation;
    scr_rot = get_scr_rotation();

    bool cur_positive = (cur_rot == ROTATION_0 || cur_rot == ROTATION_270);
    bool scr_positive = (scr_rot == ROTATION_0 || scr_rot == ROTATION_270);

    return cur_positive == scr_positive;
}

static int32_t swap_align_value(lv_obj_t *lobj, lv_flex_align_t align)
{
    const char *name = get_name(lobj);

    if (align == LV_FLEX_ALIGN_START) {
        LOG_TRACE("Flex [%s] align START (%d) -> END", name, align);
        return LV_FLEX_ALIGN_END;
    } else if (align == LV_FLEX_ALIGN_END) {
        LOG_TRACE("Flex [%s] align END (%d) -> START", name, align);
        return LV_FLEX_ALIGN_START;
    }

    LOG_TRACE("Flex [%s] normal align (%d)", name, align);
    return align;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
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

    get_gobj(cont)->layout.data = conf;

    lv_obj_set_layout(cont, LV_LAYOUT_FLEX);
    lv_obj_center(cont);

    if (set_layout_type(cont, OBJ_LAYOUT_FLEX)) {
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

/*
 * Alignment is not swapped consistently across each 90-degree rotation,
 * so we cannot reuse the same rule to produce a rotate-90 function for all cases.
 */
int32_t rotate_flex_align_one(lv_obj_t *lobj)
{
    flex_layout_t *conf = NULL;
    lv_flex_align_t main_place, cross_place, track_cross_place;
    int32_t ret;

    if (is_rotation_same_group(lobj)) {
        return 0;
    }

    conf = lobj ? get_flex_layout_data(lobj) : NULL;
    if (!conf)
        return -EINVAL;

    /*
     * Due to the limitation of the LVGL flex engine, when the main place \
     * alignment rotates and matches case START -> END, it aligns objects from \
     * the end of the parent. From that point, all relative calculations based \
     * on parent size and scroll become incorrect. Therefore, we must keep the \
     * original configuration for this value.
     * NOTE: Please be careful when selecting LV_FLEX_ALIGN_END for the main \
     * place alignment.
     */
    main_place = conf->main_place;
    if (main_place == LV_FLEX_ALIGN_END) {
        LOG_WARN("Flex [%s] align main place LV_FLEX_ALIGN_END (%d)", \
                 get_name(lobj), main_place);
    }

    cross_place = swap_align_value(lobj, conf->cross_place);
    track_cross_place = swap_align_value(lobj, conf->track_place);

    return config_flex_layout_align(lobj, main_place, cross_place, \
                                   track_cross_place);
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
    if (ret)
        return ret;

    return 0;
}

int32_t apply_flex_layout_config(lv_obj_t *lobj)
{
    int32_t ret;

    ret = apply_flex_layout_flow(lobj);
    if (ret)
        return ret;

    ret = apply_flex_layout_align(lobj);
    if (ret)
        return ret;

    return 0;
}

int32_t rotate_flex_cell_90(lv_obj_t *lobj)
{
    int32_t ret;

    if (!lobj )
        return -EINVAL;

    set_size(lobj, get_h(lobj), get_w(lobj));

    return 0;
}

int32_t apply_flex_cell_config(lv_obj_t *lobj)
{
    return 0;
}
