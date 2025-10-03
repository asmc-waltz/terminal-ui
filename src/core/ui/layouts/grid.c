/**
 * @file grid.c
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
#include "ui/layout.h"

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
static int32_t get_base_size(lv_obj_t *lobj, int32_t *par_w, int32_t *par_h)
{
    lv_obj_t *par;

    par = lobj ? lv_obj_get_parent(lobj) : NULL;
    if (!par)
        return -EINVAL;

    if (par == lv_screen_active()) {
        *par_w = get_scr_width();
        *par_h = get_scr_height();
        LOG_TRACE("Descriptor based on screen: W[%d] H[%d]",
                  *par_w, *par_h);
    } else {
        *par_w = get_w(par);
        *par_h = get_h(par);
        LOG_TRACE("Descriptor based on parent size: W[%d] H[%d]",
                  *par_w, *par_h);
    }

    return 0;
}

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

    new_arr = alloc_or_extend_array(dsc->scale, sizeof(int8_t), dsc->size);
    if (!new_arr)
        return -ENOMEM;
    dsc->scale = new_arr;

    new_arr = alloc_or_extend_array(dsc->cell_pct, sizeof(int8_t), dsc->size);
    if (!new_arr)
        return -ENOMEM;
    dsc->cell_pct = new_arr;

    new_arr = alloc_or_extend_array(dsc->cell_px, sizeof(int32_t), dsc->size);
    if (!new_arr)
        return -ENOMEM;
    dsc->cell_px = new_arr;

    return 0;
}

static void fill_new_slot(grid_desc_t *dsc, int8_t is_row,
                          int32_t par_w, int32_t par_h,
                          int8_t scale, int32_t val)
{
    dsc->scale[dsc->size] = scale;

    if (scale == ENA_SCALE) {
        dsc->cell_pct[dsc->size] = val;
        dsc->cell_px[dsc->size]  = is_row ?
            pct_to_px(par_h, val) : pct_to_px(par_w, val);
    } else {
        dsc->cell_px[dsc->size]  = val;
        dsc->cell_pct[dsc->size] = is_row ?
            px_to_pct(par_h, val) : px_to_pct(par_w, val);
    }

    dsc->size++;

    /* Always keep sentinel */
    dsc->scale[dsc->size]    = LV_GRID_TEMPLATE_LAST;
    dsc->cell_pct[dsc->size] = LV_GRID_TEMPLATE_LAST;
    dsc->cell_px[dsc->size]  = LV_GRID_TEMPLATE_LAST;
}

static int32_t set_dsc_data(lv_obj_t *lobj, grid_desc_t *dsc,
                     int8_t is_row, int8_t scale, int32_t val)
{
    int32_t par_w, par_h;
    int32_t ret;

    if (!dsc)
        return -EINVAL;

    ret = get_base_size(lobj, &par_w, &par_h);
    if (ret)
        return ret;

    ret = alloc_or_extend_dsc(dsc);
    if (ret)
        return ret;

    fill_new_slot(dsc, is_row, par_w, par_h, scale, val);

    return 0;
}

