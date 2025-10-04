/**
 * @file rotation.c
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
#include "ui/grid.h"
#include "main.h"

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
static int8_t calc_rotation_turn(gobj_t *gobj)
{
    int8_t cur_rot;
    int8_t scr_rot;
    int8_t rot_cnt;

    if (!gobj)
        return -EINVAL;

    cur_rot = gobj->data.rotation;
    scr_rot = get_scr_rotation();

    if (cur_rot < ROTATION_0 || cur_rot > ROTATION_270 ||
        scr_rot < ROTATION_0 || scr_rot > ROTATION_270)
        return -EINVAL;

    /* Number of 90-degree turns to reach screen rotation from current */
    rot_cnt = (scr_rot - cur_rot + 4) % 4;

    return rot_cnt;
}

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
static int32_t gobj_get_center(gobj_t *gobj, uint32_t par_w, uint32_t par_h)
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

static void gobj_update_alignment_rot90(gobj_t *gobj)
{
    int8_t align = LV_ALIGN_DEFAULT;

    switch (gobj->align.value) {
        case LV_ALIGN_TOP_LEFT:
            align = LV_ALIGN_TOP_RIGHT;
            break;
        case LV_ALIGN_TOP_MID:
            align = LV_ALIGN_RIGHT_MID;
            break;
        case LV_ALIGN_TOP_RIGHT:
            align = LV_ALIGN_BOTTOM_RIGHT;
            break;
        case LV_ALIGN_LEFT_MID:
            align = LV_ALIGN_TOP_MID;
            break;
        case LV_ALIGN_CENTER:
            align = LV_ALIGN_CENTER;
            break;
        case LV_ALIGN_RIGHT_MID:
            align = LV_ALIGN_BOTTOM_MID;
            break;
        case LV_ALIGN_BOTTOM_LEFT:
            align = LV_ALIGN_TOP_LEFT;
            break;
        case LV_ALIGN_BOTTOM_MID:
            align = LV_ALIGN_LEFT_MID;
            break;
        case LV_ALIGN_BOTTOM_RIGHT:
            align = LV_ALIGN_BOTTOM_LEFT;
            break;

        /* Outside aligns on TOP side -> RIGHT side */
        case LV_ALIGN_OUT_TOP_LEFT:
            align = LV_ALIGN_OUT_RIGHT_TOP;
            break;
        case LV_ALIGN_OUT_TOP_MID:
            align = LV_ALIGN_OUT_RIGHT_MID;
            break;
        case LV_ALIGN_OUT_TOP_RIGHT:
            align = LV_ALIGN_OUT_RIGHT_BOTTOM;
            break;
        /* Outside aligns on BOTTOM side -> LEFT side */
        case LV_ALIGN_OUT_BOTTOM_LEFT:
            align = LV_ALIGN_OUT_LEFT_TOP;
            break;
        case LV_ALIGN_OUT_BOTTOM_MID:
            align = LV_ALIGN_OUT_LEFT_MID;
            break;
        case LV_ALIGN_OUT_BOTTOM_RIGHT:
            align = LV_ALIGN_OUT_LEFT_BOTTOM;
            break;
        /* Outside aligns on LEFT side -> TOP side */
        case LV_ALIGN_OUT_LEFT_TOP:
            align = LV_ALIGN_OUT_TOP_RIGHT;
            break;
        case LV_ALIGN_OUT_LEFT_MID:
            align = LV_ALIGN_OUT_TOP_MID;
            break;
        case LV_ALIGN_OUT_LEFT_BOTTOM:
            align = LV_ALIGN_OUT_TOP_LEFT;
            break;
        /* Outside aligns on RIGHT side -> BOTTOM side */
        case LV_ALIGN_OUT_RIGHT_TOP:
            align = LV_ALIGN_OUT_BOTTOM_RIGHT;
            break;
        case LV_ALIGN_OUT_RIGHT_MID:
            align = LV_ALIGN_OUT_BOTTOM_MID;
            break;
        case LV_ALIGN_OUT_RIGHT_BOTTOM:
            align = LV_ALIGN_OUT_BOTTOM_LEFT;
            break;
        default:
            LOG_ERROR("The current object alignment is invalid");
            return;
    }

    gobj->align.value = align;
}

static void gobj_swap_offset_rot90(gobj_t *gobj)
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

