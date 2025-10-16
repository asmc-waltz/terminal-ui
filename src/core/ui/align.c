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
 *  - validates result before updating meta state (atomic update),
 *  - logs clearly.
 *
 * Returns:
 *  0        -> success (meta updated)
 *  -EINVAL  -> bad input
 *  -ERANGE  -> computed midpoint is out of new parent bounds
 */
int32_t get_center(lv_obj_t *lobj, uint32_t par_w, uint32_t par_h)
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
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta) {
        LOG_ERROR("null meta");
        return -EINVAL;
    }

    scr_rot = get_scr_rotation();
    old_rot = meta->data.rotation;

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
    old_pw = meta->align.par_w;
    old_ph = meta->align.par_h;
    L = meta->align.mid_x;
    T = meta->align.mid_y;
    R = old_pw - L;
    B = old_ph - T;

    LOG_TRACE("obj id=%d %s - old_rot=%d -> scr_rot=%d, "
              "old_pw=%d old_ph=%d, L=%d T=%d R=%d B=%d, "
              "new_par=(%d,%d)", meta->id, meta->name,
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

    /* Atomic update of meta position state */
    meta->align.mid_x = new_x_mid;
    meta->align.mid_y = new_y_mid;
    meta->align.par_w = par_w;
    meta->align.par_h = par_h;
    meta->data.rotation = scr_rot;

    LOG_TRACE("success new_mid=(%d,%d) new_par=(%d,%d) rot=%d",
              new_x_mid, new_y_mid, par_w, par_h, scr_rot);

    return 0;
}

void set_pos(lv_obj_t *lobj, int32_t x_ofs, int32_t y_ofs)
{
    obj_meta_t *meta = NULL;
    LV_ASSERT_NULL(lobj);

    lv_obj_set_pos(lobj, x_ofs, y_ofs);

    meta = get_meta(lobj);
    if (!meta->size.w)
        LOG_WARN("Cannot calculate the center x");
    if (!meta->size.h)
        LOG_WARN("Cannot calculate the center y");
    meta->align.mid_x = x_ofs + (meta->size.w / 2);
    meta->align.mid_y = y_ofs + (meta->size.h / 2);
}

void set_pos_center(lv_obj_t *lobj)
{
    obj_meta_t *meta = NULL;
    lv_obj_t *par;
    int32_t x_ofs, y_ofs;

    LV_ASSERT_NULL(lobj);

    // lv_obj_set_pos(lobj, x_ofs, y_ofs);
    par = lv_obj_get_parent(lobj);
    x_ofs = (get_w(par) - lv_obj_get_width(lobj)) / 2;
    y_ofs = (get_h(par) - lv_obj_get_height(lobj)) / 2;

    set_pos(lobj, x_ofs, y_ofs);
}

void set_align(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                      int32_t x_ofs_px, int32_t y_ofs_px)
{
    obj_meta_t *meta = NULL;
    LV_ASSERT_NULL(lobj);

    meta = get_meta(lobj);
    LV_ASSERT_NULL(meta);
    meta->align.value = align;
    meta->align.base = base;
    meta->align.x = x_ofs_px;
    meta->align.y = y_ofs_px;
    meta->align.scale_x = DIS_SCALE;
    meta->align.scale_y = DIS_SCALE;

    apply_align_meta(lobj);
}

void set_align_scale_x(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                            int32_t x_ofs_pct, int32_t y_ofs_px)
{
    obj_meta_t *meta = NULL;

    LV_ASSERT_NULL(lobj);
    meta = get_meta(lobj);
    LV_ASSERT_NULL(meta);

    meta->align.value = align;
    meta->align.base = base;
    meta->align.x = x_ofs_pct;
    meta->align.y = y_ofs_px;
    meta->align.scale_x = ENA_SCALE;
    meta->align.scale_y = DIS_SCALE;

    apply_align_meta(lobj);
}

void set_align_scale_y(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                            int32_t x_ofs_px, int32_t y_ofs_pct)
{
    obj_meta_t *meta = NULL;

    LV_ASSERT_NULL(lobj);
    meta = get_meta(lobj);
    LV_ASSERT_NULL(meta);

    meta->align.value = align;
    meta->align.base = base;
    meta->align.x = x_ofs_px;
    meta->align.y = y_ofs_pct;
    meta->align.scale_x = DIS_SCALE;
    meta->align.scale_y = ENA_SCALE;

    apply_align_meta(lobj);
}

void set_align_scale(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                             int32_t x_ofs_pct, int32_t y_ofs_pct)
{
    obj_meta_t *meta = NULL;

    LV_ASSERT_NULL(lobj);
    meta = get_meta(lobj);
    LV_ASSERT_NULL(meta);

    meta->align.value = align;
    meta->align.base = base;
    meta->align.x = x_ofs_pct;
    meta->align.y = y_ofs_pct;
    meta->align.scale_x = ENA_SCALE;
    meta->align.scale_y = ENA_SCALE;

    apply_align_meta(lobj);
}

void apply_align_meta(lv_obj_t *lobj)
{
    int32_t x_ofs_px;
    int32_t y_ofs_px;
    obj_meta_t *meta = NULL;

    LV_ASSERT_NULL(lobj);
    meta = get_meta(lobj);
    LV_ASSERT_NULL(meta);

    if (meta->align.scale_x == ENA_SCALE)
        x_ofs_px = pct_to_px(get_par_w(lobj), meta->align.x);
    else
        x_ofs_px = meta->align.x;

    if (meta->align.scale_y == ENA_SCALE)
        y_ofs_px = pct_to_px(get_par_h(lobj), meta->align.y);
    else
        y_ofs_px = meta->align.y;

    lv_obj_align_to(lobj, meta->align.base, meta->align.value, x_ofs_px, y_ofs_px);
}

/*
 * Rotate object alignment by 90° clockwise.
 * Uses lookup table instead of switch-case for O(1) performance.
 */
int32_t rotate_alignment_meta_90(lv_obj_t *lobj)
{
    int8_t align;
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
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

    align = meta->align.value;

    if (align < 0 || align >= 21 || align_rot90_map[align] == 0) {
        LOG_ERROR("Invalid alignment (%d) for object %s", align, \
                  get_name(lobj));
        return -EIO;
    }

    meta->align.value = align_rot90_map[align];

    return 0;
}

int32_t rotate_alignment_offset_meta_90(lv_obj_t *lobj)
{
    int32_t tmp_x_aln;
    int32_t tmp_scale_x;
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    tmp_x_aln = meta->align.x;
    tmp_scale_x = meta->align.scale_x;

    meta->align.x = -(meta->align.y);
    meta->align.scale_x = meta->align.scale_y;

    meta->align.y = tmp_x_aln;
    meta->align.scale_y = tmp_scale_x;

    return 0;
}

