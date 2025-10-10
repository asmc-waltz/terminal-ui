/**
 * @file flex.h
 *
 */

#ifndef G_FLEX_H
#define G_FLEX_H
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
typedef struct flex_cell {
    lv_border_side_t border_side;
    int32_t pad_top;
    int32_t pad_bot;
    int32_t pad_left;
    int32_t pad_right;
} flex_cell_t;

typedef struct flex_layout {
    lv_flex_flow_t flow;
    lv_flex_align_t main_place;
    lv_flex_align_t cross_place;
    lv_flex_align_t track_place;
    int32_t pad_row;
    int32_t pad_column;
} flex_layout_t;

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
lv_obj_t *create_flex_layout_object(lv_obj_t *par, const char *name);
int32_t config_flex_layout_align(lv_obj_t *lobj, lv_flex_align_t main_place, \
                                 lv_flex_align_t cross_place, \
                                 lv_flex_align_t track_cross_place);
int32_t apply_flex_layout_align(lv_obj_t *lobj);

int32_t config_flex_layout_flow(lv_obj_t *lobj, lv_flex_flow_t flow);
int32_t apply_flex_layout_flow(lv_obj_t *lobj);
int32_t set_flex_layout_flow(lv_obj_t *lobj, lv_flex_flow_t flow);

int32_t rotate_flex_layout_90(lv_obj_t *lobj);

int32_t config_flex_cell_border_side(lv_obj_t *lobj, int32_t value);
int32_t apply_flex_cell_border_side(lv_obj_t *lobj);
int32_t set_flex_cell_border_side(lv_obj_t *lobj, int32_t value);
int32_t config_flex_cell_pad(lv_obj_t *lobj, int32_t pad_top, int32_t pad_bot, \
                             int32_t pad_left, int32_t pad_right);
int32_t apply_flex_cell_pad(lv_obj_t *lobj);
int32_t set_flex_cell_pad(lv_obj_t *lobj, int32_t pad_top, int32_t pad_bot, \
                          int32_t pad_left, int32_t pad_right);

int32_t rotate_flex_cell_border_side_90(lv_obj_t *lobj);
int32_t rotate_flex_cell_90(lv_obj_t *lobj);
int32_t apply_flex_cell_config(lv_obj_t *lobj);

static inline flex_layout_t *get_flex_layout_data(lv_obj_t *lobj)
{
    return lobj ? (flex_layout_t *)get_gobj(lobj)->data.internal : NULL;
}

static inline int32_t set_flex_cell_type(lv_obj_t *lobj)
{
    return set_obj_cell_type(lobj, OBJ_FLEX_CELL);
}

static inline flex_cell_t *get_flex_cell_data(lv_obj_t *lobj)
{
    return lobj ? (flex_cell_t *)get_gobj(lobj)->data.sub_data : NULL;
}

/**********************
 *      MACROS
 **********************/

#endif /* G_FLEX_H */
