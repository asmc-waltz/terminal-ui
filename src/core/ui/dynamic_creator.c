/**
 * @file dynamic_creator.c
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

#include <ui/ui_plat.h>
#include <ui/fonts.h>

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

// Location must be calculate after new size is calculated
static int32_t g_obj_get_center(g_obj *pg_obj, int32_t par_w, int32_t par_h)
{
    int32_t new_x_mid = 0;
    int32_t new_y_mid = 0;
    int32_t scr_rot = g_get_scr_rot_dir();

    if (!pg_obj) {
        LOG_ERROR("Invalid g object");
        return -EINVAL;
    }

    if (scr_rot == pg_obj->inf.rot) {
        return 0;
    }

    LOG_INFO("ROTATE [%d]: par-w=(%d) par-h=(%d)", \
             scr_rot, par_w, par_h);

    if (pg_obj->inf.rot == ROT_0) {
        if (scr_rot == ROT_90) {
            new_x_mid = par_w - pg_obj->inf.y_mid;
            new_y_mid = pg_obj->inf.x_mid;
        } else if (scr_rot == ROT_180) {
            new_x_mid = par_w - pg_obj->inf.x_mid;
            new_y_mid = par_h - pg_obj->inf.y_mid;
        } else if (scr_rot == ROT_270) {
            new_x_mid = pg_obj->inf.y_mid;
            new_y_mid = par_h - pg_obj->inf.x_mid;
        }
    } else if (pg_obj->inf.rot == ROT_90) {
        if (scr_rot == ROT_0) {
            new_x_mid = pg_obj->inf.y_mid;
            new_y_mid = par_w - pg_obj->inf.x_mid;
        } else if (scr_rot == ROT_180) {
            new_x_mid = par_w - pg_obj->inf.y_mid;
            new_y_mid = par_h - (par_w - pg_obj->inf.x_mid);
        } else if (scr_rot == ROT_270) {
            new_x_mid = par_w - pg_obj->inf.x_mid;
            new_y_mid = par_h - pg_obj->inf.y_mid;
        }
    } else if (pg_obj->inf.rot == ROT_180) {
        if (scr_rot == ROT_0) {
            new_x_mid = par_w - pg_obj->inf.x_mid;
            new_y_mid = par_h - pg_obj->inf.y_mid;
        } else if (scr_rot == ROT_90) {
            new_x_mid = par_w - (par_h - pg_obj->inf.y_mid);
            new_y_mid = par_w - pg_obj->inf.x_mid;
        } else if (scr_rot == ROT_270) {
            new_x_mid = par_h - pg_obj->inf.y_mid;
            new_y_mid = par_h - (par_w - pg_obj->inf.x_mid);
        }
    } else if (pg_obj->inf.rot == ROT_270) {
        if (scr_rot == ROT_0) {
            new_x_mid = par_h - pg_obj->inf.y_mid;
            new_y_mid = pg_obj->inf.x_mid;
        } else if (scr_rot == ROT_90) {
            new_x_mid = par_w - pg_obj->inf.x_mid;
            new_y_mid = par_h - pg_obj->inf.y_mid;
        } else if (scr_rot == ROT_180) {
            new_x_mid = par_w - (par_h - pg_obj->inf.y_mid);
            new_y_mid = par_h - pg_obj->inf.x_mid;
        }
    }

    if (new_x_mid >= 0) {
        pg_obj->inf.x_mid = new_x_mid;
    } else {
        LOG_ERROR("Relocation x_mid=%d", new_x_mid);
        return -1;
    }

    if (new_y_mid >= 0) {
        pg_obj->inf.y_mid = new_y_mid;
    } else {
        LOG_ERROR("Relocation y_mid=%d", new_y_mid);
        return -1;
    }

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

    g_obj_get_center(pg_obj, pg_obj_par->inf.w, pg_obj_par->inf.h);

    return 0;
}

static void g_swap_xy_size(g_obj *pg_obj)
{
    int32_t tmp_width;

    tmp_width = pg_obj->inf.w;
    pg_obj->inf.w = pg_obj->inf.h;
    pg_obj->inf.h = tmp_width;
}

static int32_t g_obj_rot_resize(g_obj *pg_obj, int8_t set)
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

    if (set)
        lv_obj_set_size(pg_obj->obj, pg_obj->inf.w, pg_obj->inf.h);
    return 0;
}

static int32_t g_base_obj_rotate(g_obj *pg_obj)
{
    int32_t ret;

    ret = g_obj_rot_resize(pg_obj, 1);
    if (ret) {
        return -EINVAL;
    }

    ret= g_obj_rot_relocation(pg_obj);
    if (ret) {
        return -EINVAL;
    }

    lv_obj_set_pos(pg_obj->obj, pg_obj->inf.x_mid - (pg_obj->inf.w / 2), \
                   pg_obj->inf.y_mid - (pg_obj->inf.h / 2));

    return 0;
}

static int32_t g_transform_obj_rotate(g_obj *pg_obj)
{
    int32_t ret;
    int32_t scr_rot = g_get_scr_rot_dir();
    int32_t rot_val = 0;

    ret = g_obj_rot_resize(pg_obj, 0);
    if (ret) {
        return -EINVAL;
    }

    ret= g_obj_rot_relocation(pg_obj);
    if (ret) {
        return -EINVAL;
    }

    if (scr_rot == LV_DISPLAY_ROTATION_0) {
        rot_val = 0;
        lv_obj_set_style_transform_rotation(pg_obj->obj, rot_val, 0);
        lv_obj_set_pos(pg_obj->obj, pg_obj->inf.x_mid - (pg_obj->inf.w / 2), \
                       pg_obj->inf.y_mid - (pg_obj->inf.h / 2));
    } else if (scr_rot == LV_DISPLAY_ROTATION_90) {
        rot_val = 900;
        lv_obj_set_style_transform_rotation(pg_obj->obj, rot_val, 0);
        lv_obj_set_pos(pg_obj->obj, pg_obj->inf.x_mid + (pg_obj->inf.w / 2), \
                       pg_obj->inf.y_mid - (pg_obj->inf.h / 2));
    } else if (scr_rot == LV_DISPLAY_ROTATION_180) {
        rot_val = 1800;
        lv_obj_set_style_transform_rotation(pg_obj->obj, rot_val, 0);
        lv_obj_set_pos(pg_obj->obj, pg_obj->inf.x_mid + (pg_obj->inf.w / 2), \
                       pg_obj->inf.y_mid + (pg_obj->inf.h / 2));
    } else if (scr_rot == LV_DISPLAY_ROTATION_270) {
        rot_val = 2700;
        lv_obj_set_style_transform_rotation(pg_obj->obj, rot_val, 0);
        lv_obj_set_pos(pg_obj->obj, pg_obj->inf.x_mid - (pg_obj->inf.w / 2), \
                       pg_obj->inf.y_mid + (pg_obj->inf.h / 2));
    }

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
    switch (pg_obj->inf.type) {
        case OBJ_BASE:
        case OBJ_BTN:
        case OBJ_SLIDER:
            ret = g_base_obj_rotate(pg_obj);
            break;
        case OBJ_LABEL:
        case OBJ_SWITCH:
        case OBJ_ICON:
            ret = g_transform_obj_rotate(pg_obj);
            break;
        default:
            LOG_WARN("Unknown G object type");
            break;
    }

    if (ret) {
        return ret;
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

lv_obj_t *gf_create_gobj_type(lv_obj_t *par, int32_t type, uint32_t id)
{
    g_obj *pg_obj = NULL;
    lv_obj_t *pl_obj = NULL;

    LV_ASSERT_NULL(par);
    LOG_TRACE("Create obj id %d", id);

    if (type == OBJ_BASE) {
        pl_obj = lv_obj_create(par);
    } else if (type == OBJ_BTN) {
        pl_obj = lv_btn_create(par);
    } else if (type == OBJ_SLIDER) {
        pl_obj = lv_slider_create(par);
    } else if (type == OBJ_LABEL) {
        pl_obj = lv_label_create(par);
    } else if (type == OBJ_ICON) {
        // TODO: ICON is the combination of BASE & LABEL
        pl_obj = lv_label_create(par);
    } else if (type == OBJ_SWITCH) {
        pl_obj = lv_switch_create(par);
    } else {
        LOG_TRACE("G Object type %d - id %d invalid", type, id);
        return NULL;
    }

    LV_ASSERT_NULL(pl_obj);

    pg_obj = gf_register_obj(par, pl_obj, id);
    pg_obj->inf.type = type;
    pg_obj->inf.rot = ROT_0;

    LV_ASSERT_NULL(pg_obj);

    return pg_obj->obj;
}

void gf_gobj_set_size(lv_obj_t *pl_obj, int32_t w, int32_t h)
{
    g_obj *pg_obj = NULL;
    LV_ASSERT_NULL(pl_obj);

    pg_obj = pl_obj->user_data;
    pg_obj->inf.w = w;
    pg_obj->inf.h = h;

    lv_obj_set_size(pl_obj, pg_obj->inf.w, pg_obj->inf.h);
}

void gf_gobj_get_size(lv_obj_t *pl_obj)
{
    g_obj *pg_obj = NULL;
    LV_ASSERT_NULL(pl_obj);

    lv_obj_update_layout(pl_obj);
    pg_obj = pl_obj->user_data;
    pg_obj->inf.w = lv_obj_get_width(pl_obj);
    pg_obj->inf.h = lv_obj_get_height(pl_obj);
}

void gf_gobj_set_pos(lv_obj_t *pl_obj, int32_t x_ofs, int32_t y_ofs)
{
    g_obj *pg_obj = NULL;
    LV_ASSERT_NULL(pl_obj);

    lv_obj_set_pos(pl_obj, x_ofs, y_ofs);

    pg_obj = pl_obj->user_data;
    if (!pg_obj->inf.w)
        LOG_WARN("Cannot calculate the center x");
    if (!pg_obj->inf.h)
        LOG_WARN("Cannot calculate the center y");
    pg_obj->inf.x_mid = x_ofs + (pg_obj->inf.w / 2);
    pg_obj->inf.y_mid = y_ofs + (pg_obj->inf.h / 2);
}

lv_obj_t * gf_create_box(lv_obj_t *par, uint32_t id, int32_t x, int32_t y, \
                         uint32_t w, uint32_t h, lv_color_t color)
{
    lv_obj_t *pl_obj = gf_create_gobj_type(par, OBJ_BASE, id);

    gf_gobj_set_size(pl_obj, w, h);
    gf_gobj_set_pos(pl_obj, x, y);

    lv_obj_set_style_pad_all(pl_obj, 0, 0);
    lv_obj_set_style_pad_gap(pl_obj, 0, 0);

    lv_obj_set_style_bg_color(pl_obj, color, 0);
    return pl_obj;
}

lv_obj_t * gf_create_textbox(lv_obj_t *par, uint32_t id, int32_t x, int32_t y, \
                             const char *txt_str)
{
    int32_t w, h;
    lv_obj_t *lbl = gf_create_gobj_type(par, OBJ_LABEL, id);

    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_18, 0);
    lv_label_set_text_fmt(lbl, "%s", txt_str);

    lv_obj_update_layout(lbl);
    w = lv_obj_get_width(lbl);
    h = lv_obj_get_height(lbl);
    gf_gobj_set_size(lbl, w, h);
    gf_gobj_set_pos(lbl, x, y);

    LOG_TRACE("Textbox is created: h=%d w=%d - x=%d y=%d", h, w, x, y);
    return lbl;
}

lv_obj_t * gf_create_switch(lv_obj_t *par, uint32_t id, int32_t x, int32_t y, \
                         uint32_t w, uint32_t h)
{
    lv_obj_t *sw = gf_create_gobj_type(par, OBJ_SWITCH, id);

    gf_gobj_set_size(sw, w, h);
    gf_gobj_set_pos(sw, x, y);

    LOG_TRACE("Switch is created: h=%d w=%d - x=%d y=%d", h, w, x, y);
    return sw;
}

lv_obj_t * gf_create_sym(lv_obj_t *par, uint32_t id, int32_t x, int32_t y, \
                         const lv_font_t *font, const char *index, \
                         lv_color_t color)
{
    lv_obj_t *symbol = gf_create_gobj_type(par, OBJ_ICON, id);

    lv_obj_set_style_text_font(symbol, font, 0);
    lv_obj_set_style_text_color(symbol, color, 0);
    lv_label_set_text(symbol, index);

    gf_gobj_get_size(symbol);
    gf_gobj_set_pos(symbol, x, y);

    return symbol;
}

lv_obj_t * gf_create_btn(lv_obj_t *par, uint32_t id, int32_t x, int32_t y, \
                         uint32_t w, uint32_t h)
{
    lv_obj_t *btn = gf_create_gobj_type(par, OBJ_BTN, id);
    gf_gobj_set_size(btn, w, h);
    gf_gobj_set_pos(btn, x, y);

    return btn;
}

lv_obj_t * gf_create_slider(lv_obj_t *par, uint32_t id, int32_t x, int32_t y, \
                         uint32_t w, uint32_t h)
{
    lv_obj_t *slider = gf_create_gobj_type(par, OBJ_SLIDER, id);

    gf_gobj_set_size(slider, w, h);
    gf_gobj_set_pos(slider, x, y);

    return slider;
}
/*
 * TESTING *********************************************************************
 */
