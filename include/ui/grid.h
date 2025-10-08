/**
 * @file grid.h
 *
 */

#ifndef G_GRID_H
#define G_GRID_H
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
    int8_t max;
    int8_t span;
    lv_grid_align_t align;
} grid_rc_t;

typedef struct grid_cell {
    grid_rc_t row;
    grid_rc_t col;
} grid_cell_t;

typedef struct grid_desc {
    int8_t size;
    int32_t *cell_pct;
} grid_desc_t;

typedef struct grid_pad {
    int8_t scale;
    int8_t pct;
    int32_t px;
} grid_pad_t;

typedef struct grid_layout_rc {
    grid_desc_t *dsc;
    lv_grid_align_t align;
    grid_pad_t *pad;
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
lv_obj_t *create_grid_layout_object(lv_obj_t *par, const char *name);

int32_t add_grid_layout_col_dsc(lv_obj_t *lobj, int32_t val);
int32_t add_grid_layout_row_dsc(lv_obj_t *lobj, int32_t val);
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
                               int8_t col_pos, int8_t col_span, \
                               int8_t col_max, lv_grid_align_t row_align, \
                               int8_t row_pos, int8_t row_span, int8_t row_max);
int32_t apply_grid_cell_align_and_pos(lv_obj_t * lobj);
int32_t set_grid_cell_align(lv_obj_t * lobj, lv_grid_align_t col_align, \
                            int32_t col_pos, int32_t col_span, \
                            lv_grid_align_t row_align, int32_t row_pos, \
                            int32_t row_span);
int32_t rotate_grid_cell_pos_90(lv_obj_t *lobj);

int32_t rotate_grid_dsc_90(lv_obj_t *lobj);
int32_t rotate_grid_align_90(lv_obj_t *lobj);
int32_t rotate_grid_rc_pad_90(lv_obj_t *lobj);
int32_t rotate_grid_layout_90(lv_obj_t *lobj);
int32_t apply_grid_layout_config(lv_obj_t *lobj);

static inline grid_layout_t *get_grid_layout_data(lv_obj_t *lobj)
{
    return lobj ? (grid_layout_t *)get_gobj(lobj)->data.internal : NULL;
}

static inline grid_pad_t *get_layout_row_pad_data(lv_obj_t *lobj)
{
    grid_layout_t *layout = get_grid_layout_data(lobj);
    return layout ? layout->row.pad : NULL;
}

static inline grid_pad_t *get_layout_col_pad_data(lv_obj_t *lobj)
{
    grid_layout_t *layout = get_grid_layout_data(lobj);
    return layout ? layout->col.pad : NULL;
}

static inline grid_desc_t *get_layout_row_dsc_data(lv_obj_t *lobj)
{
    grid_layout_t *layout = get_grid_layout_data(lobj);
    return layout ? layout->row.dsc : NULL;
}

static inline grid_desc_t *get_layout_col_dsc_data(lv_obj_t *lobj)
{
    grid_layout_t *layout = get_grid_layout_data(lobj);
    return layout ? layout->col.dsc : NULL;
}

static inline lv_grid_align_t *get_layout_row_align(lv_obj_t *lobj)
{
    grid_layout_t *layout = get_grid_layout_data(lobj);
    return layout ? &layout->row.align : NULL;
}

static inline lv_grid_align_t *get_layout_col_align(lv_obj_t *lobj)
{
    grid_layout_t *layout = get_grid_layout_data(lobj);
    return layout ? &layout->col.align : NULL;
}

static inline grid_cell_t *get_cell_data(lv_obj_t *lobj)
{
    return lobj ? (grid_cell_t *)get_gobj(lobj)->data.sub_data : NULL;
}

static inline grid_rc_t *get_cell_row_data(lv_obj_t *lobj)
{
    grid_cell_t *cell_data = get_cell_data(lobj);
    return cell_data ? &cell_data->row : NULL;
}

static inline grid_rc_t *get_cell_col_data(lv_obj_t *lobj)
{
    grid_cell_t *cell_data = get_cell_data(lobj);
    return cell_data ? &cell_data->col : NULL;
}

static inline int32_t set_grid_cell_type(lv_obj_t *lobj)
{
    return set_obj_cell_type(lobj, OBJ_GRID_CELL);
}
/**********************
 *      MACROS
 **********************/

#endif /* G_GRID_H */