static int32_t g_obj_rot_calc_align(gobj_t *gobj)
{
    int8_t cur_rot;
    int8_t scr_rot;
    int8_t rot_cnt;

    if (!gobj)
        return -EINVAL;

    cur_rot = gobj->data.rotation;
    scr_rot = get_scr_rotation();

    if (cur_rot < ROTATION_0 || cur_rot > ROTATION_270 ||
        scr_rot < ROTATION_0 || scr_rot > ROTATION_270)
        return -EINVAL;

    rot_cnt = (scr_rot - cur_rot + 4) % 4;

    for (int8_t i = 0; i < rot_cnt; i++) {
        gobj_update_alignment_rot90(gobj);
        gobj_swap_offset_rot90(gobj);
    }

    return 0;
}

static int32_t rotate_base_gobj(gobj_t *gobj)
{
    int32_t ret;
    lv_obj_t *lobj;

    lobj = get_lobj(gobj);
    if (!lobj)
        return -EINVAL;

    /*
     * For some objects like the keyboard, the size and ratio are different
     * between horizontal and vertical modes. Therefore, we must redraw the
     * object to a compatible ratio before performing the component rotation.
     */
    if (gobj->data.pre_rot_redraw_cb) {
        gobj->data.pre_rot_redraw_cb(lobj);
    }

    // The size and scale calculation depends on alignment values,
    // so we must process these first.
    if (gobj->align.value != LV_ALIGN_DEFAULT) {
        ret = g_obj_rot_calc_align(gobj);
        if (ret) {
            return -EINVAL;
        }
    }
    /*
     * For each object, when rotation occurs, its size must be recalculated.
     * Since the root coordinate does not change, the width and height
     * will be adjusted according to the logical rotation.
     */
    ret = calc_gobj_rotated_size(gobj);
    if (ret) {
        return -EINVAL;
    }


    if (gobj->data.post_rot_resize_adjust_cb) {
        gobj->data.post_rot_resize_adjust_cb(lobj);
    } else {
        apply_gobj_size(lobj);
    }

    /*
     * For an object placed inside a parent, its new center point must be
     * recalculated based on the logical rotation. Using this new center,
     * the width and height can then be updated accordingly.
     */
    if (gobj->align.value == LV_ALIGN_DEFAULT) {

        ret = gobj_get_center(gobj, get_par_w(lobj), get_par_h(lobj));
        if (ret) {
            return -EINVAL;
        }
        lv_obj_set_pos(lobj, gobj->align.mid_x - (get_w(lobj) / 2), \
                       gobj->align.mid_y - (get_h(lobj) / 2));
    } else {
        apply_gobj_align(lobj);
    }

    return 0;
}

static int32_t rotate_transform_gobj(gobj_t *gobj)
{
    int32_t ret;
    int32_t scr_rot = get_scr_rotation();
    int32_t rot_val = 0;
    lv_obj_t *lobj;

    lobj = get_lobj(gobj);
    if (!lobj)
        return -EINVAL;

    ret = calc_gobj_rotated_size(gobj);
    if (ret) {
        return -EINVAL;
    }

    ret = gobj_get_center(gobj, get_par_w(lobj), get_par_h(lobj));
    if (ret) {
        return -EINVAL;
    }

    if (scr_rot == ROTATION_0) {
        rot_val = 0;
        lv_obj_set_style_transform_rotation(lobj, rot_val, 0);
        lv_obj_set_pos(lobj, gobj->align.mid_x - (get_w(lobj) / 2), \
                       gobj->align.mid_y - (get_h(lobj) / 2));
    } else if (scr_rot == ROTATION_90) {
        rot_val = 900;
        lv_obj_set_style_transform_rotation(lobj, rot_val, 0);
        lv_obj_set_pos(lobj, gobj->align.mid_x + (get_w(lobj) / 2), \
                       gobj->align.mid_y - (get_h(lobj) / 2));
    } else if (scr_rot == ROTATION_180) {
        rot_val = 1800;
        lv_obj_set_style_transform_rotation(lobj, rot_val, 0);
        lv_obj_set_pos(lobj, gobj->align.mid_x + (get_w(lobj) / 2), \
                       gobj->align.mid_y + (get_h(lobj) / 2));
    } else if (scr_rot == ROTATION_270) {
        rot_val = 2700;
        lv_obj_set_style_transform_rotation(lobj, rot_val, 0);
        lv_obj_set_pos(lobj, gobj->align.mid_x - (get_w(lobj) / 2), \
                       gobj->align.mid_y + (get_h(lobj) / 2));
    }

    return 0;
}

static int32_t rotate_layout_gobj(gobj_t *gobj)
{
    int32_t ret;
    int8_t rot_cnt;
    lv_obj_t *lobj;

    lobj = gobj ? get_lobj(gobj) : NULL;
    if (!lobj)
        return -EINVAL;

    rot_cnt = calc_rotation_turn(gobj);

    for (int8_t i = 0; i < rot_cnt; i++) {
        ret = rotate_grid_layout_90(lobj);
        if (ret) {
            LOG_ERROR("Failed to rotate layout object data");
            return -EIO;
        }
    }

    ret = apply_grid_layout_config(lobj);
    if (ret) {
        LOG_ERROR("Failed to apply new layout object data");
        return -EIO;
    }

    return 0;
}

