/**
 * @file layout.h
 *
 */

#ifndef G_LAYOUT_H
#define G_LAYOUT_H
/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>

#include <lvgl.h>
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

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*=====================
 * Setter functions
 *====================*/

/*=====================
 * Getter functions
 *====================*/

/*=====================
 * Other functions
 *====================*/
lv_obj_t *create_grid_layout(lv_obj_t *par, const char *name);

int32_t set_grid_layout(lv_obj_t *lobj, \
                        lv_grid_align_t col_align, lv_grid_align_t row_align);

/**********************
 *      MACROS
 **********************/

#endif /* G_LAYOUT_H */
