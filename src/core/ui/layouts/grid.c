/**
 * @file grid.c
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
#include "ui/grid.h"

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
/* Generic array allocator/expander */
static void *alloc_or_extend_array(void *arr, size_t elem_size, int32_t count)
{
    void *new_arr;

    if (!arr) {
        /* First time: allocate count + 1 sentinel */
        new_arr = calloc(count + 1, elem_size);
    } else {
        /* Extend: add 1 new slot + sentinel */
        new_arr = realloc(arr, (count + 2) * elem_size);
    }

    return new_arr;
}

static int32_t alloc_or_extend_dsc(grid_desc_t *dsc)
{
    void *new_arr;

    new_arr = alloc_or_extend_array(dsc->cell_pct, sizeof(int32_t), dsc->size);
    if (!new_arr)
        return -ENOMEM;
    dsc->cell_pct = new_arr;

    return 0;
}

static void fill_new_slot(grid_desc_t *dsc, int32_t val)
{
    dsc->cell_pct[dsc->size]  = val;
    dsc->size++;
    /* Always keep sentinel */
    dsc->cell_pct[dsc->size] = LV_GRID_TEMPLATE_LAST;
}

static int32_t set_dsc_data(lv_obj_t *lobj, grid_desc_t *dsc, int32_t val)
{
    int32_t ret;

    if (!dsc)
        return -EINVAL;

    ret = alloc_or_extend_dsc(dsc);
    if (ret)
        return ret;

    fill_new_slot(dsc, val);

    return 0;
}

static void free_grid_desc(grid_desc_t *dsc)
{
    if (!dsc)
        return;

    if (dsc->cell_pct) {
        free(dsc->cell_pct);
        dsc->cell_pct = NULL;
    }

    dsc->size = 0;
    free(dsc);
}

static void on_size_changed_cb(lv_event_t *e)
{
    lv_obj_t *lobj = lv_event_get_target(e);
    int32_t ret;

    ret = store_computed_object_size(lobj);
    if (ret) {
        LOG_ERROR("Cell [%s] failed to get and store current size", \
                  get_obj_name(lobj));
        return;
    }

    LOG_TRACE("Cell [%s] size updated", get_obj_name(lobj));
}

/*
 * Update cell configuration according to the current state of layout.
 */
static inline int32_t config_grid_cell_position(lv_obj_t *lobj, \
                                                      int8_t r_index_max, \
                                                      int8_t r_index_ofs, \
                                                      int8_t c_index_max, \
                                                      int8_t c_index_ofs)
{
    grid_cell_t *conf;
    lv_obj_t *par;

    if (!lobj)
        return -EINVAL;

    conf = get_grid_cell_data(lobj);
    if (!conf)
        return -EIO;

    conf->row.max = r_index_max;
    conf->row.index = conf->row.index + (r_index_ofs);
    conf->col.max = c_index_max;
    conf->col.index = conf->col.index + (c_index_ofs);

    LOG_TRACE("Cell [%s] new pos row [%d] max [%d] - column [%d] max [%d]", \
              get_obj_name(lobj), \
              conf->row.index, conf->row.max, conf->col.index, conf->col.max);

    return 0;
}

/*
 * Determine whether the new cell should be appended (normal direction)
 * based on current screen rotation and descriptor type.
 */
static inline bool is_append_direction(int32_t scr_rot, dsc_op_t type)
{
    if (type == ADD_ROW || type == REMOVE_ROW) {
        return (scr_rot == ROTATION_0 || scr_rot == ROTATION_90);
    } else if (type == ADD_COLUMN || type == REMOVE_COLUMN) {
        return (scr_rot == ROTATION_0 || scr_rot == ROTATION_270);
    } else {
        LOG_ERROR("Grid layout descriptors operation type invalid: %d", type);
        return false;
    }
}

/*
 * Append new cell descriptor value to existing layout descriptor.
 */
static inline int32_t append_normal_dsc(lv_obj_t *lobj, grid_desc_t *dsc, \
                                        int32_t value)
{
    int32_t ret;

    ret = set_dsc_data(lobj, dsc, value);
    if (ret) {
        LOG_ERROR("Layout %s append new cell failed, ret %d", \
                  get_obj_name(lobj), ret);
        return ret;
    }

    return 0;
}

/*
 * Insert new cell descriptor as a new layout descriptor, cloning
 * the previous one into the new descriptor with new value as first entry.
 */
