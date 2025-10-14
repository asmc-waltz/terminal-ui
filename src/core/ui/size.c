/**
 * @file size.c
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

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int32_t rotate_size_meta_90(lv_obj_t *lobj)
{
    int32_t tmp_w;
    int32_t tmp_par_w_pct;
    int32_t tmp_w_scale;
    obj_meta_t *meta;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    tmp_w = meta->size.w;
    tmp_par_w_pct = meta->size.par_w_pct;
    tmp_w_scale = meta->size.scale_w;

    meta->size.w = meta->size.h;
    meta->size.par_w_pct = meta->size.par_h_pct;
    meta->size.scale_w = meta->size.scale_h;

    meta->size.h = tmp_w;
    meta->size.par_h_pct = tmp_par_w_pct;
    meta->size.scale_h = tmp_w_scale;

    return 0;
}

int32_t set_scr_size(int32_t width, int32_t height)
{
    ctx_t *ctx = get_ctx();
    if (ctx == NULL)
        return -EINVAL;

    ctx->scr.width = width;
    ctx->scr.height = height;

    return 0;
}

int32_t get_scr_width(void)
{
    ctx_t *ctx = get_ctx();
    if (ctx == NULL)
        return -EINVAL;

    return ctx->scr.width;
}

int32_t get_scr_height(void)
{
    ctx_t *ctx = get_ctx();
    if (ctx == NULL)
        return -EINVAL;

    return ctx->scr.height;
}

/*
 * Set fix size for object based on input values.
 */
void set_size(lv_obj_t *lobj, int32_t px_x, int32_t px_y)
{
    obj_meta_t *meta = NULL;
    LV_ASSERT_NULL(lobj);

    meta = get_meta(lobj);
    meta->size.w = px_x;
    meta->size.h = px_y;
    meta->size.par_w_pct = 0;
    meta->size.par_h_pct = 0;
    meta->size.scale_w = DIS_SCALE;
    meta->size.scale_h = DIS_SCALE;

    apply_size_meta(lobj);
}

/*
 * Set dynamic size for object based on parent size.
 * Only the Width is resized; Height remains default.
 */
void set_size_scale_w(lv_obj_t *lobj, int32_t pct_x, int32_t px_y)
{
    obj_meta_t *meta = NULL;
    LV_ASSERT_NULL(lobj);

    meta = get_meta(lobj);
    LV_ASSERT_NULL(meta);

    meta->size.w = 0;
    meta->size.h = px_y;
    meta->size.par_w_pct = pct_x;
    meta->size.par_h_pct = 0;
    meta->size.scale_w = ENA_SCALE;
    meta->size.scale_h = DIS_SCALE;

    apply_size_meta(lobj);
}

/*
 * Set dynamic size for object based on parent size.
 * Only the Height is resized; Width remains default.
 */
void set_size_scale_h(lv_obj_t *lobj, int32_t px_x, int32_t pct_y)
{
    obj_meta_t *meta = NULL;
    LV_ASSERT_NULL(lobj);

    meta = get_meta(lobj);
    LV_ASSERT_NULL(meta);

    meta->size.w = px_x;
    meta->size.h = 0;
    meta->size.par_w_pct = 0;
    meta->size.par_h_pct = pct_y;
    meta->size.scale_w = DIS_SCALE;
    meta->size.scale_h = ENA_SCALE;

    apply_size_meta(lobj);
}

/*
 * Set dynamic size for object based on parent size.
 * Both of Width and Height coordinate are resized.
 */
void set_size_scale(lv_obj_t *lobj, int32_t pct_x, int32_t pct_y)
{
    obj_meta_t *meta = NULL;
    LV_ASSERT_NULL(lobj);

    meta = get_meta(lobj);
    LV_ASSERT_NULL(meta);

    meta->size.w = 0;
    meta->size.h = 0;
    meta->size.par_w_pct = pct_x;
    meta->size.par_h_pct = pct_y;
    meta->size.scale_w = ENA_SCALE;
    meta->size.scale_h = ENA_SCALE;

    apply_size_meta(lobj);
}

/*
 * Object size can be set by percent for scaling or by pixel for fixed size.
 * Since both percent and raw pixel values exist, they must be kept in sync
 * whenever size is updated or applied. This synchronization also helps sibling
 * objects calculate the remaining parent space more easily.
 */
void apply_size_meta(lv_obj_t *lobj)
{
    obj_meta_t *meta = NULL;

    LV_ASSERT_NULL(lobj);
    meta = get_meta(lobj);
    LV_ASSERT_NULL(meta);

    if (meta->size.scale_w == ENA_SCALE) {
        // percent to pixel
        meta->size.w = pct_to_px(get_par_w(lobj), meta->size.par_w_pct);
    } else {
        // Update object size in percent-based scaling
        // Pixel to percent
        meta->size.par_w_pct =  px_to_pct(get_par_w(lobj), get_w(lobj));
        LOG_TRACE("Update obj W size [%d] -> percent [%d]", \
                  get_w(lobj), meta->size.par_w_pct);
    }


    if (meta->size.scale_h == ENA_SCALE) {
        // percent to pixel
        meta->size.h = pct_to_px(get_par_h(lobj), meta->size.par_h_pct);
    } else {
        // Update object size in percent-based scaling
        // Pixel to percent
        meta->size.par_h_pct =  px_to_pct(get_par_h(lobj), get_h(lobj));
        LOG_TRACE("Update obj H size [%d] -> percent [%d]", \
                  get_h(lobj), meta->size.par_h_pct);
    }

    lv_obj_set_size(lobj, meta->size.w, meta->size.h);
}

/*
 * Some objects, such as grid cells, have dynamic sizes depending on their
 * descriptor and alignment. When the descriptor or layout changes, the stored
 * size may become outdated, making it difficult for child objects to calculate
 * their own sizes relative to the cell.
 *
 * This utility retrieves the current object size after layout updates and stores
 * it into the meta data in both percent and pixel units for later synchronization.
 */
int32_t store_computed_size(lv_obj_t *lobj)
{
    obj_meta_t *meta;
    int32_t w, h;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    if (get_type(lobj) == OBJ_BASE)
        return 0;

    w = lv_obj_get_width(lobj);
    h = lv_obj_get_height(lobj);

    if (w < 0 || h < 0) {
        LOG_ERROR("Object [%d] get invalid size W[%d] - H[%d]", \
                  get_name(lobj), w, h);
        return -EIO;
    }

    meta->size.par_w_pct = px_to_pct(get_par_w(lobj), w);
    meta->size.w = w;
    meta->size.par_h_pct = px_to_pct(get_par_h(lobj), h);
    meta->size.h = h;

    LOG_TRACE("Update object [%s] size\nParent Width [%d] - Height [%d]\n"\
             "Storaged size Width [%d or %d\%] - Height [%d or %d\%]", \
             meta->name, \
             get_par_w(lobj), \
             get_par_h(lobj), \
             meta->size.w, \
             meta->size.par_w_pct, \
             meta->size.h, \
             meta->size.par_h_pct);

    return 0;
}