static void free_grid_desc(grid_desc_t *dsc)
{
    if (!dsc)
        return;

    if (dsc->scale) {
        free(dsc->scale);
        dsc->scale = NULL;
    }

    if (dsc->cell_pct) {
        free(dsc->cell_pct);
        dsc->cell_pct = NULL;
    }

    if (dsc->cell_px) {
        free(dsc->cell_px);
        dsc->cell_px = NULL;
    }

    dsc->size = 0;
    free(dsc);
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
 * @param row_align the horizontal alignment in the cell. `LV_GRID_START/END/CENTER/STRETCH`
 * @param row_pos row ID
 * @param row_span number of rows to take (>= 1)
 */
int32_t config_grid_cell_align(lv_obj_t *lobj, lv_grid_align_t col_align, \
                               int32_t col_pos, int32_t col_span, \
                               lv_grid_align_t row_align, int32_t row_pos, \
                               int32_t row_span)
{
    grid_cell_t *conf;

    if (!lobj)
        return -EINVAL;

    conf = calloc(1, sizeof(*conf));
    if (!conf)
        return -ENOMEM;

    get_gobj(lobj)->data.internal = conf;

    conf->col.index = col_pos;
    conf->col.span = col_span;
    conf->col.align = col_align;

    conf->row.index = row_pos;
    conf->row.span = row_span;
    conf->row.align = row_align;

    return 0;
}

int32_t apply_grid_cell_align(lv_obj_t * lobj)
{
    grid_cell_t *conf;

    conf = lobj ? (grid_cell_t *)get_gobj(lobj)->data.internal : NULL;
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

    return 0;
}

int32_t set_grid_cell_align(lv_obj_t * lobj, lv_grid_align_t col_align, \
                            int32_t col_pos, int32_t col_span, \
                            lv_grid_align_t row_align, int32_t row_pos, \
                            int32_t row_span)
{
    int32_t ret;

    ret = config_grid_cell_align(lobj, \
                                 col_align, \
                                 col_pos, \
                                 col_span, \
                                 row_align, \
                                 row_pos, \
                                 row_span \
                                 );
    if (ret)
        return ret;

    ret = apply_grid_cell_align(lobj);
    if (ret)
        return ret;

    return 0;
}

int32_t add_grid_layout_row_dsc(lv_obj_t *lobj, int8_t scale, int32_t val)
{
    int32_t ret;
    grid_desc_t *dsc;

    if (!lobj)
        return -EINVAL;

    dsc = get_layout_row_dsc_data(lobj);
    if (!dsc)
        return -EIO;

    ret = set_dsc_data(lobj, dsc, IS_ROW, scale, val);
    if (ret)
        return ret;

    return 0;
}

int32_t add_grid_layout_col_dsc(lv_obj_t *lobj, int8_t scale, int32_t val)
{
    int32_t ret;
    grid_desc_t *dsc;

    if (!lobj)
        return -EINVAL;

    dsc = get_layout_col_dsc_data(lobj);
    if (!dsc)
        return -EIO;

    ret = set_dsc_data(lobj, dsc, IS_COL, scale, val);
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

    lv_obj_set_grid_dsc_array(lobj, c_dsc->cell_px, r_dsc->cell_px);

    LOG_DEBUG("Applied grid descriptors: row=%d col=%d",
              r_dsc->size, c_dsc->size);

    return 0;
}

lv_obj_t *create_grid_layout(lv_obj_t *par, const char *name)
{

    grid_layout_t *conf;

    if (!par)
        return NULL;

    conf = calloc(1, sizeof(*conf));
    if (!conf)
        return NULL;

    conf->row.dsc = calloc(1, sizeof(grid_desc_t));
    if (!conf->row.dsc) {
        free(conf);
        return NULL;
    }

    conf->col.dsc = calloc(1, sizeof(grid_desc_t));
    if (!conf->col.dsc) {
        free(conf->row.dsc);
        free(conf);
        return NULL;
    }

    lv_obj_t *cont = create_layout(par, name);
    if (!cont) {
        free(conf);
        return NULL;
    }

    get_gobj(cont)->data.internal = conf;

    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_center(cont);

    return cont;
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

int32_t config_grid_layout_gap(lv_obj_t *lobj, int8_t is_row, int8_t scale, \
                               int32_t val)
{
    int32_t par_w, par_h;
    grid_pad_t *pad;
    int32_t ret;

    if (!lobj)
        return -EINVAL;

    if (is_row == IS_ROW)
        pad = get_layout_row_pad_data(lobj);
    else
        pad = get_layout_col_pad_data(lobj);

    if (!pad)
        return -EIO;

    ret = get_base_size(lobj, &par_w, &par_h);
    if (ret)
        return ret;

    pad->scale = scale;
    if (scale == ENA_SCALE) {
        pad->pct = val;
        pad->px = is_row ? pct_to_px(par_h, val) : pct_to_px(par_w, val);
    } else {
        pad->px = val;
        pad->pct = is_row ? px_to_pct(par_h, val) : px_to_pct(par_w, val);
    }

    return 0;
}

int32_t config_grid_layout_pad_col(lv_obj_t *lobj, int8_t scale, int32_t val)
{
    return config_grid_layout_gap(lobj, IS_COL, scale, val);
}

int32_t config_grid_layout_pad_row(lv_obj_t *lobj, int8_t scale, int32_t val)
{
    return config_grid_layout_gap(lobj, IS_ROW, scale, val);
}

int32_t apply_grid_layout_gap(lv_obj_t *lobj)
{
    grid_pad_t *row_pad, *col_pad;

    if (!lobj)
        return -EINVAL;

    row_pad = get_layout_row_pad_data(lobj);
    col_pad = get_layout_col_pad_data(lobj);

    if (!row_pad || !col_pad)
        return -EIO;

    lv_obj_set_style_pad_row(lobj, row_pad->px, 0);
    lv_obj_set_style_pad_column(lobj, col_pad->px, 0);

    return 0;
}

int32_t set_grid_layout_gap(lv_obj_t *lobj, int8_t scale_col, int32_t pad_col, \
                            int8_t scale_row, int32_t pad_row)
{
    int32_t ret;

    if (!lobj)
        return -EINVAL;

    ret = config_grid_layout_pad_row(lobj, scale_row, pad_row);
    if (ret)
        return ret;

    ret = config_grid_layout_pad_col(lobj, scale_col, pad_col);
    if (ret)
        return ret;

    ret = apply_grid_layout_gap(lobj);
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
        if (c_dsc->scale[i] == ENA_SCALE)
            ret = set_dsc_data(lobj, next_r_dsc, IS_ROW, ENA_SCALE, \
                               c_dsc->cell_pct[i]);
        else
            ret = set_dsc_data(lobj, next_r_dsc, IS_ROW, DIS_SCALE, \
                               c_dsc->cell_px[i]);

        if (ret)
            goto out_free_c;
    }

    for (i = (r_dsc->size - 1); i >= 0; i--) {
        if (r_dsc->scale[i] == ENA_SCALE)
            ret = set_dsc_data(lobj, next_c_dsc, IS_COL, ENA_SCALE, \
                               r_dsc->cell_pct[i]);
        else
            ret = set_dsc_data(lobj, next_c_dsc, IS_COL, DIS_SCALE, \
                               r_dsc->cell_px[i]);

        if (ret)
            goto out_free_c;
    }

    free_grid_desc(r_dsc);
    free_grid_desc(c_dsc);

    conf = get_layout_data(lobj);
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
