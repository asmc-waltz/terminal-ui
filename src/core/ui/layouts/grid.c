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

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int32_t set_dsc_data(lv_obj_t *par, grid_desc_t *dsc, int8_t is_row, int32_t val_pct)
{
    int32_t par_w, par_h;
    int32_t *new_px, *new_pct;

    if (!dsc)
        return -EINVAL;

    /* Determine base size */
    if (par) {
        par_w = get_w(par);
        par_h = get_h(par);
        LOG_TRACE("Descriptor based on parent size: W[%d] H[%d]", par_w, par_h);
    } else {
        par_w = get_scr_width();
        par_h = get_scr_height();
        LOG_TRACE("Descriptor based on screen: W[%d] H[%d]", par_w, par_h);
    }

    /* First-time allocation */
    if (!dsc->cell_px || !dsc->cell_pct) {
        LOG_TRACE("Create new grid descriptor");
        dsc->size = 0;

        dsc->cell_px  = calloc(2, sizeof(int32_t));
        dsc->cell_pct = calloc(2, sizeof(int32_t));
        if (!dsc->cell_px || !dsc->cell_pct)
            return -ENOMEM;
    } else {
        /* Extend both arrays in one go */
        LOG_TRACE("Append value into grid descriptor");
        new_px  = realloc(dsc->cell_px,  (dsc->size + 2) * sizeof(int32_t));
        new_pct = realloc(dsc->cell_pct, (dsc->size + 2) * sizeof(int32_t));

        if (!new_px || !new_pct)
            return -ENOMEM;

        dsc->cell_px  = new_px;
        dsc->cell_pct = new_pct;
    }

    /* Fill new slot */
    dsc->cell_pct[dsc->size] = val_pct;
    dsc->cell_px[dsc->size]  = is_row ?
        pct_to_px(par_h, val_pct) : pct_to_px(par_w, val_pct);

    dsc->size++;

    /* Always keep sentinel */
    dsc->cell_pct[dsc->size] = LV_GRID_TEMPLATE_LAST;
    dsc->cell_px[dsc->size]  = LV_GRID_TEMPLATE_LAST;

    return 0;
}

int32_t get_dsc_data(grid_desc_t *dsc, int32_t index, int32_t value)
{
    if (!dsc || !dsc->cell_px)
        return -EINVAL;

    if (index < 0 || index >= dsc->size)
        return -EINVAL;

    return dsc->cell_px[index];
}

int32_t edit_dsc_data(grid_desc_t *dsc, int32_t index, int32_t value)
{
    if (!dsc || !dsc->cell_px)
        return -EINVAL;

    if (index < 0 || index >= dsc->size)
        return -EINVAL;

    dsc->cell_px[index] = value;
    return 0;
}

void free_dsc(grid_desc_t *dsc)
{
    if (!dsc)
        return;

    if (dsc->cell_px) {
        LOG_DEBUG("Free grid descriptor size=%d", dsc->size);
        free(dsc->cell_px);
        dsc->cell_px = NULL;
    }

    dsc->size = 0;
}

int32_t apply_grid_layout(lv_obj_t *lobj, grid_layout_t *layout)
{
    if (!lobj || !layout)
        return -EINVAL;

    if (!layout->row_dsc.cell_px || !layout->col_dsc.cell_px)
        return -EINVAL;

    lv_obj_set_grid_dsc_array(lobj, layout->col_dsc.cell_px,
                                   layout->row_dsc.cell_px);

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

int32_t set_grid_layout(lv_obj_t *lobj, \
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