static int32_t insert_new_grid_dsc(lv_obj_t *lobj, grid_desc_t *dsc, \
                                   int32_t value, dsc_op_t type)
{
    grid_desc_t *next_dsc;
    grid_layout_t *conf;
    int32_t ret;
    int32_t i;

    next_dsc = calloc(1, sizeof(*next_dsc));
    if (!next_dsc)
        return -ENOMEM;

    ret = set_dsc_data(lobj, next_dsc, value);
    if (ret) {
        LOG_ERROR("Layout %s insert new cell failed, ret %d", \
                  get_obj_name(lobj), ret);
        goto out_free;
    }

    for (i = 0; i < dsc->size; i++) {
        ret = set_dsc_data(lobj, next_dsc, dsc->cell_pct[i]);
        if (ret)
            goto out_free;
    }

    conf = get_grid_layout_data(lobj);
    if (!conf) {
        ret = -EIO;
        goto out_free;
    }

    if (type == ADD_ROW)
        conf->row.dsc = next_dsc;
    else if (type == ADD_COLUMN)
        conf->col.dsc = next_dsc;

    free_grid_desc(dsc);
    return 0;

out_free:
    free_grid_desc(next_dsc);
    return ret;
}

/*
 * Remove last cell descriptor as a new layout descriptor, cloning
 * the previous one into the new descriptor.
 */
static int32_t delete_latest_grid_dsc(lv_obj_t *lobj, grid_desc_t *dsc, \
                                      dsc_op_t type)
{
    grid_desc_t *next_dsc;
    grid_layout_t *conf;
    int32_t i, scr_rot;
    int32_t start = 0, stop = 0;
    int32_t ret = 0;

    if (!lobj || !dsc)
        return -EINVAL;

    scr_rot = get_scr_rotation();
    if (is_append_direction(scr_rot, type)) {
        start = 0;
        stop = dsc->size - 1;
    } else {
        start = 1;
        stop = dsc->size;
    }

    next_dsc = calloc(1, sizeof(*next_dsc));
    if (!next_dsc)
        return -ENOMEM;

    for (i = start; i < stop; i++) {
        ret = set_dsc_data(lobj, next_dsc, dsc->cell_pct[i]);
        if (ret)
            goto out_free;
    }

    conf = get_grid_layout_data(lobj);
    if (!conf) {
        ret = -EIO;
        goto out_free;
    }

    if (type == REMOVE_ROW)
        conf->row.dsc = next_dsc;
    else if (type == REMOVE_COLUMN)
        conf->col.dsc = next_dsc;

    free_grid_desc(dsc);
    return 0;

out_free:
    free_grid_desc(next_dsc);
    return ret;
}

static int32_t check_and_delete_invalid_cell_object(lv_obj_t *lobj, dsc_op_t type)
{
    grid_desc_t *r_dsc, *c_dsc;
    grid_cell_t *conf;
    lv_obj_t *par;
    bool del = false;
    int32_t scr_rot, ret = 0;

    if (!lobj)
        return -EINVAL;

    par = lv_obj_get_parent(lobj);
    if (!par)
        return -EIO;

    r_dsc = get_layout_row_dsc_data(par);
    c_dsc = get_layout_col_dsc_data(par);
    conf  = get_grid_cell_data(lobj);
    if (!r_dsc || !c_dsc || !conf)
        return -EIO;

    scr_rot = get_scr_rotation();

    switch (type) {
    case REMOVE_ROW:
        del = is_append_direction(scr_rot, type) ?
              (conf->row.index == conf->row.max) :
              (conf->row.index == 0);
        break;
    case REMOVE_COLUMN:
        del = is_append_direction(scr_rot, type) ?
              (conf->col.index == conf->col.max) :
              (conf->col.index == 0);
        break;
    default:
        return -ENOTSUP;
    }

    if (!del)
        return 0;

    LOG_TRACE("Delete cell [%s] row:%d/%d col:%d/%d",
              get_obj_name(lobj),
              conf->row.index, conf->row.max,
              conf->col.index, conf->col.max);

    ret = remove_obj_and_child_by_name(get_obj_name(lobj), \
                                       &get_gobj(par)->child);
    if (ret) {
        LOG_WARN("Cell [%s] remove failed, ret %d", get_obj_name(lobj), ret);
        return ret;
    }

    /* return semantic: “object was deleted” */
    return -ENOENT;
}

/*
 * Refresh configuration of all grid cells when grid layout descriptors
 * (row/column definitions) are updated.
 */