static int32_t rotate_cell_gobj(gobj_t *gobj)
{
    int32_t ret;
    int8_t rot_cnt;
    lv_obj_t *lobj;

    lobj = gobj ? get_lobj(gobj) : NULL;
    if (!lobj)
        return -EINVAL;

    rot_cnt = calc_rotation_turn(gobj);

    for (int8_t i = 0; i < rot_cnt; i++) {
        ret = rotate_grid_cell_pos_90(lobj);
        if (ret) {
            LOG_ERROR("Failed to rotate cell object data");
            return -EIO;
        }
    }

    ret = apply_grid_cell_align_and_pos(lobj);
    if (ret) {
        LOG_ERROR("Failed to apply new cell object data");
        return -EIO;
    }

    return 0;
}

static int32_t gobj_refresh(gobj_t *gobj)
{
    int32_t ret;
    lv_obj_t *lobj;
    int32_t scr_rot = get_scr_rotation();

    lobj = get_lobj(gobj);
    if (!lobj)
        return -EINVAL;

    if (!gobj) {
        LOG_ERROR("Invalid g object");
        return -EINVAL;
    }

    // NOTE: Refresh now applies beyond rotation
    // if (gobj->data.rotation == scr_rot) {
    //     return 0;
    // }

    // TODO: check obj type and update list flow, scale...
    // Text, icon, switch will be rotate
    // Frame, button, slider will be resize and relocation
    switch (gobj->data.obj_type) {
        case OBJ_BOX:
        case OBJ_BTN:
        case OBJ_SLIDER:
            ret = rotate_base_gobj(gobj);
            break;
        case OBJ_LABEL:
        case OBJ_SWITCH:
        case OBJ_ICON:
        case OBJ_TEXTAREA:
            ret = rotate_transform_gobj(gobj);
            break;
        case OBJ_CELL:
            ret = rotate_cell_gobj(gobj);
            break;
        case OBJ_LAYOUT:
            ret = rotate_layout_gobj(gobj);
            break;
        default:
            LOG_WARN("Unknown G object type: %d", gobj->data.obj_type);
            break;
    }

    if (ret) {
        LOG_ERROR("Failed to handle object refresh event, ret %d", ret);
        return ret;
    }

    if (gobj->align.list) {
        ret = update_list_by_rot(gobj);
        if (ret)
            LOG_WARN("Unable to update list value, ret %d", ret);

        ret = set_list_scroll_dir(gobj);
        if (ret)
            LOG_WARN("Unable to update scroll configuration, ret %d", ret);
    }

    gobj->data.rotation = scr_rot;

    return 0;
}

static int32_t gobj_refresh_child(gobj_t *gobj)
{
    gobj_t *p_obj;
    int32_t ret;
    struct list_head *par_list;

    par_list = &gobj->child;

    list_for_each_entry(p_obj, par_list, node) {
        ret = gobj_refresh(p_obj);
        if (ret < 0) {
            LOG_ERROR("Object %d (%s) rotation failed", gobj->id, gobj->name);
            return ret;
        }

        ret = gobj_refresh_child(p_obj);
        if (ret < 0) {
            LOG_ERROR("Child object %d (%s) rotation failed", gobj->id, gobj->name);
            return ret;
        }

        if (p_obj->align.list) {
            update_list_align_by_rot(p_obj);
        }
    }

    return 0;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int32_t set_scr_rotation(int32_t rot_dir)
{
    ctx_t *ctx = get_ctx();
    if (ctx == NULL)
        return -EINVAL;

    ctx->scr.rotation = rot_dir;

    return 0;
}

int32_t get_scr_rotation()
{
    ctx_t *ctx = get_ctx();
    if (ctx == NULL)
        return -EINVAL;

    return ctx->scr.rotation;
}

/*
 * This function handles object layout updates including rotation, scaling,
 * and repositioning. Each object may define its own callbacks for move,
 * scale, or rotate, which will be invoked during the rotation job.
 * Although primarily used for rotation checks and updates, it may trigger
 * broader layout adjustments.
 */
int32_t refresh_obj_tree_layout(gobj_t *gobj)
{
    int32_t ret;

    ret = gobj_refresh(gobj);
    if (ret < 0)
        return ret;

    ret = gobj_refresh_child(gobj);
    if (ret < 0)
        return ret;

    return 0;
}
