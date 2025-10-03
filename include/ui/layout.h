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
#define IS_COL                          0
#define IS_ROW                          1

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
    int8_t *scale;
    int8_t *cell_pct;
    int32_t *cell_px;
} grid_desc_t;

typedef struct grid_pad {
    int8_t scale;
    int8_t pct;
    int32_t px;
} grid_pad_t;

typedef struct grid_layout_rc {
    grid_desc_t dsc;
    lv_grid_align_t align;
    grid_pad_t pad;
} grid_layout_rc_t;

typedef struct grid_layout {
    grid_layout_rc_t row;
    grid_layout_rc_t col;
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

int32_t add_grid_layout_col_dsc(lv_obj_t *lobj, int8_t scale, int32_t val);
int32_t add_grid_layout_row_dsc(lv_obj_t *lobj, int8_t scale, int32_t val);
int32_t apply_grid_layout_dsc(lv_obj_t *lobj);

int32_t config_grid_layout_align(lv_obj_t *lobj, \
                        lv_grid_align_t col_align, lv_grid_align_t row_align);
int32_t apply_grid_layout_align(lv_obj_t *lobj);
int32_t set_grid_layout_align(lv_obj_t *lobj, \
                        lv_grid_align_t col_align, lv_grid_align_t row_align);

int32_t config_grid_layout_pad_col(lv_obj_t *lobj, int8_t scale, int32_t val);
int32_t config_grid_layout_pad_row(lv_obj_t *lobj, int8_t scale, int32_t val);
int32_t apply_grid_layout_gap(lv_obj_t *lobj);
int32_t set_grid_layout_gap(lv_obj_t *lobj, int8_t scale_col, int32_t pad_col, \
                            int8_t scale_row, int32_t pad_row);

int32_t config_grid_cell_align(lv_obj_t *lobj, lv_grid_align_t col_align, \
                               int32_t col_pos, int32_t col_span, \
                               lv_grid_align_t row_align, int32_t row_pos, \
                               int32_t row_span);
int32_t apply_grid_cell_align(lv_obj_t * lobj);
int32_t set_grid_cell_align(lv_obj_t * lobj, lv_grid_align_t col_align, \
                            int32_t col_pos, int32_t col_span, \
                            lv_grid_align_t row_align, int32_t row_pos, \
                            int32_t row_span);

/**********************
 *      MACROS
 **********************/

#endif /* G_LAYOUT_H */
