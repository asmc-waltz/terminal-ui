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
static int8_t calc_rotation_turn(lv_obj_t *lobj)
{
    int8_t cur_rot;
    int8_t scr_rot;
    int8_t rot_cnt;
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    cur_rot = meta->data.rotation;
    scr_rot = get_scr_rotation();

    if (cur_rot < ROTATION_0 || cur_rot > ROTATION_270 ||
        scr_rot < ROTATION_0 || scr_rot > ROTATION_270)
        return -EINVAL;

    /* Number of 90-degree turns to reach screen rotation from current */
    rot_cnt = (scr_rot - cur_rot + 4) % 4;

    return rot_cnt;
}

static int32_t rotate_alignment_meta(lv_obj_t *lobj)
{
    int8_t cur_rot;
    int8_t scr_rot;
    int8_t rot_cnt;

    if (!lobj)
        return -EINVAL;

    rot_cnt = calc_rotation_turn(lobj);
    if (rot_cnt <= 0)
        return 0;

    /* Perform rotation by 90° steps */
    for (int8_t i = 0; i < rot_cnt; i++) {
        rotate_alignment_meta_90(lobj);
        rotate_alignment_offset_meta_90(lobj);
    }

    return 0;
}

static int32_t rotate_size_meta(lv_obj_t *lobj)
{
    int32_t scr_rot, cur_rot;
    int32_t rot_cnt;

    if (!lobj)
        return -EINVAL;

    rot_cnt = calc_rotation_turn(lobj);
    if (rot_cnt <= 0)
        return 0;

    /* Perform rotation by 90° steps */
    for (int8_t i = 0; i < rot_cnt; i++) {
        rotate_size_meta_90(lobj);
    }

    return 0;
}

static int32_t rotate_generic_style_meta(lv_obj_t *lobj)
{
    int32_t ret, rot_cnt;
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    rot_cnt = calc_rotation_turn(lobj);
    if (rot_cnt <= 0)
        return 0;

    /* Perform rotation by 90° steps */
    for (int8_t i = 0; i < rot_cnt; i++) {
        ret = rotate_border_side_meta_90(lobj);
        if (ret)
            return ret;

        ret = rotate_padding_meta_90(lobj);
        if (ret)
            return ret;
    }

    ret = apply_border_side_meta(lobj);
    if (ret)
        return ret;

    ret = apply_padding_meta(lobj);
    if (ret)
        return ret;

    ret = apply_row_column_padding_meta(lobj);
    if (ret)
        return ret;

    return 0;
}

/*
 * Common adjustment handler used after layout rotation.
 * Handles align, size, and positional recalculation for rotated objects.
 */
static inline int32_t rotate_common_post_adjust(lv_obj_t *lobj)
{
    int32_t par_w, par_h;
    int32_t ret;
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    /* Recalculate alignment values if needed */
    if (meta->align.value != LV_ALIGN_DEFAULT) {
        ret = rotate_alignment_meta(lobj);
        if (ret)
            return -EINVAL;
    }

    /*
     * For each object, when rotation occurs, its size must be recalculated.
     * Since the root coordinate does not change, the width and height
     * will be adjusted according to the logical rotation.
     */
    ret = rotate_size_meta(lobj);
    if (ret)
        return -EINVAL;

    apply_size_meta(lobj);

    /*
     * For an object placed inside a parent, its new center point must be
     * recalculated based on the logical rotation. Using this new center,
     * the width and height can then be updated accordingly.
     */
    if (meta->align.value == LV_ALIGN_DEFAULT) {
        par_w = get_par_w(lobj);
        par_h = get_par_h(lobj);

        ret = get_center(lobj, par_w, par_h);
        if (ret)
            return -EINVAL;

        lv_obj_set_pos(lobj, meta->align.mid_x - (get_w(lobj) / 2), \
                       meta->align.mid_y - (get_h(lobj) / 2));
    } else {
        apply_align_meta(lobj);
    }

    return 0;
}

/*
 * Rotate an object (non-layout cell) such as keyboard or standalone widget.
 * The size and alignment logic depends on its ratio and orientation mode.
 */
static int32_t rotate_generic_geometry_meta(lv_obj_t *lobj)
{
    int32_t ret;

    if (!lobj)
        return -EINVAL;

    /* Ignore size and align adjustment for base (non-rotated) object */
    if (get_type(lobj) == OBJ_BASE)
        return 0;

    /*
     * For objects that take the role of a layout container (such as flex or grid)
     * but are themselves a cell of a parent layout, their size and alignment depend
     * entirely on the parent and cell configurations. Therefore, manual
     * repositioning mechanisms can be safely skipped.
     */
    if (get_cell_type(lobj) == OBJ_GRID_CELL || \
        get_cell_type(lobj) == OBJ_FLEX_CELL)
        return 0;

    /*
     * For base objects, rotation only affects geometric and alignment data.
     * There is no layout type to verify, so skip type check.
     */
    ret = rotate_common_post_adjust(lobj);
    if (ret)
        return ret;

    return 0;
}

