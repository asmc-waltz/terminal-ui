/**
 * @file dynamic_creator.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
// #define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL defined locally will override the global setting in this file"
#endif
#include <log.h>

#include <stdint.h>
#include <errno.h>

#include <lvgl.h>

#include <ui/ui_plat.h>

/*********************
 *      DEFINES
 *********************/
#define HW_DISPLAY_WIDTH                1024
#define HW_DISPLAY_HEIGHT               600

#define ROT_0                           LV_DISPLAY_ROTATION_0
#define ROT_90                          LV_DISPLAY_ROTATION_90
#define ROT_180                         LV_DISPLAY_ROTATION_180
#define ROT_270                         LV_DISPLAY_ROTATION_270
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
static int32_t g_scr_rot_dir = ROT_0;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
// NOTE
// Location must be calculate after new size is calculated
static int32_t g_obj_get_new_xy_ofs(g_obj *pg_obj, int32_t par_w, int32_t par_h)
{
    int32_t real_x_ofs = 0;
    int32_t real_y_ofs = 0;
    int32_t scr_rot = g_get_scr_rot_dir();

    if (!pg_obj) {
        LOG_ERROR("Invalid g object");
        return -EINVAL;
    }

    if (scr_rot == pg_obj->inf.rot) {
        return 0;
    }

    if (pg_obj->inf.rot == ROT_0) {
        if (scr_rot == ROT_90) {
            real_x_ofs = par_w - (pg_obj->inf.w + pg_obj->inf.y_ofs);
            real_y_ofs = pg_obj->inf.x_ofs;
        } else if (scr_rot == ROT_180) {
            real_x_ofs = par_w - (pg_obj->inf.x_ofs + pg_obj->inf.w);
            real_y_ofs = par_h - (pg_obj->inf.y_ofs + pg_obj->inf.h);
        } else if (scr_rot == ROT_270) {
            real_x_ofs = pg_obj->inf.y_ofs;
            real_y_ofs = par_h - (pg_obj->inf.h + pg_obj->inf.x_ofs);
        }
    } else if (pg_obj->inf.rot == ROT_90) {
        if (scr_rot == ROT_0) {
            real_x_ofs = pg_obj->inf.y_ofs;
            real_y_ofs = par_w - (pg_obj->inf.x_ofs + pg_obj->inf.h);
        } else if (scr_rot == ROT_180) {
            real_x_ofs = par_w - (pg_obj->inf.y_ofs + pg_obj->inf.w);
            real_y_ofs = par_h - (par_w - pg_obj->inf.x_ofs);
        } else if (scr_rot == ROT_270) {
            real_x_ofs = par_w - (pg_obj->inf.x_ofs + pg_obj->inf.w);
            real_y_ofs = par_h - (pg_obj->inf.y_ofs + pg_obj->inf.h);
        }
    } else if (pg_obj->inf.rot == ROT_180) {
        if (scr_rot == ROT_0) {
            real_x_ofs = par_w - (pg_obj->inf.x_ofs + pg_obj->inf.w);
            real_y_ofs = par_h - (pg_obj->inf.y_ofs + pg_obj->inf.h);
        } else if (scr_rot == ROT_90) {
            real_x_ofs = par_w - (par_h - pg_obj->inf.y_ofs);
            real_y_ofs = par_w - (pg_obj->inf.x_ofs + pg_obj->inf.h);
        } else if (scr_rot == ROT_270) {
            real_x_ofs = par_h - (pg_obj->inf.y_ofs + pg_obj->inf.w);
            real_y_ofs = par_h - (par_w - pg_obj->inf.x_ofs);
        }
    } else if (pg_obj->inf.rot == ROT_270) {
        if (scr_rot == ROT_0) {
            real_x_ofs = par_h - (pg_obj->inf.y_ofs + pg_obj->inf.w);
            real_y_ofs = pg_obj->inf.x_ofs;
        } else if (scr_rot == ROT_90) {
            real_x_ofs = par_w - (pg_obj->inf.x_ofs + pg_obj->inf.w);
            real_y_ofs = par_h - (pg_obj->inf.y_ofs + pg_obj->inf.h);
        } else if (scr_rot == ROT_180) {
            real_x_ofs = par_w - (par_h - pg_obj->inf.y_ofs);
            real_y_ofs = par_h - (pg_obj->inf.x_ofs + pg_obj->inf.h);
        }
    }

    if (real_x_ofs >= 0) {
        pg_obj->inf.x_ofs = real_x_ofs;
    } else {
        LOG_ERROR("Relocation x=%d", real_x_ofs);
        return -1;
    }

    if (real_y_ofs >= 0) {
        pg_obj->inf.y_ofs = real_y_ofs;
    } else {
        LOG_ERROR("Relocation y=%d", real_y_ofs);
        return -1;
    }

    LOG_INFO("ROTATE %d, X=%d - Y=%d", scr_rot, real_x_ofs, real_y_ofs);

    return 0;
}


