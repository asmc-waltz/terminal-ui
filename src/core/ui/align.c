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

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/*
 * Recalculate object's midpoint (x_mid, y_mid) when parent size
 * or screen rotation changes.
 *
 * The function preserves the relative placement rules used in the
 * original implementation: midpoint values are distances from the
 * parent's left/top edges (not the child's left/top corner).
 *
 * This version:
 *  - covers all old_rot -> scr_rot combinations (12 mappings),
 *  - validates inputs,
 *  - computes new midpoint using local variables,
 *  - validates result before updating gobj state (atomic update),
 *  - logs clearly.
 *
 * Returns:
 *  0        -> success (gobj updated)
 *  -EINVAL  -> bad input
 *  -ERANGE  -> computed midpoint is out of new parent bounds
 */
int32_t gobj_get_center(gobj_t *gobj, uint32_t par_w, uint32_t par_h)
{
    int32_t new_x_mid = -1;
    int32_t new_y_mid = -1;
    int32_t scr_rot;
    int32_t old_rot;
    int32_t old_pw;
    int32_t old_ph;
    int32_t L; /* distance from old left edge to object's center */
    int32_t T; /* distance from old top edge  to object's center */
    int32_t R; /* distance from old right edge to object's center */
    int32_t B; /* distance from old bottom edge to object's center */

    if (!gobj) {
        LOG_ERROR("null gobj");
        return -EINVAL;
    }

    scr_rot = get_scr_rotation();
    old_rot = gobj->data.rotation;

    /* nothing to do if rotation unchanged */
    if (scr_rot == old_rot)
        return 0;

    /* sanity check rotation values (expect 0..3 mapping to 0/90/180/270) */
    if (old_rot < ROTATION_0 || old_rot > ROTATION_270 ||
        scr_rot < ROTATION_0 || scr_rot > ROTATION_270) {
        LOG_ERROR("invalid rot old=%d new=%d",
                  old_rot, scr_rot);
        return -EINVAL;
    }

    /* cache old parent geometry and gaps */
    old_pw = gobj->align.par_w;
    old_ph = gobj->align.par_h;
    L = gobj->align.mid_x;
    T = gobj->align.mid_y;
    R = old_pw - L;
    B = old_ph - T;

    LOG_TRACE("obj id=%d %s - old_rot=%d -> scr_rot=%d, "
              "old_pw=%d old_ph=%d, L=%d T=%d R=%d B=%d, "
              "new_par=(%d,%d)", gobj->id, gobj->name,
              old_rot, scr_rot, old_pw, old_ph, L, T, R, B, par_w, par_h);

    /* === mapping table: old_rot -> scr_rot ===
     * We keep the exact equations used originally (explicit 12 cases).
     * Each expression computes new center coordinates relative to new parent.
     */
    switch (old_rot) {
    case ROTATION_0:
        if (scr_rot == ROTATION_90) {
            new_x_mid = par_w - T;
            new_y_mid = L;
        } else if (scr_rot == ROTATION_180) {
            new_x_mid = par_w - L;
            new_y_mid = par_h - T;
        } else if (scr_rot == ROTATION_270) {
            new_x_mid = T;
            new_y_mid = par_h - L;
        }
        break;
    case ROTATION_90:
        if (scr_rot == ROTATION_0) {
            new_x_mid = T;
            new_y_mid = old_pw - L; /* old_pw == parent's width before rotation */
        } else if (scr_rot == ROTATION_180) {
            new_x_mid = par_w - T;
            new_y_mid = par_h - (old_pw - L);
        } else if (scr_rot == ROTATION_270) {
            new_x_mid = par_w - L;
            new_y_mid = par_h - T;
        }
        break;
    case ROTATION_180:
        if (scr_rot == ROTATION_0) {
            new_x_mid = par_w - L;
            new_y_mid = par_h - T;
        } else if (scr_rot == ROTATION_90) {
            new_x_mid = par_w - (old_ph - T);
            new_y_mid = old_pw - L;
        } else if (scr_rot == ROTATION_270) {
            new_x_mid = old_ph - T;
            new_y_mid = par_h - (old_pw - L);
        }
        break;
    case ROTATION_270:
        if (scr_rot == ROTATION_0) {
            new_x_mid = old_ph - T;
            new_y_mid = L;
        } else if (scr_rot == ROTATION_90) {
            new_x_mid = par_w - L;
            new_y_mid = par_h - T;
        } else if (scr_rot == ROTATION_180) {
            new_x_mid = par_w - (old_ph - T);
            new_y_mid = par_h - L;
        }
        break;
    default:
        /* unreachable due to earlier validation */
        return -EINVAL;
    }

    /* ensure mapping was computed */
    if (new_x_mid < 0 || new_y_mid < 0) {
        LOG_ERROR("mapping not produced or negative: x=%d y=%d",
                  new_x_mid, new_y_mid);
        return -ERANGE;
    }

    /* bounds check against new parent size before mutating state */
    if (new_x_mid < 0 || new_x_mid > par_w ||
        new_y_mid < 0 || new_y_mid > par_h) {
        LOG_WARN("computed midpoint out of bounds: "
                 "x=%d (0..%d) y=%d (0..%d)",
                 new_x_mid, par_w, new_y_mid, par_h);
        return -ERANGE;
    }

    /* Atomic update of gobj position state */
    gobj->align.mid_x = new_x_mid;
    gobj->align.mid_y = new_y_mid;
    gobj->align.par_w = par_w;
    gobj->align.par_h = par_h;
    gobj->data.rotation = scr_rot;

    LOG_TRACE("success new_mid=(%d,%d) new_par=(%d,%d) rot=%d",
              new_x_mid, new_y_mid, par_w, par_h, scr_rot);

    return 0;
}

