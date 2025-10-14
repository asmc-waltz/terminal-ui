/**
 * @file padding.c
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
static inline int32_t config_meta_padding(lv_obj_t *lobj, int32_t pad_top, \
                                      int32_t pad_bot, int32_t pad_left, \
                                      int32_t pad_right)
{
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    meta->layout.pad_top = pad_top;
    meta->layout.pad_bot = pad_bot;
    meta->layout.pad_left = pad_left;
    meta->layout.pad_right = pad_right;

    return 0;
}

static inline int32_t config_meta_row_padding(lv_obj_t *lobj, int32_t pad)
{
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    meta->layout.pad_row = pad;

    return 0;
}

static inline int32_t config_meta_column_padding(lv_obj_t *lobj, int32_t pad)
{
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    meta->layout.pad_column = pad;

    return 0;
}

static inline int32_t apply_meta_row_padding(lv_obj_t *lobj)
{
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    lv_obj_set_style_pad_row(lobj, meta->layout.pad_row, 0);

    return 0;
}

static inline int32_t apply_meta_column_padding(lv_obj_t *lobj)
{
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    lv_obj_set_style_pad_column(lobj, meta->layout.pad_column, 0);

    return 0;
}
/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int32_t rotate_padding_90(lv_obj_t *lobj)
{
    obj_meta_t *meta;
    int32_t tmp_pad = 0;
    int32_t ret = 0;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!lobj)
        return -EINVAL;

    ret = config_meta_padding(lobj, \
                               meta->layout.pad_right, meta->layout.pad_left, \
                               meta->layout.pad_top, meta->layout.pad_bot);
    if (ret)
        return ret;


    tmp_pad = meta->layout.pad_row;
    ret = config_meta_row_padding(lobj, meta->layout.pad_column);
    if (ret)
        return ret;

    ret = config_meta_column_padding(lobj, tmp_pad);
    if (ret)
        return ret;

    return 0;
}

int32_t apply_padding(lv_obj_t *lobj)
{
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    lv_obj_set_style_pad_top(lobj, meta->layout.pad_top, 0);
    lv_obj_set_style_pad_bottom(lobj, meta->layout.pad_bot, 0);
    lv_obj_set_style_pad_left(lobj, meta->layout.pad_left, 0);
    lv_obj_set_style_pad_right(lobj, meta->layout.pad_right, 0);

    return 0;
}

int32_t apply_row_column_padding(lv_obj_t *lobj)
{
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    lv_obj_set_style_pad_row(lobj, meta->layout.pad_row, 0);
    lv_obj_set_style_pad_column(lobj, meta->layout.pad_column, 0);

    return 0;
}

int32_t set_padding(lv_obj_t *lobj, int32_t pad_top, int32_t pad_bot, \
                          int32_t pad_left, int32_t pad_right)
{
    int32_t ret;

    ret = config_meta_padding(lobj, pad_top, pad_bot, pad_left, pad_right);
    if (ret)
        return ret;

    return apply_padding(lobj);
}

int32_t set_row_padding(lv_obj_t *lobj, int32_t pad)
{
    int32_t ret;

    ret = config_meta_row_padding(lobj, pad);
    if (ret)
        return ret;

    return apply_meta_row_padding(lobj);
}

int32_t set_column_padding(lv_obj_t *lobj, int32_t pad)
{
    int32_t ret;

    ret = config_meta_column_padding(lobj, pad);
    if (ret)
        return ret;

    return apply_meta_column_padding(lobj);
}
