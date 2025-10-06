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
static void swap_w_h_size(gobj_t *gobj)
{
    int32_t tmp_w;
    int32_t tmp_par_w_pct;
    int32_t tmp_w_scale;

    tmp_w = gobj->size.w;
    tmp_par_w_pct = gobj->size.par_w_pct;
    tmp_w_scale = gobj->size.scale_w;

    gobj->size.w = gobj->size.h;
    gobj->size.par_w_pct = gobj->size.par_h_pct;
    gobj->size.scale_w = gobj->size.scale_h;

    gobj->size.h = tmp_w;
    gobj->size.par_h_pct = tmp_par_w_pct;
    gobj->size.scale_h = tmp_w_scale;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int32_t calc_gobj_rotated_size(gobj_t *gobj)
{
    int32_t scr_rot, cur_rot;
    int32_t rot_cnt;

    if (!gobj) {
        LOG_ERROR("Invalid g object");
        return -EINVAL;
    }

    scr_rot = get_scr_rotation();  /* returns 0-3 */
    cur_rot = gobj->data.rotation;        /* stored as 0-3 */

    if (cur_rot < ROTATION_0 || cur_rot > ROTATION_270 ||
        scr_rot < ROTATION_0 || scr_rot > ROTATION_270)
        return -EINVAL;

    rot_cnt = (scr_rot - cur_rot + 4) % 4;

    if (rot_cnt == 1 || rot_cnt == 3)
        swap_w_h_size(gobj);

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
void set_gobj_size(lv_obj_t *lobj, int32_t px_x, int32_t px_y)
{
    gobj_t *gobj = NULL;
    LV_ASSERT_NULL(lobj);

    gobj = get_gobj(lobj);
    gobj->size.w = px_x;
    gobj->size.h = px_y;
    gobj->size.par_w_pct = 0;
    gobj->size.par_h_pct = 0;
    gobj->size.scale_w = DIS_SCALE;
    gobj->size.scale_h = DIS_SCALE;

    apply_gobj_size(lobj);
}

/*
 * Set dynamic size for object based on parent size.
 * Only the Width is resized; Height remains default.
 */
void set_gobj_size_scale_w(lv_obj_t *lobj, int32_t pct_x, int32_t px_y)
{
    gobj_t *gobj = NULL;
    LV_ASSERT_NULL(lobj);

    gobj = get_gobj(lobj);
    LV_ASSERT_NULL(gobj);

    gobj->size.w = 0;
    gobj->size.h = px_y;
    gobj->size.par_w_pct = pct_x;
    gobj->size.par_h_pct = 0;
    gobj->size.scale_w = ENA_SCALE;
    gobj->size.scale_h = DIS_SCALE;

    apply_gobj_size(lobj);
}

/*
 * Set dynamic size for object based on parent size.
 * Only the Height is resized; Width remains default.
 */
void set_gobj_size_scale_h(lv_obj_t *lobj, int32_t px_x, int32_t pct_y)
{
    gobj_t *gobj = NULL;
    LV_ASSERT_NULL(lobj);

    gobj = get_gobj(lobj);
    LV_ASSERT_NULL(gobj);

    gobj->size.w = px_x;
    gobj->size.h = 0;
    gobj->size.par_w_pct = 0;
    gobj->size.par_h_pct = pct_y;
    gobj->size.scale_w = DIS_SCALE;
    gobj->size.scale_h = ENA_SCALE;

    apply_gobj_size(lobj);
}

/*
 * Set dynamic size for object based on parent size.
 * Both of Width and Height coordinate are resized.
 */
void set_gobj_size_scale(lv_obj_t *lobj, int32_t pct_x, int32_t pct_y)
{
    gobj_t *gobj = NULL;
    LV_ASSERT_NULL(lobj);

    gobj = get_gobj(lobj);
    LV_ASSERT_NULL(gobj);

    gobj->size.w = 0;
    gobj->size.h = 0;
    gobj->size.par_w_pct = pct_x;
    gobj->size.par_h_pct = pct_y;
    gobj->size.scale_w = ENA_SCALE;
    gobj->size.scale_h = ENA_SCALE;

    apply_gobj_size(lobj);
}

/*
 * Object size can be set by percent for scaling or by pixel for fixed size.
 * Since both percent and raw pixel values exist, they must be kept in sync
 * whenever size is updated or applied. This synchronization also helps sibling
 * objects calculate the remaining parent space more easily.
 */
void apply_gobj_size(lv_obj_t *lobj)
{
    gobj_t *gobj = NULL;

    LV_ASSERT_NULL(lobj);
    gobj = get_gobj(lobj);
    LV_ASSERT_NULL(gobj);

    if (gobj->size.scale_w == ENA_SCALE) {
        // percent to pixel
        gobj->size.w = pct_to_px(get_par_w(lobj), gobj->size.par_w_pct);
    } else {
        // Update object size in percent-based scaling
        // Pixel to percent
        gobj->size.par_w_pct =  px_to_pct(get_par_w(lobj), get_w(lobj));
        LOG_TRACE("Update obj W size [%d] -> percent [%d]", \
                  get_w(lobj), gobj->size.par_w_pct);
    }


    if (gobj->size.scale_h == ENA_SCALE) {
        // percent to pixel
        gobj->size.h = pct_to_px(get_par_h(lobj), gobj->size.par_h_pct);
    } else {
        // Update object size in percent-based scaling
        // Pixel to percent
        gobj->size.par_h_pct =  px_to_pct(get_par_h(lobj), get_h(lobj));
        LOG_TRACE("Update obj H size [%d] -> percent [%d]", \
                  get_h(lobj), gobj->size.par_h_pct);
    }

    lv_obj_set_size(lobj, gobj->size.w, gobj->size.h);
}

/*
 * Some objects, such as grid cells, have dynamic sizes depending on their
 * descriptor and alignment. When the descriptor or layout changes, the stored
 * size may become outdated, making it difficult for child objects to calculate
 * their own sizes relative to the cell.
 *
 * This utility retrieves the current object size after layout updates and stores
 * it into the gobj data in both percent and pixel units for later synchronization.
 */
int32_t store_computed_object_size(lv_obj_t *lobj)
{
    gobj_t *gobj;
    int32_t w, h;

    gobj = lobj ? get_gobj(lobj) : NULL;
    if (!gobj)
        return -EINVAL;

    if (gobj->data.obj_type == OBJ_BASE)
        return 0;

    w = lv_obj_get_width(lobj);
    h = lv_obj_get_height(lobj);

    if (w < 0 || h < 0) {
        LOG_ERROR("Object [%d] get invalid size W[%d] - H[%d]", \
                  get_name(lobj), w, h);
        return -EIO;
    }

    gobj->size.par_w_pct = px_to_pct(get_par_w(lobj), w);
    gobj->size.w = w;
    gobj->size.par_h_pct = px_to_pct(get_par_h(lobj), h);
    gobj->size.h = h;

    LOG_TRACE("Update object [%s] size\nParent Width [%d] - Height [%d]\n"\
             "Storaged size Width [%d or %d\%] - Height [%d or %d\%]", \
             gobj->name, \
             get_par_w(lobj), \
             get_par_h(lobj), \
             gobj->size.w, \
             gobj->size.par_w_pct, \
             gobj->size.h, \
             gobj->size.par_h_pct);

    return 0;
}