void set_gobj_pos(lv_obj_t *lobj, int32_t x_ofs, int32_t y_ofs)
{
    gobj_t *gobj = NULL;
    LV_ASSERT_NULL(lobj);

    lv_obj_set_pos(lobj, x_ofs, y_ofs);

    gobj = get_gobj(lobj);
    if (!gobj->size.w)
        LOG_WARN("Cannot calculate the center x");
    if (!gobj->size.h)
        LOG_WARN("Cannot calculate the center y");
    gobj->align.mid_x = x_ofs + (gobj->size.w / 2);
    gobj->align.mid_y = y_ofs + (gobj->size.h / 2);
}

void set_gobj_pos_center(lv_obj_t *lobj)
{
    gobj_t *gobj = NULL;
    lv_obj_t *par;
    int32_t x_ofs, y_ofs;

    LV_ASSERT_NULL(lobj);

    // lv_obj_set_pos(lobj, x_ofs, y_ofs);
    par = lv_obj_get_parent(lobj);
    x_ofs = (get_w(par) - lv_obj_get_width(lobj)) / 2;
    y_ofs = (get_h(par) - lv_obj_get_height(lobj)) / 2;

    set_gobj_pos(lobj, x_ofs, y_ofs);
}

void set_gobj_align(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                      int32_t x_ofs_px, int32_t y_ofs_px)
{
    gobj_t *gobj = NULL;
    LV_ASSERT_NULL(lobj);

    gobj = get_gobj(lobj);
    LV_ASSERT_NULL(gobj);
    gobj->align.value = align;
    gobj->align.base = base;
    gobj->align.x = x_ofs_px;
    gobj->align.y = y_ofs_px;
    gobj->align.scale_x = DIS_SCALE;
    gobj->align.scale_y = DIS_SCALE;

    apply_gobj_align(lobj);
}

void set_gobj_align_scale_x(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                            int32_t x_ofs_pct, int32_t y_ofs_px)
{
    gobj_t *gobj = NULL;

    LV_ASSERT_NULL(lobj);
    gobj = get_gobj(lobj);
    LV_ASSERT_NULL(gobj);

    gobj->align.value = align;
    gobj->align.base = base;
    gobj->align.x = x_ofs_pct;
    gobj->align.y = y_ofs_px;
    gobj->align.scale_x = ENA_SCALE;
    gobj->align.scale_y = DIS_SCALE;

    apply_gobj_align(lobj);
}

void set_gobj_align_scale_y(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                            int32_t x_ofs_px, int32_t y_ofs_pct)
{
    gobj_t *gobj = NULL;

    LV_ASSERT_NULL(lobj);
    gobj = get_gobj(lobj);
    LV_ASSERT_NULL(gobj);

    gobj->align.value = align;
    gobj->align.base = base;
    gobj->align.x = x_ofs_px;
    gobj->align.y = y_ofs_pct;
    gobj->align.scale_x = DIS_SCALE;
    gobj->align.scale_y = ENA_SCALE;

    apply_gobj_align(lobj);
}

