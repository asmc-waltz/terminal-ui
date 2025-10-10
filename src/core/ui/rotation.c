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
#include "ui/flex.h"
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

static int32_t rotate_gobj_alignment(gobj_t *gobj)
{
    int8_t cur_rot;
    int8_t scr_rot;
    int8_t rot_cnt;

    if (!gobj)
        return -EINVAL;

    rot_cnt = calc_rotation_turn(gobj);
    if (rot_cnt <= 0)
        return 0;

    /* Perform rotation by 90° steps */
    for (int8_t i = 0; i < rot_cnt; i++) {
        rotate_gobj_alignment_90(gobj);
        rotate_alignment_offset_90(gobj);
    }

    return 0;
}

int32_t rotate_gobj_size(gobj_t *gobj)
{
    int32_t scr_rot, cur_rot;
    int32_t rot_cnt;

    if (!gobj) {
        return -EINVAL;
    }

    rot_cnt = calc_rotation_turn(gobj);
    if (rot_cnt <= 0)
        return 0;

    /* Perform rotation by 90° steps */
    for (int8_t i = 0; i < rot_cnt; i++) {
        rotate_gobj_size_90(gobj);
    }

    return 0;
}

/*
 * Common adjustment handler used after layout rotation.
 * Handles align, size, and positional recalculation for rotated objects.
 */
static int32_t rotate_common_post_adjust(gobj_t *gobj)
{
    int32_t par_w, par_h;
    lv_obj_t *lobj;
    int32_t ret;

    lobj = gobj ? get_lobj(gobj) : NULL;
    if (!lobj)
        return -EINVAL;

    /* Ignore size and align adjustment for base (non-rotated) object */
    if (gobj->data.obj_type == OBJ_BASE)
        return 0;

    /*
     * For objects that take the role of a layout container (such as flex or grid)
     * but are themselves a cell of a parent layout, their size and alignment depend
     * entirely on the parent and cell configurations. Therefore, manual
     * repositioning mechanisms can be safely skipped.
     */
    if (gobj->data.cell_type == OBJ_GRID_CELL || \
        gobj->data.cell_type == OBJ_FLEX_CELL)
        return 0;

    /* Recalculate alignment values if needed */
    if (gobj->align.value != LV_ALIGN_DEFAULT) {
        ret = rotate_gobj_alignment(gobj);
        if (ret)
            return -EINVAL;
    }

    /*
     * For each object, when rotation occurs, its size must be recalculated.
     * Since the root coordinate does not change, the width and height
     * will be adjusted according to the logical rotation.
     */
    ret = rotate_gobj_size(gobj);
    if (ret)
        return -EINVAL;

    apply_gobj_size(lobj);

    /*
     * For an object placed inside a parent, its new center point must be
     * recalculated based on the logical rotation. Using this new center,
     * the width and height can then be updated accordingly.
     */
    if (gobj->align.value == LV_ALIGN_DEFAULT) {
        par_w = get_par_w(lobj);
        par_h = get_par_h(lobj);

        ret = gobj_get_center(gobj, par_w, par_h);
        if (ret)
            return -EINVAL;

        lv_obj_set_pos(lobj, gobj->align.mid_x - (get_w(lobj) / 2), \
                       gobj->align.mid_y - (get_h(lobj) / 2));
    } else {
        apply_gobj_align(lobj);
    }

    return 0;
}

/*
 * Rotate a base object (non-layout) such as keyboard or standalone widget.
 * The size and alignment logic depends on its ratio and orientation mode.
 */
static int32_t rotate_base_gobj(gobj_t *gobj)
{
    lv_obj_t *lobj;
    int32_t ret;

    lobj = gobj ? get_lobj(gobj) : NULL;
    if (!lobj)
        return -EINVAL;

    /*
     * For some objects like the keyboard, the size and ratio are different
     * between horizontal and vertical modes. Therefore, we must redraw the
     * object to a compatible ratio before performing the component rotation.
     */
    if (gobj->data.prerotate_cb)
        gobj->data.prerotate_cb(lobj);

    /*
     * For base objects, rotation only affects geometric and alignment data.
     * There is no layout type to verify, so skip type check.
     */
    ret = rotate_common_post_adjust(gobj);
    if (ret)
        return ret;

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

    ret = rotate_gobj_size(gobj);
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

/*
 * Rotate grid layout object based on current and target screen rotation.
 * Handles pre/post redraw, layout config reapply, and post-adjust steps.
 */
static int32_t rotate_grid_layout_gobj(gobj_t *gobj)
{
    int32_t ret;
    int8_t rot_cnt;
    lv_obj_t *lobj;

    lobj = gobj ? get_lobj(gobj) : NULL;
    if (!lobj)
        return -EINVAL;

    if (gobj->data.prerotate_cb) {
        ret = gobj->data.prerotate_cb(lobj);
        if (ret) {
            LOG_ERROR("Failed to execute redraw callback for object %s", \
                      gobj->name);
            return ret;
        }
    }

    rot_cnt = calc_rotation_turn(gobj);
    if (rot_cnt <= 0)
        return 0;

    /* Perform rotation by 90° steps */
    for (int8_t i = 0; i < rot_cnt; i++) {
        ret = rotate_grid_layout_90(lobj);
        if (ret) {
            LOG_ERROR("Layout [%s] rotation failed, ret %d", \
                      get_name(lobj), ret);
            return ret;
        }
    }

    ret = apply_grid_layout_config(lobj);
    if (ret) {
        LOG_ERROR("Layout [%s] apply config failed, ret %d", \
                  get_name(lobj), ret);
        return ret;
    }

    return rotate_common_post_adjust(gobj);
}

static int32_t rotate_grid_cell_gobj(gobj_t *gobj)
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
            LOG_ERROR("Cell [%s] rotation failed, ret %d", get_name(lobj), ret);
            return -EIO;
        }
    }

    ret = apply_grid_cell_align_and_pos(lobj);
    if (ret) {
        LOG_ERROR("Cell [%s] apply config failed, ret %d", get_name(lobj), ret);
        return -EIO;
    }

    lv_obj_mark_layout_as_dirty(lv_obj_get_parent(lobj));

    return 0;
}

