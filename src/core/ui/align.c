/**
 * @file align.c
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
void set_gobj_pos(lv_obj_t *lobj, int32_t x_ofs, int32_t y_ofs)
{
    gobj_t *gobj = NULL;
    LV_ASSERT_NULL(lobj);

    lv_obj_set_pos(lobj, x_ofs, y_ofs);

    gobj = lobj->user_data;
    if (!gobj->size.w)
        LOG_WARN("Cannot calculate the center x");
    if (!gobj->size.h)
        LOG_WARN("Cannot calculate the center y");
    gobj->aln.mid_x = x_ofs + (gobj->size.w / 2);
    gobj->aln.mid_y = y_ofs + (gobj->size.h / 2);
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
    if (!gobj->size.w)
        LOG_WARN("Cannot calculate the center x");
    if (!gobj->size.h)
        LOG_WARN("Cannot calculate the center y");
    gobj->aln.mid_x = x_ofs + (gobj->size.w / 2);
    gobj->aln.mid_y = y_ofs + (gobj->size.h / 2);
}

void set_gobj_align(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                      int32_t x_ofs_px, int32_t y_ofs_px)
{
    gobj_t *gobj = NULL;
    LV_ASSERT_NULL(lobj);

    gobj = lobj->user_data;
    LV_ASSERT_NULL(gobj);
    gobj->aln.align = align;
    gobj->aln.base = base;
    gobj->aln.x = x_ofs_px;
    gobj->aln.y = y_ofs_px;
    gobj->aln.scale_x = DIS_SCALE;
    gobj->aln.scale_y = DIS_SCALE;

    apply_gobj_align(lobj);
}

void set_gobj_align_scale_x(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                            int32_t x_ofs_pct, int32_t y_ofs_px)
{
    gobj_t *gobj = NULL;

    LV_ASSERT_NULL(lobj);
    gobj = get_gobj(lobj);
    LV_ASSERT_NULL(gobj);

    gobj->aln.align = align;
    gobj->aln.base = base;
    gobj->aln.x = x_ofs_pct;
    gobj->aln.y = y_ofs_px;
    gobj->aln.scale_x = ENA_SCALE;
    gobj->aln.scale_y = DIS_SCALE;

    apply_gobj_align(lobj);
}

void set_gobj_align_scale_y(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                            int32_t x_ofs_px, int32_t y_ofs_pct)
{
    gobj_t *gobj = NULL;

    LV_ASSERT_NULL(lobj);
    gobj = get_gobj(lobj);
    LV_ASSERT_NULL(gobj);

    gobj->aln.align = align;
    gobj->aln.base = base;
    gobj->aln.x = x_ofs_px;
    gobj->aln.y = y_ofs_pct;
    gobj->aln.scale_x = DIS_SCALE;
    gobj->aln.scale_y = ENA_SCALE;

    apply_gobj_align(lobj);
}

void set_gobj_align_scale(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                             int32_t x_ofs_pct, int32_t y_ofs_pct)
{
    gobj_t *gobj = NULL;

    LV_ASSERT_NULL(lobj);
    gobj = get_gobj(lobj);
    LV_ASSERT_NULL(gobj);

    gobj->aln.align = align;
    gobj->aln.base = base;
    gobj->aln.x = x_ofs_pct;
    gobj->aln.y = y_ofs_pct;
    gobj->aln.scale_x = ENA_SCALE;
    gobj->aln.scale_y = ENA_SCALE;

    apply_gobj_align(lobj);
}

void apply_gobj_align(lv_obj_t *lobj)
{
    int32_t x_ofs_px;
    int32_t y_ofs_px;
    gobj_t *gobj = NULL;

    LV_ASSERT_NULL(lobj);
    gobj = get_gobj(lobj);
    LV_ASSERT_NULL(gobj);

    if (gobj->aln.scale_x == ENA_SCALE)
        x_ofs_px = calc_pixels(obj_width((gobj->par)->obj), gobj->aln.x);
    else
        x_ofs_px = gobj->aln.x;

    if (gobj->aln.scale_y == ENA_SCALE)
        y_ofs_px = calc_pixels(obj_height((gobj->par)->obj), gobj->aln.y);
    else
        y_ofs_px = gobj->aln.y;

    lv_obj_align_to(lobj, gobj->aln.base, gobj->aln.align, x_ofs_px, y_ofs_px);
}