static inline int32_t rotate_visual_object(lv_obj_t *lobj)
{
    int32_t ret;
    int32_t scr_rot = get_scr_rotation();
    int32_t rot_val = 0;
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    ret = rotate_size_meta(lobj);
    if (ret) {
        return -EINVAL;
    }

    ret = get_center(lobj, get_par_w(lobj), get_par_h(lobj));
    if (ret) {
        return -EINVAL;
    }

    if (scr_rot == ROTATION_0) {
        rot_val = 0;
        lv_obj_set_style_transform_rotation(lobj, rot_val, 0);
        lv_obj_set_pos(lobj, meta->align.mid_x - (get_w(lobj) / 2), \
                       meta->align.mid_y - (get_h(lobj) / 2));
    } else if (scr_rot == ROTATION_90) {
        rot_val = 900;
        lv_obj_set_style_transform_rotation(lobj, rot_val, 0);
        lv_obj_set_pos(lobj, meta->align.mid_x + (get_w(lobj) / 2), \
                       meta->align.mid_y - (get_h(lobj) / 2));
    } else if (scr_rot == ROTATION_180) {
        rot_val = 1800;
        lv_obj_set_style_transform_rotation(lobj, rot_val, 0);
        lv_obj_set_pos(lobj, meta->align.mid_x + (get_w(lobj) / 2), \
                       meta->align.mid_y + (get_h(lobj) / 2));
    } else if (scr_rot == ROTATION_270) {
        rot_val = 2700;
        lv_obj_set_style_transform_rotation(lobj, rot_val, 0);
        lv_obj_set_pos(lobj, meta->align.mid_x - (get_w(lobj) / 2), \
                       meta->align.mid_y + (get_h(lobj) / 2));
    }

    return 0;
}

/*
 * Rotate grid layout object based on current and target screen rotation.
 * Handles pre/post redraw, layout config reapply, and post-adjust steps.
 */
static int32_t rotate_grid_layout_meta(lv_obj_t *lobj)
{
    int32_t ret;
    int8_t rot_cnt;

    if (!lobj)
        return -EINVAL;

    rot_cnt = calc_rotation_turn(lobj);
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

    return 0;
}

static int32_t rotate_grid_cell_meta(lv_obj_t *lobj)
{
    int32_t ret;
    int8_t rot_cnt;

    if (!lobj)
        return -EINVAL;

    rot_cnt = calc_rotation_turn(lobj);

    for (int8_t i = 0; i < rot_cnt; i++) {
        ret = rotate_grid_cell_pos_90(lobj);
        if (ret) {
            LOG_ERROR("Cell [%s] rotation failed, ret %d", \
                      get_name(lobj), ret);
            return -EIO;
        }
    }

    ret = apply_grid_cell_align_and_pos(lobj);
    if (ret) {
        LOG_ERROR("Cell [%s] apply config failed, ret %d", \
                  get_name(lobj), ret);
        return -EIO;
    }

    return 0;
}

/*
 * Rotate flex layout object based on current and target screen rotation.
 * Includes pre/post rotation callbacks, flow update, and geometry adjust.
 */
static int32_t rotate_flex_layout_meta(lv_obj_t *lobj)
{
    int32_t ret;
    int8_t rot_cnt;
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    rot_cnt = calc_rotation_turn(lobj);
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

    ret = rotate_flex_align_one(lobj);
    if (ret) {
        LOG_ERROR("Layout [%s] rotation align failed, ret %d", \
                  get_name(lobj), ret);
        return -EIO;
    }

    ret = apply_flex_layout_config(lobj);
    if (ret) {
        LOG_ERROR("Layout [%s] apply config failed, ret %d", \
                  get_name(lobj), ret);
        return -EIO;
    }

    return 0;
}

static int32_t rotate_flex_cell_meta(lv_obj_t *lobj)
{
    int32_t ret;
    int8_t rot_cnt;

    if (!lobj)
        return -EINVAL;

    rot_cnt = calc_rotation_turn(lobj);
    if (rot_cnt <= 0)
        return 0;

    /* Perform rotation by 90° steps */
    for (int8_t i = 0; i < rot_cnt; i++) {
        ret = rotate_flex_cell_90(lobj);
        if (ret) {
            LOG_ERROR("Cell [%s] rotation failed, ret %d", \
                      get_name(lobj), ret);
            return -EIO;
        }
    }

    ret = apply_flex_cell_config(lobj);
    if (ret) {
        LOG_ERROR("Cell [%s] apply config failed, ret %d", \
                  get_name(lobj), ret);
        return -EIO;
    }

    return 0;
}

