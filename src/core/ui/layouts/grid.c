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
int32_t set_dsc_data(grid_desc_t *dsc, int32_t value)
{
    int32_t *new_arr;

    if (!dsc)
        return -EINVAL;

    if (!dsc->arr) {
        LOG_INFO("Create new grid descriptor");
        dsc->size = 0;
        dsc->arr = calloc(2, sizeof(int32_t)); /* 1 value + LV_GRID_TEMPLATE_LAST */
        if (!dsc->arr)
            return -ENOMEM;
    } else {
        new_arr = realloc(dsc->arr, (dsc->size + 2) * sizeof(int32_t));
        if (!new_arr)
            return -ENOMEM;
        dsc->arr = new_arr;
    }

    dsc->arr[dsc->size] = value;
    dsc->size++;
    dsc->arr[dsc->size] = LV_GRID_TEMPLATE_LAST;

    return 0;
}

int32_t get_dsc_data(grid_desc_t *dsc, int32_t index, int32_t value)
{
    if (!dsc || !dsc->arr)
        return -EINVAL;

    if (index < 0 || index >= dsc->size)
        return -EINVAL;

    return dsc->arr[index];
}

int32_t edit_dsc_data(grid_desc_t *dsc, int32_t index, int32_t value)
{
    if (!dsc || !dsc->arr)
        return -EINVAL;

    if (index < 0 || index >= dsc->size)
        return -EINVAL;

    dsc->arr[index] = value;
    return 0;
}

void free_dsc(grid_desc_t *dsc)
{
    if (!dsc)
        return;

    if (dsc->arr) {
        LOG_DEBUG("Free grid descriptor size=%d", dsc->size);
        free(dsc->arr);
        dsc->arr = NULL;
    }

    dsc->size = 0;
}

int32_t apply_grid_layout(lv_obj_t *lobj, grid_layout_t *layout)
{
    if (!lobj || !layout)
        return -EINVAL;

    if (!layout->row_dsc.arr || !layout->col_dsc.arr)
        return -EINVAL;

    lv_obj_set_grid_dsc_array(lobj, layout->col_dsc.arr,
                                   layout->row_dsc.arr);

    return 0;
}

/**********************
 *   GLOBAL FUNCTIONS
 ***********Az**********/
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