static int32_t refresh_grid_layout_cells_position(lv_obj_t *lobj, \
                                                  dsc_op_t type)
{
    grid_desc_t *r_dsc, *c_dsc;
    grid_cell_t *conf;
    gobj_t *gobj, *p_obj, *n;
    int8_t r_index_ofs = 0, c_index_ofs = 0;
    int32_t ret, scr_rot;

    if (!lobj)
        return -EINVAL;

    gobj = get_gobj(lobj);
    if (!gobj)
        return -EINVAL;

    if (list_empty(&gobj->child))
        return 0;

    r_dsc = get_layout_row_dsc_data(lobj);
    c_dsc = get_layout_col_dsc_data(lobj);
    if (!r_dsc || !c_dsc)
        return -EIO;

    if (r_dsc->size <= 0 || c_dsc->size <= 0) {
        LOG_ERROR("Layout [%s] dsc invalid: Row size [%d] - Column size [%d]", \
                  get_obj_name(lobj), r_dsc->size, c_dsc->size);
        return -ERANGE;
    }

    scr_rot = get_scr_rotation();
    if (!is_append_direction(scr_rot, type)) {
        if (type == ADD_ROW)
            r_index_ofs = 1;
        else if (type == ADD_COLUMN)
            c_index_ofs = 1;
        else if (type == REMOVE_ROW)
            r_index_ofs = -1;
        else if (type == REMOVE_COLUMN)
            c_index_ofs = -1;
    }

    list_for_each_entry_safe(p_obj, n, &gobj->child, node) {
        if (get_gobj_cell_type(p_obj) != OBJ_GRID_CELL)
            continue;

        ret = check_and_delete_invalid_cell_object(get_lobj(p_obj), type);
        if (ret == -ENOENT)
            continue; /* No problem, object was removed */

        ret = config_grid_cell_position(get_lobj(p_obj), \
                                              r_dsc->size - 1, \
                                              r_index_ofs, \
                                              c_dsc->size - 1, \
                                              c_index_ofs \
                                              );
        if (ret) {
            LOG_WARN("Cell [%s] configuration refresh failed, ret %d",
                     get_gobj_name(p_obj), ret);
        }

        ret = apply_grid_cell_align_and_pos(get_lobj(p_obj));
        if (ret) {
            LOG_WARN("Cell [%s] configuration apply failed, ret %d",
                     get_gobj_name(p_obj), ret);
        }
    }

    return 0;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/*
 * Set the cell of an object. The object's parent needs to have grid layout, else nothing will happen
 * @param obj pointer to an object
 * @param col_align the vertical alignment in the cell. `LV_GRID_START/END/CENTER/STRETCH`
 * @param col_pos column ID
 * @param col_span number of columns to take (>= 1)
 * @param col_max column ID max
 * @param row_align the horizontal alignment in the cell. `LV_GRID_START/END/CENTER/STRETCH`
 * @param row_pos row ID
 * @param row_span number of rows to take (>= 1)
 * @param row_max row ID max
 */
int32_t config_grid_cell_align(lv_obj_t *lobj, lv_grid_align_t col_align, \
                               int8_t col_pos, int8_t col_span, \
                               int8_t col_max, lv_grid_align_t row_align, \
                               int8_t row_pos, int8_t row_span, int8_t row_max)
{
    grid_cell_t *conf;

    if (!lobj)
        return -EINVAL;

    if (row_pos < 0 || row_pos > row_max) {
        LOG_WARN("Cell [%s] invalid row index: %d", \
                 get_obj_name(lobj), row_pos);
        return -EINVAL;
    }

    if (col_pos < 0 || col_pos > col_max) {
        LOG_WARN("Cell [%s] invalid column index: %d", \
                 get_obj_name(lobj), col_pos);
        return -EINVAL;
    }

    conf = get_grid_cell_data(lobj);
    if (!conf) {
        conf = calloc(1, sizeof(*conf));
        if (!conf)
            return -EIO;
        LOG_TRACE("Cell [%s] configuration memory allocated", \
                  get_obj_name(lobj));
    }

    get_gobj(lobj)->layout.cell_data = conf;

    conf->col.index = col_pos;
    conf->col.max = col_max;
    conf->col.span = col_span;
    conf->col.align = col_align;

    conf->row.index = row_pos;
    conf->row.max = row_max;
    conf->row.span = row_span;
    conf->row.align = row_align;

    return 0;
}

int32_t apply_grid_cell_align_and_pos(lv_obj_t * lobj)
{
    grid_cell_t *conf;
    int32_t ret;

    conf = lobj ? get_grid_cell_data(lobj) : NULL;
    if (!conf)
        return -EINVAL;

    lv_obj_set_grid_cell(lobj,
                         conf->col.align, \
                         conf->col.index, \
                         conf->col.span, \
                         conf->row.align, \
                         conf->row.index, \
                         conf->row.span \
                         );
    /*
     * TODO: Remaining issues:
     *
     * 1. To get the size calculated by the layout engine as soon as possible,
     *    we must call "lv_obj_update_layout()". However, multiple consecutive
     *    calls can cause instability or unexpected behavior in LVGL.
     *
     * 2. Without explicitly updating the layout, after applying a new layout,
     *    the object must wait for LVGL to refresh it before obtaining the correct
     *    size. The updated size is usually propagated through the "on_size_changed_cb"
     *    callback.
     *
     *    Due to LVGL’s single-threaded nature, this size update may occur later
     *    than subsequent child refresh or rotation operations. As a result, child
     *    objects might read outdated parent sizes, leading to incorrect size
     *    computations.
     *
     * We must define a reliable synchronization mechanism to ensure child objects
     * recalculate their size only after the parent’s size is fully updated.
     */

    return 0;
}

int32_t set_grid_cell_align(lv_obj_t *lobj, lv_grid_align_t col_align, \
                            int32_t col_pos, int32_t col_span, \
                            lv_grid_align_t row_align, int32_t row_pos, \
                            int32_t row_span)
{
    int32_t ret;
    lv_obj_t *par;
    grid_desc_t *r_dsc, *c_dsc;

    par = lobj ? lv_obj_get_parent(lobj) : NULL;
    if (!par)
        return -EINVAL;

    r_dsc = get_layout_row_dsc_data(par);
    if (!r_dsc)
        return -EIO;

    c_dsc = get_layout_col_dsc_data(par);
    if (!c_dsc)
        return -EIO;

    ret = config_grid_cell_align(lobj, \
                                 col_align, \
                                 col_pos, \
                                 col_span, \
                                 (c_dsc->size - 1), \
                                 row_align, \
                                 row_pos, \
                                 row_span, \
                                 (r_dsc->size - 1) \
                                 );
    if (ret)
        return ret;

    ret = apply_grid_cell_align_and_pos(lobj);
    if (ret)
        return ret;

    lv_obj_add_event_cb(lobj, on_size_changed_cb, \
                        LV_EVENT_SIZE_CHANGED, NULL);

    return 0;
}

int32_t rotate_grid_cell_pos_90(lv_obj_t *lobj)
{
    grid_rc_t *r_cell, *c_cell;
    int32_t ret;

    if (!lobj)
        return -EINVAL;

    r_cell = get_cell_row_data(lobj);
    c_cell = get_cell_col_data(lobj);
    if (!r_cell || !c_cell)
        return -EIO;

    LOG_TRACE("Cell [%s] rotate 90:\tFrom row-col \t[%d][%d]", \
              get_gobj(lobj)->name, r_cell->index, c_cell->index);

    ret = config_grid_cell_align(lobj, \
                                 r_cell->align, \
                                 (r_cell->max - r_cell->index), \
                                 r_cell->span, \
                                 r_cell->max, \
                                 c_cell->align, \
                                 c_cell->index, \
                                 c_cell->span, \
                                 c_cell->max
                                 );

    LOG_TRACE("Cell [%s] rotate 90:\tTo row-col \t[%d][%d]", \
              get_gobj(lobj)->name, r_cell->index, c_cell->index);

    if (ret)
        return ret;

    return 0;
}

/*
 * Append grid layout descriptor.
 * Depending on the rotation and descriptor type (row/col),
 * the operation will either append to current descriptor or
 * create a new one and clone the previous configuration.
 */
int32_t append_grid_layout_dsc(lv_obj_t *lobj, grid_desc_t *dsc, \
                               int32_t value, dsc_op_t type)
{
    int32_t scr_rot;

    if (!lobj || !dsc)
        return -EINVAL;

    scr_rot = get_scr_rotation();

    if (is_append_direction(scr_rot, type))
        return append_normal_dsc(lobj, dsc, value);

    return insert_new_grid_dsc(lobj, dsc, value, type);
}

/*
 * The newly added row descriptor will always remain at the bottom
 * of the logical screen.
 */
int32_t add_grid_layout_row_dsc(lv_obj_t *lobj, int32_t val)
{
    int32_t ret;
    grid_desc_t *dsc;

    if (!lobj)
        return -EINVAL;

    dsc = get_layout_row_dsc_data(lobj);
    if (!dsc)
        return -EIO;

    ret = append_grid_layout_dsc(lobj, dsc, val, ADD_ROW);
    if (ret)
        return ret;

    ret = refresh_grid_layout_cells_position(lobj, ADD_ROW);
    if (ret)
        return ret;

    return 0;
}

/*
 * The newly added column descriptor will always remain at the right
 * side of the logical screen.
 */
int32_t add_grid_layout_col_dsc(lv_obj_t *lobj, int32_t val)
{
    int32_t ret;
    grid_desc_t *dsc;

    if (!lobj)
        return -EINVAL;

    dsc = get_layout_col_dsc_data(lobj);
    if (!dsc)
        return -EIO;

    ret = append_grid_layout_dsc(lobj, dsc, val, ADD_COLUMN);
    if (ret)
        return ret;

    ret = refresh_grid_layout_cells_position(lobj, ADD_COLUMN);
    if (ret)
        return ret;

    return 0;
}

int32_t remove_grid_layout_last_row_dsc(lv_obj_t *lobj)
{
    grid_desc_t *dsc;
    int32_t ret;

    if (!lobj)
        return -EINVAL;

    dsc = get_layout_row_dsc_data(lobj);
    if (!dsc)
        return -EIO;

    ret = delete_latest_grid_dsc(lobj, dsc, REMOVE_ROW);
    if (ret)
        return ret;

    ret = refresh_grid_layout_cells_position(lobj, REMOVE_ROW);
    if (ret)
        return ret;

    return 0;
}

int32_t remove_grid_layout_last_column_dsc(lv_obj_t *lobj)
{
    grid_desc_t *dsc;
    int32_t ret;

    if (!lobj)
        return -EINVAL;

    dsc = get_layout_col_dsc_data(lobj);
    if (!dsc)
        return -EIO;

    ret = delete_latest_grid_dsc(lobj, dsc, REMOVE_COLUMN);
    if (ret)
        return ret;

    ret = refresh_grid_layout_cells_position(lobj, REMOVE_COLUMN);
    if (ret)
        return ret;

    return 0;
}

int32_t apply_grid_layout_dsc(lv_obj_t *lobj)
{
    grid_desc_t *r_dsc, *c_dsc;

    if (!lobj)
        return -EINVAL;

    r_dsc = get_layout_row_dsc_data(lobj);
    c_dsc = get_layout_col_dsc_data(lobj);

    if (!r_dsc || !c_dsc)
        return -EIO;

    lv_obj_set_grid_dsc_array(lobj, c_dsc->cell_pct, r_dsc->cell_pct);

    LOG_DEBUG("Layout [%s] descriptors updated: Row [%d] - Column [%d]", \
              get_obj_name(lobj), r_dsc->size, c_dsc->size);

    return 0;
}

lv_obj_t *create_grid_layout_object(lv_obj_t *par, const char *name)
{
    lv_obj_t *cont = NULL;
    grid_layout_t *conf = NULL;

    if (!par)
        return NULL;

    conf = (grid_layout_t *)calloc(1, sizeof(grid_layout_t));
    if (!conf)
        return NULL;

    conf->row.dsc = (grid_desc_t *)calloc(1, sizeof(grid_desc_t));
    if (!conf->row.dsc)
        goto out_free_conf;

    conf->col.dsc = (grid_desc_t *)calloc(1, sizeof(grid_desc_t));
    if (!conf->col.dsc)
        goto out_free_row_dsc;

    cont = create_box(par, name);
    if (!cont)
        goto out_free_col_dsc;

    get_gobj(cont)->layout.data = conf;

    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_center(cont);

    if (set_obj_layout_type(cont, OBJ_LAYOUT_GRID)) {
        LOG_ERROR("Failed to set object sub type");
        goto out_free_col_dsc;
    }

    return cont;

out_free_col_dsc:
    free(conf->col.dsc);
out_free_row_dsc:
    free(conf->row.dsc);
out_free_conf:
    free(conf);
    return NULL;
}

int32_t config_grid_layout_align(lv_obj_t *lobj, \
                        lv_grid_align_t col_align, lv_grid_align_t row_align)
{
    lv_grid_align_t *r_align, *c_align;

    if (!lobj)
        return -EINVAL;

    r_align = get_layout_row_align(lobj);
    c_align = get_layout_col_align(lobj);

    if (!r_align || !c_align)
        return -EIO;

    *r_align = row_align;
    *c_align = col_align;

    return 0;
}

int32_t apply_grid_layout_align(lv_obj_t *lobj)
{
    lv_grid_align_t *r_align, *c_align;

    if (!lobj)
        return -EINVAL;

    r_align = get_layout_row_align(lobj);
    c_align = get_layout_col_align(lobj);

    if (!r_align || !c_align)
        return -EIO;

    lv_obj_set_grid_align(lobj, *c_align, *r_align);

    return 0;
}

int32_t set_grid_layout_align(lv_obj_t *lobj, \
                        lv_grid_align_t col_align, lv_grid_align_t row_align)
{
    int32_t ret;

    if (!lobj)
        return -EINVAL;

    ret = config_grid_layout_align(lobj, col_align, row_align);
    if (ret)
        return ret;

    ret = apply_grid_layout_align(lobj);
    if (ret)
        return ret;

    return 0;
}

int32_t rotate_grid_dsc_90(lv_obj_t *lobj)
{
    int32_t ret;
    grid_desc_t *r_dsc, *c_dsc;
    grid_desc_t *next_r_dsc = NULL, *next_c_dsc = NULL;
    grid_layout_t *conf;
    int32_t i;

    if (!lobj)
        return -EINVAL;

    r_dsc = get_layout_row_dsc_data(lobj);
    if (!r_dsc)
        return -EIO;

    c_dsc = get_layout_col_dsc_data(lobj);
    if (!c_dsc)
        return -EIO;

    next_r_dsc = calloc(1, sizeof(grid_desc_t));
    if (!next_r_dsc) {
        ret = -ENOMEM;
        goto out;
    }

    next_c_dsc = calloc(1, sizeof(grid_desc_t));
    if (!next_c_dsc) {
        ret = -ENOMEM;
        goto out_free_r;
    }

    for (i = 0; i < c_dsc->size; i++) {
        ret = set_dsc_data(lobj, next_r_dsc, c_dsc->cell_pct[i]);
        if (ret)
            goto out_free_c;
    }

    for (i = (r_dsc->size - 1); i >= 0; i--) {
        ret = set_dsc_data(lobj, next_c_dsc, r_dsc->cell_pct[i]);
        if (ret)
            goto out_free_c;
    }

    free_grid_desc(r_dsc);
    free_grid_desc(c_dsc);

    conf = get_grid_layout_data(lobj);
    if (!conf) {
        ret = -EIO;
        goto out_free_c;
    }

    conf->row.dsc = next_r_dsc;
    conf->col.dsc = next_c_dsc;

    return 0;

out_free_c:
    free_grid_desc(next_c_dsc);
out_free_r:
    free_grid_desc(next_r_dsc);
out:
    return ret;
}

int32_t rotate_grid_align_90(lv_obj_t *lobj)
{
    lv_grid_align_t *r_align, *c_align;
    lv_grid_align_t tmp_align;

    if (!lobj)
        return -EINVAL;

    r_align = get_layout_row_align(lobj);
    c_align = get_layout_col_align(lobj);

    if (!r_align || !c_align)
        return -EIO;

    tmp_align = *r_align;
    *r_align = *c_align;
    *c_align = tmp_align;

    return 0;
}

int32_t rotate_grid_layout_90(lv_obj_t *lobj)
{
    int32_t ret;

    ret = rotate_grid_dsc_90(lobj);
    if (ret) {
        LOG_ERROR("Grid layout dsc rotation failed, ret %d", ret);
        return ret;
    }

    ret = rotate_grid_align_90(lobj);
    if (ret) {
        LOG_ERROR("Grid layout align rotation failed, ret %d", ret);
        return ret;
    }

    LOG_TRACE("Layout [%s] configuration is rotated 90 degree", \
              get_obj_name(lobj));

    return 0;
}

int32_t apply_grid_layout_config(lv_obj_t *lobj)
{
    int32_t ret;

    ret = apply_grid_layout_dsc(lobj);
    if (ret) {
        LOG_ERROR("Grid layout dsc apply failed, ret %d", ret);
        return ret;
    }

    ret = apply_grid_layout_align(lobj);
    if (ret) {
        LOG_ERROR("Grid layout align apply failed, ret %d", ret);
        return ret;
    }

    LOG_TRACE("Layout [%s] configuration is updated", get_obj_name(lobj));

    return 0;
}