static int32_t g_obj_rot_relocation(g_obj *pg_obj)
{
    lv_obj_t *pl_obj_par = NULL;
    g_obj *pg_obj_par = NULL;

    if (!pg_obj) {
        LOG_ERROR("Invalid g object");
        return -EINVAL;
    }

    pl_obj_par = lv_obj_get_parent(pg_obj->obj);
    if (!pl_obj_par) {
        LOG_ERROR("Invalid lvgl object");
        return -EINVAL;
    }

    pg_obj_par = pl_obj_par->user_data;
    if (!pg_obj_par) {
        LOG_ERROR("Invalid g parent object");
        return -EINVAL;
    }

    g_obj_get_new_xy_ofs(pg_obj, pg_obj_par->inf.w, pg_obj_par->inf.h);

    lv_obj_set_pos(pg_obj->obj, pg_obj->inf.x_ofs, pg_obj->inf.y_ofs);
    return 0;
}

static void g_swap_xy_size(g_obj *pg_obj)
{
    int32_t tmp_width;

    tmp_width = pg_obj->inf.w;
    pg_obj->inf.w = pg_obj->inf.h;
    pg_obj->inf.h = tmp_width;
}

static int32_t g_obj_rot_resize(g_obj *pg_obj)
{
    int32_t scr_rot = g_get_scr_rot_dir();

    if (!pg_obj) {
        LOG_ERROR("Invalid g object");
        return -EINVAL;
    }

    // Must check: input rot is different from the current rotation inf

    if (scr_rot == ROT_0) {
        if (pg_obj->inf.rot == ROT_90 || pg_obj->inf.rot == ROT_270)
            g_swap_xy_size(pg_obj);
    } else if (scr_rot == ROT_90) {
        if (pg_obj->inf.rot == ROT_0 || pg_obj->inf.rot == ROT_180)
            g_swap_xy_size(pg_obj);
    } else if (scr_rot == ROT_180) {
        if (pg_obj->inf.rot == ROT_90 || pg_obj->inf.rot == ROT_270)
            g_swap_xy_size(pg_obj);
    } else if (scr_rot == ROT_270) {
        if (pg_obj->inf.rot == ROT_0 || pg_obj->inf.rot == ROT_180)
            g_swap_xy_size(pg_obj);
    }

    lv_obj_set_size(pg_obj->obj, pg_obj->inf.w, pg_obj->inf.h);
    return 0;
}


static int32_t g_obj_rotate(g_obj *pg_obj)
{
    int32_t ret;
    int32_t scr_rot = g_get_scr_rot_dir();

    if (!pg_obj) {
        LOG_ERROR("Invalid g object");
        return -EINVAL;
    }

    if (pg_obj->inf.rot == scr_rot) {
        LOG_TRACE("Object is rotated already");
        return 0;
    } else {
        LOG_TRACE("Object is rotating to %d", scr_rot);
    }

    // TODO: check obj type and update flex flow, scale...
    // Text, icon, switch will be rotate
    // Frame, button, slider will be resize and relocation

    ret = g_obj_rot_resize(pg_obj);
    if (ret) {
        return -EINVAL;
    }

    ret= g_obj_rot_relocation(pg_obj);
    if (ret) {
        return -EINVAL;
    }

    pg_obj->inf.rot = scr_rot;

    return 0;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
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

int32_t g_set_scr_rot_dir(int32_t rot_dir)
{
    g_scr_rot_dir = rot_dir;
    return 0;
}

int32_t g_get_scr_rot_dir()
{
    return g_scr_rot_dir;
}

int32_t g_relocation_all()
{
    // Retrieve current screen dimensions.
    // Recalculate object size, position, and layout based on rotation.
}

int32_t g_scale_all()
{
    // Retrieve current screen dimensions.
    // Recalculate object size, position, and layout based on rotation.
}

int32_t g_rotate_event_handler()
{
    int32_t ret;
    int32_t rot_dir;
    // e.g. from dbus
    // g_set_scr_rot_dir(LV_DISPLAY_ROTATION_0);
    g_set_scr_rot_dir(LV_DISPLAY_ROTATION_90);
    // g_set_scr_rot_dir(LV_DISPLAY_ROTATION_180);
    // g_set_scr_rot_dir(LV_DISPLAY_ROTATION_270);

    // Retrieve rotation state.
    rot_dir = g_get_scr_rot_dir();

    // Adjust screen size accordingly.
    if (rot_dir == LV_DISPLAY_ROTATION_0) {
        g_set_scr_size(1024, 600);
    } else if (rot_dir == LV_DISPLAY_ROTATION_90) {
        g_set_scr_size(600, 1024);
    } else if (rot_dir == LV_DISPLAY_ROTATION_180) {
        g_set_scr_size(1024, 600);
    } else if (rot_dir == LV_DISPLAY_ROTATION_270) {
        g_set_scr_size(600, 1024);
    }

    // Invoke scale event handler.
    ret = g_scale_all();
    if (ret) {
        LOG_ERROR("Scale screen size failed");
    }

    // Invoke relocation event handler.
    ret = g_relocation_all();
    if (ret) {
        LOG_ERROR("Relocation object on screen failed");
    }

    // Invalidate layers to refresh UI.
}

