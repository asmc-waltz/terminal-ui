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
    int32_t tmp_w_scale;

    tmp_w = gobj->size.w;
    tmp_w_scale = gobj->size.scale_w;

    gobj->size.w = gobj->size.h;
    gobj->size.scale_w = gobj->size.scale_h;

    gobj->size.h = tmp_w;
    gobj->size.scale_h = tmp_w_scale;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int32_t scale_gobj(gobj_t *gobj)
{
    gobj_t *gobj_par = NULL;
    int32_t scale_w, scale_h;
    int32_t abs_val_x, abs_val_y;
    int8_t scr_rot;
    int32_t par_w, par_h;

    if (!gobj) {
        LOG_ERROR("Invalid g object");
        return -EINVAL;
    }

    gobj_par = gobj->par;
    if (!gobj_par) {
        LOG_ERROR("Invalid g parent object");
        return -EINVAL;
    }

    if (gobj_par->scale.ena_w || gobj_par->scale.ena_h) {
        par_w = gobj_par->scale.w;
        par_h = gobj_par->scale.h;
    } else {
        par_w = gobj_par->size.w;
        par_h = gobj_par->size.h;
    }

    /*
     * TODO:
     * This logic works when alignment is based on the parent,
     * but it will fail if aligned to another object.
     */
    abs_val_x = abs(gobj->aln.x);
    abs_val_y = abs(gobj->aln.y);
    scr_rot = get_scr_rotation();  /* returns 0-3 */
    scale_w = gobj->size.w;
    scale_h = gobj->size.h;

    if (gobj->scale.ena_h) {
        if (scr_rot == ROTATION_0 || scr_rot == ROTATION_180) {
            scale_h = par_h - abs_val_y - calc_pixels(par_h, gobj->scale.pad_h);
        } else if (scr_rot == ROTATION_270 || scr_rot == ROTATION_90) {
            scale_w = par_w - abs_val_x - calc_pixels(par_w, gobj->scale.pad_h);
        }
    }

    if (gobj->scale.ena_w) {
        if (scr_rot == ROTATION_0 || scr_rot == ROTATION_180) {
            scale_w = par_w - abs_val_x - calc_pixels(par_w, gobj->scale.pad_w);
        } else if (scr_rot == ROTATION_270 || scr_rot == ROTATION_90) {
            scale_h = par_h - abs_val_y - calc_pixels(par_h, gobj->scale.pad_w);
        }
    }

    gobj->scale.h = scale_h;
    gobj->scale.w = scale_w;

    LOG_TRACE("Parent: w=%d h=%d - ABS align x=%d y=%d - Pading w=%d h=%d\n" \
              "\tObj: scale: w->%d h->%d", \
              gobj_par->size.w, gobj_par->size.h, abs_val_x, abs_val_y, \
              gobj->scale.pad_w, gobj->scale.pad_h, gobj->scale.w, gobj->scale.h);

    return 0;
}
int32_t calc_gobj_rotated_size(gobj_t *gobj)
{
    int32_t scr_rot, cur_rot;
    int32_t rot_cnt;

    if (!gobj) {
        LOG_ERROR("Invalid g object");
        return -EINVAL;
    }

    scr_rot = get_scr_rotation();  /* returns 0-3 */
    cur_rot = gobj->rotation;        /* stored as 0-3 */

    if (cur_rot < ROTATION_0 || cur_rot > ROTATION_270 ||
        scr_rot < ROTATION_0 || scr_rot > ROTATION_270)
        return -EINVAL;

    rot_cnt = (scr_rot - cur_rot + 4) % 4;

    if (rot_cnt == 1 || rot_cnt == 3)
        swap_w_h_size(gobj);

    if (gobj->scale.ena_h || gobj->scale.ena_w) {
        if (scale_gobj(gobj))
            return -EINVAL;
    }
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

    gobj = lobj->user_data;
    gobj->size.w = px_x;
    gobj->size.h = px_y;
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

    gobj->size.w = pct_x;
    gobj->size.h = px_y;
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
    gobj->size.h = pct_y;
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

    gobj->size.w = pct_x;
    gobj->size.h = pct_y;
    gobj->size.scale_w = ENA_SCALE;
    gobj->size.scale_h = ENA_SCALE;

    apply_gobj_size(lobj);
}

void apply_gobj_size(lv_obj_t *lobj)
{
    gobj_t *gobj = NULL;
    int32_t px_x = 0;
    int32_t px_y = 0;

    LV_ASSERT_NULL(lobj);
    gobj = get_gobj(lobj);
    LV_ASSERT_NULL(gobj);

    if (gobj->size.scale_w == ENA_SCALE)
        px_x = calc_pixels(obj_width((gobj->par)->obj), gobj->size.w);
    else
        px_x = gobj->size.w;

    if (gobj->size.scale_h == ENA_SCALE)
        px_y = calc_pixels(obj_height((gobj->par)->obj), gobj->size.h);
    else
        px_y = gobj->size.h;

    lv_obj_set_size(lobj, px_x, px_y);
}

void gobj_get_size(lv_obj_t *lobj)
{
    gobj_t *gobj = NULL;
    LV_ASSERT_NULL(lobj);

    lv_obj_update_layout(lobj);
    gobj = lobj->user_data;
    gobj->size.w = lv_obj_get_width(lobj);
    gobj->size.h = lv_obj_get_height(lobj);
    gobj->size.scale_w = DIS_SCALE;
    gobj->size.scale_h = DIS_SCALE;
}

/*
 * For object expansion. Since gobject is based on lvgl object but supports
 * logical rotation, we must recalculate position and size for each rotation.
 * This depends on the parent size. Rotation causes layout changes as rescale.
 * Scaling must align based on the bottom and right edges, while normal objects
 * mostly align based on the top-left corner. This may affect objects aligned
 * differently. Carefully verify any object not aligned by top-left corner, as
 * they must still support size expansion.
 */
int32_t enable_scale_w(lv_obj_t *lobj)
{
    gobj_t *gobj;

    if (!lobj->user_data)
        return -1;

    gobj = lobj->user_data;
    gobj->scale.ena_w = 1;

    return 0;
}

int32_t enable_scale_h(lv_obj_t *lobj)
{
    gobj_t *gobj;

    if (!lobj->user_data)
        return -1;

    gobj = lobj->user_data;
    gobj->scale.ena_h = 1;

    return 0;
}

int32_t disable_scale_w(lv_obj_t *lobj)
{
    gobj_t *gobj;

    if (!lobj->user_data)
        return -1;

    gobj = lobj->user_data;
    gobj->scale.ena_w = 0;

    return 0;
}

int32_t disable_scale_h(lv_obj_t *lobj)
{
    gobj_t *gobj;

    if (!lobj->user_data)
        return -1;

    gobj = lobj->user_data;
    gobj->scale.ena_h = 0;

    return 0;
}

int32_t set_obj_scale_pad_w(lv_obj_t *lobj, int32_t pad_w)
{
    gobj_t *gobj;

    if (!lobj->user_data)
        return -1;

    gobj = lobj->user_data;
    gobj->scale.pad_w = pad_w;

    return 0;
}

int32_t set_obj_scale_pad_h(lv_obj_t *lobj, int32_t pad_h)
{
    gobj_t *gobj;

    if (!lobj->user_data)
        return -1;

    gobj = lobj->user_data;
    gobj->scale.pad_h = pad_h;

    return 0;
}

