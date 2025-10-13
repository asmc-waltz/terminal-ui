/**
 * @file border.c
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
static inline int32_t config_gobj_border_side(lv_obj_t *lobj, int32_t value)
{
    gobj_t *gobj;

    gobj = lobj ? get_gobj(lobj) : NULL;
    if (!gobj)
        return -EINVAL;

    gobj->layout.border_side = value;

    return 0;
}


/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/*
 * Rotate object border sides by 90° clockwise.
 * Uses a lookup table for all 16 combinations of LV_BORDER_SIDE_* flags.
 */
int32_t rotate_border_side_90(lv_obj_t *lobj)
{
    gobj_t *gobj;
    lv_border_side_t current;

    gobj = lobj ? get_gobj(lobj) : NULL;
    if (!gobj)
        return -EINVAL;

    static const lv_border_side_t border_rot_table[16] = {
        [LV_BORDER_SIDE_NONE] = LV_BORDER_SIDE_NONE,
        [LV_BORDER_SIDE_TOP] = LV_BORDER_SIDE_RIGHT,
        [LV_BORDER_SIDE_BOTTOM] = LV_BORDER_SIDE_LEFT,
        [LV_BORDER_SIDE_LEFT] = LV_BORDER_SIDE_TOP,
        [LV_BORDER_SIDE_RIGHT] = LV_BORDER_SIDE_BOTTOM,
        [LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT] =
                                LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM,
        [LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM] =
                                LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT,
        [LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_RIGHT] =
                                LV_BORDER_SIDE_RIGHT | LV_BORDER_SIDE_BOTTOM,
        [LV_BORDER_SIDE_RIGHT | LV_BORDER_SIDE_BOTTOM] =
                                LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_BOTTOM,
        [LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_BOTTOM] =
                                LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_TOP,
        [LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_TOP] =
                                LV_BORDER_SIDE_RIGHT | LV_BORDER_SIDE_TOP,
        [LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_RIGHT | LV_BORDER_SIDE_BOTTOM] =
                                LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_BOTTOM |
                                LV_BORDER_SIDE_RIGHT,
        [LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_RIGHT] =
                                LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_RIGHT |
                                LV_BORDER_SIDE_BOTTOM,
        [LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT | LV_BORDER_SIDE_BOTTOM] =
                                LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_LEFT |
                                LV_BORDER_SIDE_BOTTOM,
        [LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_BOTTOM] =
                                LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_TOP |
                                LV_BORDER_SIDE_RIGHT,
        [LV_BORDER_SIDE_FULL] = LV_BORDER_SIDE_FULL,
    };

    current = gobj->layout.border_side;
    if (current >= 16) {
        LOG_ERROR("Object %s has invalid border mask %d",
                  get_name(lobj), current);
        return -EINVAL;
    }

    config_gobj_border_side(lobj, border_rot_table[current]);

    return 0;
}

int32_t apply_border_side(lv_obj_t *lobj)
{
    gobj_t *gobj;

    gobj = lobj ? get_gobj(lobj) : NULL;
    if (!gobj)
        return -EINVAL;

    lv_obj_set_style_border_side(lobj, gobj->layout.border_side, 0);
    return 0;
}

/*
 * Set and immediately apply border side configuration for an object
 */
int32_t set_border_side(lv_obj_t *lobj, int32_t value)
{
    int32_t ret;

    ret = config_gobj_border_side(lobj, value);
    if (ret)
        return ret;

    return apply_border_side(lobj);
}
