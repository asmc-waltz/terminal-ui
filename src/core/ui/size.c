/**
 * @file size.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL defined locally will override the global setting in this file"
#endif
#include <log.h>

#include <stdint.h>
#include <errno.h>

#include <lvgl.h>

#include <list.h>
#include <ui/ui_plat.h>

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
static int32_t g_scr_width = HW_DISPLAY_WIDTH;
static int32_t g_scr_hight = HW_DISPLAY_HEIGHT;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void g_swap_xy_size(g_obj *gobj)
{
    int32_t tmp_width;

    tmp_width = gobj->pos.w;
    gobj->pos.w = gobj->pos.h;
    gobj->pos.h = tmp_width;
}


/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int32_t g_obj_rot_calc_size(g_obj *gobj)
{
    int32_t scr_rot = g_get_scr_rot_dir();

    if (!gobj) {
        LOG_ERROR("Invalid g object");
        return -EINVAL;
    }

    // Must check: input rot is different from the current rotation inf

    if (scr_rot == ROTATION_0) {
        if (gobj->pos.rot == ROTATION_90 || gobj->pos.rot == ROTATION_270)
            g_swap_xy_size(gobj);
    } else if (scr_rot == ROTATION_90) {
        if (gobj->pos.rot == ROTATION_0 || gobj->pos.rot == ROTATION_180)
            g_swap_xy_size(gobj);
    } else if (scr_rot == ROTATION_180) {
        if (gobj->pos.rot == ROTATION_90 || gobj->pos.rot == ROTATION_270)
            g_swap_xy_size(gobj);
    } else if (scr_rot == ROTATION_270) {
        if (gobj->pos.rot == ROTATION_0 || gobj->pos.rot == ROTATION_180)
            g_swap_xy_size(gobj);
    }

    return 0;
}

int32_t g_set_scr_size(int32_t width, int32_t hight)
{
    // TODO: lock
    g_scr_hight = hight;
    g_scr_width = width;
    return 0;
}

int32_t g_get_scr_width(int32_t width, int32_t hight)
{
    return g_scr_width;
}

int32_t g_get_scr_hight()
{
    return g_scr_hight;
}

void gf_gobj_set_size(lv_obj_t *lobj, int32_t w, int32_t h)
{
    g_obj *gobj = NULL;
    LV_ASSERT_NULL(lobj);

    gobj = lobj->user_data;
    gobj->pos.w = w;
    gobj->pos.h = h;

    lv_obj_set_size(lobj, gobj->pos.w, gobj->pos.h);
}

void gf_gobj_get_size(lv_obj_t *lobj)
{
    g_obj *gobj = NULL;
    LV_ASSERT_NULL(lobj);

    lv_obj_update_layout(lobj);
    gobj = lobj->user_data;
    gobj->pos.w = lv_obj_get_width(lobj);
    gobj->pos.h = lv_obj_get_height(lobj);
}

