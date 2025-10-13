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
typedef struct flex_layout {
    lv_flex_flow_t flow;
    lv_flex_align_t main_place;
    lv_flex_align_t cross_place;
    lv_flex_align_t track_place;
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
int32_t config_flex_layout_align(lv_obj_t *lobj, lv_flex_align_t main_place, \
                                 lv_flex_align_t cross_place, \
                                 lv_flex_align_t track_cross_place);
int32_t apply_flex_layout_align(lv_obj_t *lobj);
int32_t set_flex_layout_align(lv_obj_t *lobj, lv_flex_align_t main_place, \
                                 lv_flex_align_t cross_place, \
                                 lv_flex_align_t track_cross_place);

int32_t config_flex_layout_flow(lv_obj_t *lobj, lv_flex_flow_t flow);
int32_t apply_flex_layout_flow(lv_obj_t *lobj);
int32_t set_flex_layout_flow(lv_obj_t *lobj, lv_flex_flow_t flow);

int32_t apply_flex_layout_config(lv_obj_t *lobj);
int32_t apply_flex_cell_config(lv_obj_t *lobj);

/*=====================
 * Getter functions
 *====================*/
static inline flex_layout_t *get_flex_layout_data(lv_obj_t *lobj)
{
    return lobj ? (flex_layout_t *)get_gobj(lobj)->data.internal : NULL;
}

/*=====================
 * Other functions
 *====================*/
lv_obj_t *create_flex_layout_object(lv_obj_t *par, const char *name);
int32_t rotate_flex_align_one(lv_obj_t *lobj);
int32_t rotate_flex_layout_90(lv_obj_t *lobj);
int32_t rotate_flex_cell_90(lv_obj_t *lobj);

static inline int32_t scroll_to_first_child(lv_obj_t *par)
{
    lv_obj_t *child;

    child = par ? lv_obj_get_child(par, 0) : NULL;
    if (!child) {
        return -EINVAL;
    }

    if (!lv_obj_has_flag(child, LV_OBJ_FLAG_HIDDEN) && lv_obj_is_valid(child)) {
        lv_obj_scroll_to_view(child, LV_ANIM_OFF);
    } else {
        LOG_ERROR("First child is hidden or invalid");
    }

    return 0;
}
/**********************
 *      MACROS
 **********************/

#endif /* G_FLEX_H */
