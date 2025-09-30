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
#include "main.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct grid_rc{
    int8_t index;
    int8_t span;
    lv_grid_align_t align;
} grid_rc_t;

typedef struct grid_cell {
    grid_rc_t row;
    grid_rc_t col;
} grid_cell_t;

typedef struct grid_desc {
    int8_t size;
    int32_t *arr;
} grid_desc_t;

typedef struct grid_layout {
    grid_desc_t row_dsc;
    grid_desc_t col_dsc;
    lv_grid_align_t col_align;
    lv_grid_align_t row_align;
} grid_layout_t;
/*** *******************
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
 ***********Az**********/
lv_obj_t *create_grid_layout(lv_obj_t *par, const char *name, \
                             int32_t *col_dsc, int32_t *row_dsc)
{

    grid_layout_t *conf;

    if (!par || !col_dsc || !row_dsc)
        return NULL;

    conf = calloc(1, sizeof(*conf));
    if (!conf)
        return NULL;

    conf->col_dsc.arr = col_dsc;
    conf->row_dsc.arr = row_dsc;


    lv_obj_t *cont = create_base(par, name);
    if (!cont) {
        free(conf);
        return NULL;
    }

    get_gobj(cont)->data.internal = conf;

    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
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