void set_gobj_align_scale(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                             int32_t x_ofs_pct, int32_t y_ofs_pct)
{
    gobj_t *gobj = NULL;

    LV_ASSERT_NULL(lobj);
    gobj = get_gobj(lobj);
    LV_ASSERT_NULL(gobj);

    gobj->align.value = align;
    gobj->align.base = base;
    gobj->align.x = x_ofs_pct;
    gobj->align.y = y_ofs_pct;
    gobj->align.scale_x = ENA_SCALE;
    gobj->align.scale_y = ENA_SCALE;

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

    if (gobj->align.scale_x == ENA_SCALE)
        x_ofs_px = pct_to_px(get_par_w(lobj), gobj->align.x);
    else
        x_ofs_px = gobj->align.x;

    if (gobj->align.scale_y == ENA_SCALE)
        y_ofs_px = pct_to_px(get_par_h(lobj), gobj->align.y);
    else
        y_ofs_px = gobj->align.y;

    lv_obj_align_to(lobj, gobj->align.base, gobj->align.value, x_ofs_px, y_ofs_px);
}

/*
 * Rotate object alignment by 90° clockwise.
 * Uses lookup table instead of switch-case for O(1) performance.
 */
int32_t rotate_gobj_alignment_90(gobj_t *gobj)
{
    lv_obj_t *lobj;
    int8_t align;

    lobj = gobj ? get_lobj(gobj) : NULL;
    if (!lobj)
        return -EINVAL;

    static const int8_t align_rot90_map[] = {
        [LV_ALIGN_TOP_LEFT]          = LV_ALIGN_TOP_RIGHT,
        [LV_ALIGN_TOP_MID]           = LV_ALIGN_RIGHT_MID,
        [LV_ALIGN_TOP_RIGHT]         = LV_ALIGN_BOTTOM_RIGHT,
        [LV_ALIGN_LEFT_MID]          = LV_ALIGN_TOP_MID,
        [LV_ALIGN_CENTER]            = LV_ALIGN_CENTER,
        [LV_ALIGN_RIGHT_MID]         = LV_ALIGN_BOTTOM_MID,
        [LV_ALIGN_BOTTOM_LEFT]       = LV_ALIGN_TOP_LEFT,
        [LV_ALIGN_BOTTOM_MID]        = LV_ALIGN_LEFT_MID,
        [LV_ALIGN_BOTTOM_RIGHT]      = LV_ALIGN_BOTTOM_LEFT,

        /* Outside aligns rotation */
        [LV_ALIGN_OUT_TOP_LEFT]      = LV_ALIGN_OUT_RIGHT_TOP,
        [LV_ALIGN_OUT_TOP_MID]       = LV_ALIGN_OUT_RIGHT_MID,
        [LV_ALIGN_OUT_TOP_RIGHT]     = LV_ALIGN_OUT_RIGHT_BOTTOM,
        [LV_ALIGN_OUT_BOTTOM_LEFT]   = LV_ALIGN_OUT_LEFT_TOP,
        [LV_ALIGN_OUT_BOTTOM_MID]    = LV_ALIGN_OUT_LEFT_MID,
        [LV_ALIGN_OUT_BOTTOM_RIGHT]  = LV_ALIGN_OUT_LEFT_BOTTOM,
        [LV_ALIGN_OUT_LEFT_TOP]      = LV_ALIGN_OUT_TOP_RIGHT,
        [LV_ALIGN_OUT_LEFT_MID]      = LV_ALIGN_OUT_TOP_MID,
        [LV_ALIGN_OUT_LEFT_BOTTOM]   = LV_ALIGN_OUT_TOP_LEFT,
        [LV_ALIGN_OUT_RIGHT_TOP]     = LV_ALIGN_OUT_BOTTOM_RIGHT,
        [LV_ALIGN_OUT_RIGHT_MID]     = LV_ALIGN_OUT_BOTTOM_MID,
        [LV_ALIGN_OUT_RIGHT_BOTTOM]  = LV_ALIGN_OUT_BOTTOM_LEFT,
    };

    align = gobj->align.value;

    if (align < 0 || align >= 21 || align_rot90_map[align] == 0) {
        LOG_ERROR("Invalid alignment (%d) for object %s", align, \
                  get_obj_name(lobj));
        return -EIO;
    }

    gobj->align.value = align_rot90_map[align];

    return 0;
}

void rotate_alignment_offset_90(gobj_t *gobj)
{
    int32_t tmp_x_aln;
    int32_t tmp_scale_x;

    tmp_x_aln = gobj->align.x;
    tmp_scale_x = gobj->align.scale_x;

    gobj->align.x = -(gobj->align.y);
    gobj->align.scale_x = gobj->align.scale_y;

    gobj->align.y = tmp_x_aln;
    gobj->align.scale_y = tmp_scale_x;
}