static inline int32_t rotate_logical_object(lv_obj_t *lobj)
{
    int32_t ret;

    if (!lobj)
        return -EINVAL;

    switch (get_cell_type(lobj)) {
        case OBJ_GRID_CELL:
            ret = rotate_grid_cell_meta(lobj);
            break;
        case OBJ_FLEX_CELL:
            ret = rotate_flex_cell_meta(lobj);
            break;
        default:
            break;
    }

    if (ret) {
        LOG_ERROR("Object [%s] rotate cell metadata failed, ret %d", \
                  get_name(lobj), ret);
        return ret;
    }

    switch (get_layout_type(lobj)) {
        case OBJ_LAYOUT_GRID:
            ret = rotate_grid_layout_meta(lobj);
            break;
        case OBJ_LAYOUT_FLEX:
            ret = rotate_flex_layout_meta(lobj);
            break;
        default:
            break;
    }

    if (ret) {
        LOG_ERROR("Object [%s] rotate layout metadata failed, ret %d", \
                  get_name(lobj), ret);
        return ret;
    }

    ret = rotate_generic_style_meta(lobj);
    if (ret) {
        LOG_ERROR("Object [%s] rotate generic style metadata failed, ret %d", \
                  get_name(lobj), ret);
        return ret;
    }

    ret = rotate_generic_geometry_meta(lobj);
    if (ret) {
        LOG_ERROR("Object [%s] rotate basic metadata failed, ret %d", \
                  get_name(lobj), ret);
        return ret;
    }

    return 0;
}

static inline int32_t handle_object_transform(lv_obj_t *lobj)
{
    int32_t ret = 0;
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    /*
     * For some objects like the slider, the size and ratio are different
     * between horizontal and vertical modes. Therefore, we must redraw the
     * object to a compatible ratio before performing the component rotation.
     */
    if (meta->data.pre_rotate_cb) {
        ret = meta->data.pre_rotate_cb(lobj);
        if (ret) {
            LOG_ERROR("Failed to execute redraw callback for object %s", \
                      get_name(lobj));
            return ret;
        }
    }

    switch (get_type(lobj)) {
        case OBJ_BASE:
        case OBJ_BOX:
        case OBJ_BTN:
        case OBJ_SLIDER:
            ret = rotate_logical_object(lobj);
            break;

        case OBJ_LABEL:
        case OBJ_SWITCH:
        case OBJ_ICON:
        case OBJ_TEXTAREA:
            ret = rotate_visual_object(lobj);
            break;

        default:
            LOG_WARN("Unhandled object type %d, skipping transform", \
                     get_type(lobj));
            return -EINVAL;
    }

    if (ret) {
        LOG_ERROR("Object [%s] transform failed, ret %d", get_name(lobj), ret);
        return ret;
    }

    lv_obj_mark_layout_as_dirty(lobj);

    return 0;
}

static int32_t refresh_object(lv_obj_t *lobj)
{
    int32_t ret;
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;


    // NOTE: Refresh now applies beyond rotation
    // if (meta->data.rotation == scr_rot)
    //     return 0;

    // TODO: check obj type and update list flow, scale...
    // Text, icon, switch will be rotate
    // Frame, button, slider will be resize and relocation
    ret = handle_object_transform(lobj);
    if (ret) {
        LOG_ERROR("Failed to handle object refresh event, ret %d", ret);
        return ret;
    }

    meta->data.rotation = get_scr_rotation();

    return 0;
}

static int32_t refresh_object_child(lv_obj_t *lobj)
{
    int32_t ret;
    struct list_head *par_list;
    obj_meta_t *meta;
    obj_meta_t *child_meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    par_list = &meta->child;

    list_for_each_entry(child_meta, par_list, node) {
        ret = refresh_object_tree_layout(get_lobj(child_meta));
        if (ret < 0) {
            LOG_ERROR("List: Object %d (%s) rotation failed", \
                      child_meta->id, get_meta_name(child_meta));
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
int32_t refresh_object_tree_layout(lv_obj_t *lobj)
{
    int32_t ret;
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    ret = refresh_object(lobj);
    if (ret < 0) {
        LOG_ERROR("Object [%s] id %d rotation failed", \
                  get_name(lobj), meta->id);
        return ret;
    }

    ret = refresh_object_child(lobj);
    if (ret < 0) {
        LOG_ERROR("Object [%s] id %d: child rotation failed", \
                  get_name(lobj), meta->id);
        return ret;
    }

    if (get_layout_type(lobj) == OBJ_LAYOUT_FLEX) {
        ret = scroll_to_first_child(lobj);
        if (ret)
            LOG_WARN("Scroll [%s] to first child failed", get_name(lobj));
    }

    return 0;
}