void create_dynamic_ui()
{
    lv_obj_t *pl_main_box = NULL;
    lv_obj_t *pl_child_box = NULL;
    lv_obj_t *pl_text_box = NULL;
    lv_obj_t *pl_switch = NULL;
    lv_obj_t *pl_icon = NULL;
    lv_obj_t *pl_btn = NULL;
    lv_obj_t *pl_slider = NULL;

    pl_main_box = gf_create_box(lv_screen_active(), 0, 0, 0, 1024, 600, \
                                lv_color_hex(0x000000));
    pl_child_box = gf_create_box(pl_main_box, 0, 32, 51, 300, 200, \
                                 lv_color_hex(0xFFFFFF));
    pl_text_box = gf_create_textbox(pl_child_box, 0, 15, 25, "Go001 hahaha");

    pl_switch = gf_create_switch(pl_child_box, 0, 10, 55, 60, 30);

    pl_icon = gf_create_sym(pl_child_box, 0, 10, 100, &terminal_icons_32, \
                            ICON_TOOLBOX_SOLID, lv_color_hex(0xFFFF00));
    pl_btn = gf_create_btn(pl_child_box, 0, 40, 150, 80, 50);

    pl_slider = gf_create_slider(pl_child_box, 0, 100, 110, 150, 20);

    g_set_scr_rot_dir(LV_DISPLAY_ROTATION_90);
    g_obj_rotate(pl_child_box->user_data);
    g_obj_rotate(pl_text_box->user_data);
    g_obj_rotate(pl_switch->user_data);
    g_obj_rotate(pl_icon->user_data);
    g_obj_rotate(pl_btn->user_data);
    g_obj_rotate(pl_slider->user_data);

    LOG_INFO("LBL after rotate H=%d W=%d", ((g_obj *)(pl_text_box->user_data))->inf.h, \
             ((g_obj *)(pl_text_box->user_data))->inf.w);

    lv_obj_clear_flag(pl_main_box, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(pl_child_box, LV_OBJ_FLAG_SCROLLABLE);
}

