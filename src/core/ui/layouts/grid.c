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
static void get_base_size(lv_obj_t *par, int32_t *par_w, int32_t *par_h)
{
    if (par) {
        *par_w = get_w(par);
        *par_h = get_h(par);
        LOG_TRACE("Descriptor based on parent size: W[%d] H[%d]",
                  *par_w, *par_h);
    } else {
        *par_w = get_scr_width();
        *par_h = get_scr_height();
        LOG_TRACE("Descriptor based on screen: W[%d] H[%d]",
                  *par_w, *par_h);
    }
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

static int32_t set_dsc_data(lv_obj_t *par, grid_desc_t *dsc,
                     int8_t is_row, int8_t scale, int32_t val)
{
    int32_t par_w, par_h;
    int32_t ret;

    if (!dsc)
        return -EINVAL;

    get_base_size(par, &par_w, &par_h);

    ret = alloc_or_extend_dsc(dsc);
    if (ret)
        return ret;

    fill_new_slot(dsc, is_row, par_w, par_h, scale, val);

    return 0;
}
/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int32_t add_grid_layout_row_dsc(lv_obj_t *lobj, int8_t scale, int32_t val)
{
    int32_t ret;
    grid_desc_t *dsc;
    lv_obj_t *par;

    if (!lobj)
        return -EINVAL;

    dsc = &((grid_layout_t *)get_gobj(lobj)->data.internal)->row_dsc;


    par = get_lobj(l_to_par_gobj(lobj));
    if (par == lv_scr_act()) {
        par = NULL;
    }

    ret = set_dsc_data(par, dsc, IS_ROW, scale, val);
    if (ret)
        return ret;

    return 0;
}

int32_t add_grid_layout_col_dsc(lv_obj_t *lobj, int8_t scale, int32_t val)
{
    int32_t ret;
    grid_desc_t *dsc;
    lv_obj_t *par;

    if (!lobj)
        return -EINVAL;

    dsc = &((grid_layout_t *)get_gobj(lobj)->data.internal)->col_dsc;


    par = get_lobj(l_to_par_gobj(lobj));
    if (par == lv_scr_act()) {
        par = NULL;
    }

    ret = set_dsc_data(par, dsc, IS_COL, scale, val);
    if (ret)
        return ret;

    return 0;
}

int32_t apply_grid_layout_dsc(lv_obj_t *lobj)
{
    grid_layout_t *layout;

    layout = lobj ? (grid_layout_t *)get_gobj(lobj)->data.internal : NULL;
    if (!layout)
        return -EINVAL;

    if (!layout->row_dsc.cell_px || !layout->col_dsc.cell_px)
        return -EIO;

    lv_obj_set_grid_dsc_array(lobj, layout->col_dsc.cell_px,
                                   layout->row_dsc.cell_px);

    LOG_DEBUG("Applied grid descriptors: row=%d col=%d",
              layout->row_dsc.size, layout->col_dsc.size);

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

    lv_obj_t *cont = create_base(par, name);
    if (!cont) {
        free(conf);
        return NULL;
    }

    get_gobj(cont)->data.internal = conf;

    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_center(cont);

    return cont;
}

int32_t set_grid_layout_align(lv_obj_t *lobj, \
                        lv_grid_align_t col_align, lv_grid_align_t row_align)
{
    grid_layout_t *conf;

    if (!lobj)
        return -EINVAL;

    lv_obj_set_grid_align(lobj, col_align, row_align);

    conf = (grid_layout_t *)get_gobj(lobj)->data.internal;
    if (!conf)
        return -EIO;

    conf->row_align = row_align;
    conf->col_align = col_align;

    return 0;
}