/*
 * Rotate flex layout object based on current and target screen rotation.
 * Includes pre/post rotation callbacks, flow update, and geometry adjust.
 */
static int32_t rotate_flex_layout_gobj(gobj_t *gobj)
{
    int32_t ret;
    int8_t rot_cnt;
    lv_obj_t *lobj;

    lobj = gobj ? get_lobj(gobj) : NULL;
    if (!lobj)
        return -EINVAL;

    if (gobj->data.prerotate_cb)
        gobj->data.prerotate_cb(lobj);

    rot_cnt = calc_rotation_turn(gobj);
    if (rot_cnt <= 0)
        return 0;

    /* Perform rotation by 90° steps */
    for (int8_t i = 0; i < rot_cnt; i++) {
        ret = rotate_flex_layout_90(lobj);
        if (ret) {
            LOG_ERROR("Layout [%s] rotation failed, ret %d", \
                      get_name(lobj), ret);
            return -EIO;
        }
    }

    ret = apply_flex_layout_flow(lobj);
    if (ret) {
        LOG_ERROR("Layout [%s] apply config failed, ret %d", \
                  get_name(lobj), ret);
        return -EIO;
    }

    ret = rotate_common_post_adjust(gobj);
    if (ret) {
        LOG_ERROR("Failed to rotate layout object position");
        return -EIO;
    }

    lv_obj_mark_layout_as_dirty(lobj);

    return 0;
}

static int32_t rotate_flex_cell_gobj(gobj_t *gobj)
{
    int32_t ret;
    int8_t rot_cnt;
    lv_obj_t *lobj;

    lobj = gobj ? get_lobj(gobj) : NULL;
    if (!lobj)
        return -EINVAL;

    rot_cnt = calc_rotation_turn(gobj);
    if (rot_cnt <= 0)
        return 0;

    /* Perform rotation by 90° steps */
    for (int8_t i = 0; i < rot_cnt; i++) {
        ret = rotate_flex_cell_90(lobj);
        if (ret) {
            LOG_ERROR("Cell [%s] rotation failed, ret %d", get_name(lobj), ret);
            return -EIO;
        }
    }

    ret = apply_flex_cell_config(lobj);
    if (ret) {
        LOG_ERROR("Cell [%s] apply config failed, ret %d", get_name(lobj), ret);
        return -EIO;
    }

    lv_obj_mark_layout_as_dirty(lobj);

    return 0;
}

static inline int32_t handle_gobj_layout_rotation(gobj_t *gobj)
{
    int32_t ret;

    if (!gobj)
        return -EINVAL;

    switch (gobj->data.cell_type) {
    case OBJ_GRID_CELL:
        ret = rotate_grid_cell_gobj(gobj);
        break;
    case OBJ_FLEX_CELL:
        ret = rotate_flex_cell_gobj(gobj);
        break;
    default:
        break;
    }

    if (ret) {
        LOG_WARN("Unable to rotate cell object, ret %d", ret);
        return ret;
    }

    switch (gobj->data.layout_type) {
    case OBJ_LAYOUT_GRID:
        return rotate_grid_layout_gobj(gobj);
    case OBJ_LAYOUT_FLEX:
        return rotate_flex_layout_gobj(gobj);
    default:
        break;
    }

    return rotate_base_gobj(gobj);
}

static inline int32_t gobj_handle_transform(gobj_t *gobj)
{
    if (!gobj)
        return -EINVAL;

    switch (gobj->data.obj_type) {
        case OBJ_BASE:
        case OBJ_BOX:
        case OBJ_BTN:
        case OBJ_SLIDER:
            return handle_gobj_layout_rotation(gobj);

        case OBJ_LABEL:
        case OBJ_SWITCH:
        case OBJ_ICON:
        case OBJ_TEXTAREA:
            return rotate_transform_gobj(gobj);

        default:
            LOG_WARN("Unhandled object type %d, skipping transform", \
                     gobj->data.obj_type);
            return -EINVAL;
    }
}

static int32_t gobj_refresh(gobj_t *gobj)
{
    int32_t ret;
    lv_obj_t *lobj;

    lobj = gobj ? get_lobj(gobj) : NULL;
    if (!lobj)
        return -EINVAL;


    // NOTE: Refresh now applies beyond rotation
    // if (gobj->data.rotation == scr_rot)
    //     return 0;

    // TODO: check obj type and update list flow, scale...
    // Text, icon, switch will be rotate
    // Frame, button, slider will be resize and relocation
    ret = gobj_handle_transform(gobj);
    if (ret) {
        LOG_ERROR("Failed to handle object refresh event, ret %d", ret);
        return ret;
    }

    gobj->data.rotation = get_scr_rotation();

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
