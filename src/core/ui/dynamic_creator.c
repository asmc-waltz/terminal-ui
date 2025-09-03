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

#define TEST 1 
#if defined(TEST)
#include <time.h>
#endif

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
static int32_t g_obj_get_center(g_obj *gobj, int32_t par_w, int32_t par_h)
{
    int32_t new_x_mid = 0;
    int32_t new_y_mid = 0;
    int32_t scr_rot = g_get_scr_rot_dir();

    if (!gobj) {
        LOG_ERROR("Invalid g object");
        return -EINVAL;
    }

    if (scr_rot == gobj->pos.rot) {
        return 0;
    }

    LOG_DEBUG("\tLastest: par_h: %d par_w: %d \n"
              "\t\tCurrent: par_h:%d par_w:%d \n"
              "\t\tCurrent: x_mid:%d y_mid:%d", \
              par_h, par_w, \
              gobj->pos.par_h, gobj->pos.par_w,
              gobj->pos.x_mid, gobj->pos.y_mid);

    if (gobj->pos.rot == ROT_0) {
        if (scr_rot == ROT_90) {
            new_x_mid = par_w - gobj->pos.y_mid;
            new_y_mid = gobj->pos.x_mid;
        } else if (scr_rot == ROT_180) {
            new_x_mid = par_w - gobj->pos.x_mid;
            new_y_mid = par_h - gobj->pos.y_mid;
        } else if (scr_rot == ROT_270) {
            new_x_mid = gobj->pos.y_mid;
            new_y_mid = par_h - gobj->pos.x_mid;
        }
    } else if (gobj->pos.rot == ROT_90) {
        if (scr_rot == ROT_0) {
            new_x_mid = gobj->pos.y_mid;
            new_y_mid = gobj->pos.par_w - gobj->pos.x_mid;
        } else if (scr_rot == ROT_180) {
            new_x_mid = par_w - gobj->pos.y_mid;
            new_y_mid = par_h - (gobj->pos.par_w - gobj->pos.x_mid);
        } else if (scr_rot == ROT_270) {
            new_x_mid = par_w - gobj->pos.x_mid;
            new_y_mid = par_h - gobj->pos.y_mid;
        }
    } else if (gobj->pos.rot == ROT_180) {
        if (scr_rot == ROT_0) {
            new_x_mid = par_w - gobj->pos.x_mid;
            new_y_mid = par_h - gobj->pos.y_mid;
        } else if (scr_rot == ROT_90) {
            new_x_mid = par_w - (gobj->pos.par_h - gobj->pos.y_mid);
            new_y_mid = gobj->pos.par_w - gobj->pos.x_mid;
        } else if (scr_rot == ROT_270) {
            new_x_mid = gobj->pos.par_h - gobj->pos.y_mid;
            new_y_mid = par_h - (gobj->pos.par_w - gobj->pos.x_mid);
        }
    } else if (gobj->pos.rot == ROT_270) {
        if (scr_rot == ROT_0) {
            new_x_mid = gobj->pos.par_h - gobj->pos.y_mid;
            new_y_mid = gobj->pos.x_mid;
        } else if (scr_rot == ROT_90) {
            new_x_mid = par_w - gobj->pos.x_mid;
            new_y_mid = par_h - gobj->pos.y_mid;
        } else if (scr_rot == ROT_180) {
            new_x_mid = par_w - (gobj->pos.par_h - gobj->pos.y_mid);
            new_y_mid = par_h - gobj->pos.x_mid;
        }
    }

    gobj->pos.x_mid = new_x_mid;
    gobj->pos.par_w = par_w;
    if (new_x_mid <= 0) {
        LOG_WARN("Negative x_mid: %d", new_x_mid);
    }

    gobj->pos.y_mid = new_y_mid;
    gobj->pos.par_h = par_h;
    if (new_y_mid <= 0) {
        LOG_WARN("* Negative y_mid: %d", new_y_mid);
        return -1;
    }
    LOG_TRACE("Relocation x_mid: %d  -  y_mid: %d", new_x_mid, new_y_mid);

    return 0;
}

static int32_t g_obj_rot_calc_center(g_obj *gobj)
{
    lv_obj_t *lobj_par = NULL;
    g_obj *gobj_par = NULL;

    if (!gobj) {
        LOG_ERROR("Invalid g object");
        return -EINVAL;
    }

    lobj_par = lv_obj_get_parent(gobj->obj);
    if (!lobj_par) {
        LOG_ERROR("Invalid lvgl object");
        return -EINVAL;
    }

    gobj_par = lobj_par->user_data;
    if (!gobj_par) {
        LOG_ERROR("Invalid g parent object");
        return -EINVAL;
    }

    g_obj_get_center(gobj, gobj_par->pos.w, gobj_par->pos.h);

    return 0;
}

static void g_swap_xy_size(g_obj *gobj)
{
    int32_t tmp_width;

    tmp_width = gobj->pos.w;
    gobj->pos.w = gobj->pos.h;
    gobj->pos.h = tmp_width;
}

static int32_t g_obj_rot_calc_size(g_obj *gobj)
{
    int32_t scr_rot = g_get_scr_rot_dir();

    if (!gobj) {
        LOG_ERROR("Invalid g object");
        return -EINVAL;
    }

    // Must check: input rot is different from the current rotation inf

    if (scr_rot == ROT_0) {
        if (gobj->pos.rot == ROT_90 || gobj->pos.rot == ROT_270)
            g_swap_xy_size(gobj);
    } else if (scr_rot == ROT_90) {
        if (gobj->pos.rot == ROT_0 || gobj->pos.rot == ROT_180)
            g_swap_xy_size(gobj);
    } else if (scr_rot == ROT_180) {
        if (gobj->pos.rot == ROT_90 || gobj->pos.rot == ROT_270)
            g_swap_xy_size(gobj);
    } else if (scr_rot == ROT_270) {
        if (gobj->pos.rot == ROT_0 || gobj->pos.rot == ROT_180)
            g_swap_xy_size(gobj);
    }

    return 0;
}

static void g_obj_rot_90_set_aln(g_obj *gobj)
{
    int8_t align = LV_ALIGN_DEFAULT;

    switch (gobj->aln.align) {
        case LV_ALIGN_TOP_LEFT:         align = LV_ALIGN_TOP_RIGHT; break;
        case LV_ALIGN_TOP_MID:          align = LV_ALIGN_RIGHT_MID; break;
        case LV_ALIGN_TOP_RIGHT:        align = LV_ALIGN_BOTTOM_RIGHT; break;
        case LV_ALIGN_LEFT_MID:         align = LV_ALIGN_TOP_MID; break;
        case LV_ALIGN_CENTER:           align = LV_ALIGN_CENTER; break;
        case LV_ALIGN_RIGHT_MID:        align = LV_ALIGN_BOTTOM_MID; break;
        case LV_ALIGN_BOTTOM_LEFT:      align = LV_ALIGN_TOP_LEFT; break;
        case LV_ALIGN_BOTTOM_MID:       align = LV_ALIGN_LEFT_MID; break;
        case LV_ALIGN_BOTTOM_RIGHT:     align = LV_ALIGN_BOTTOM_LEFT; break;

        /* Outside aligns on TOP side -> RIGHT side */
        case LV_ALIGN_OUT_TOP_LEFT:     align = LV_ALIGN_OUT_RIGHT_TOP; break;
        case LV_ALIGN_OUT_TOP_MID:      align = LV_ALIGN_OUT_RIGHT_MID; break;
        case LV_ALIGN_OUT_TOP_RIGHT:    align = LV_ALIGN_OUT_RIGHT_BOTTOM; break;
        /* Outside aligns on BOTTOM side -> LEFT side */
        case LV_ALIGN_OUT_BOTTOM_LEFT:  align = LV_ALIGN_OUT_LEFT_TOP; break;
        case LV_ALIGN_OUT_BOTTOM_MID:   align = LV_ALIGN_OUT_LEFT_MID; break;
        case LV_ALIGN_OUT_BOTTOM_RIGHT: align = LV_ALIGN_OUT_LEFT_BOTTOM; break;
        /* Outside aligns on LEFT side -> TOP side */
        case LV_ALIGN_OUT_LEFT_TOP:     align = LV_ALIGN_OUT_TOP_RIGHT; break;
        case LV_ALIGN_OUT_LEFT_MID:     align = LV_ALIGN_OUT_TOP_MID; break;
        case LV_ALIGN_OUT_LEFT_BOTTOM:  align = LV_ALIGN_OUT_TOP_LEFT; break;
        /* Outside aligns on RIGHT side -> BOTTOM side */
        case LV_ALIGN_OUT_RIGHT_TOP:    align = LV_ALIGN_OUT_BOTTOM_RIGHT; break;
        case LV_ALIGN_OUT_RIGHT_MID:    align = LV_ALIGN_OUT_BOTTOM_MID; break;
        case LV_ALIGN_OUT_RIGHT_BOTTOM: align = LV_ALIGN_OUT_BOTTOM_LEFT; break;
        default:
            LOG_ERROR("The current object alignment is invalid");
            return;
    }

    gobj->aln.align = align;
}

static void g_obj_rot_90_swap_ofs(g_obj *gobj)
{
    int32_t swap;

    swap = gobj->aln.x;
    gobj->aln.x = -(gobj->aln.y);
    gobj->aln.y = swap;
}

static int32_t g_obj_rot_calc_align(g_obj *gobj)
{
    int8_t scr_rot;
    int8_t rot_cnt;

    scr_rot = g_get_scr_rot_dir();
    switch (gobj->pos.rot) {
        case ROT_0:
            switch (scr_rot) {
                case ROT_0: rot_cnt = 0; break;
                case ROT_90: rot_cnt = 1; break;
                case ROT_180: rot_cnt = 2; break;
                case ROT_270: rot_cnt = 3; break;
                default:
                    return -1;
            }
            break;
        case ROT_90:
            switch (scr_rot) {
                case ROT_0: rot_cnt = 3; break;
                case ROT_90: rot_cnt = 0; break;
                case ROT_180: rot_cnt = 1; break;
                case ROT_270: rot_cnt = 2; break;
                default:
                    return -1;
            }
            break;
        case ROT_180:
            switch (scr_rot) {
                case ROT_0: rot_cnt = 2; break;
                case ROT_90: rot_cnt = 3; break;
                case ROT_180: rot_cnt = 0; break;
                case ROT_270: rot_cnt = 1; break;
                default:
                    return -1;
            }
            break;
        case ROT_270:
            switch (scr_rot) {
                case ROT_0: rot_cnt = 1; break;
                case ROT_90: rot_cnt = 2; break;
                case ROT_180: rot_cnt = 3; break;
                case ROT_270: rot_cnt = 0; break;
                default:
                    return -1;
            }
            break;
        default:
            return -1;
    }

    for (int8_t cnt = 0; cnt < rot_cnt; cnt++) {
        g_obj_rot_90_set_aln(gobj);
        g_obj_rot_90_swap_ofs(gobj);
    }

    return 0;
}

static int32_t g_base_obj_rotate(g_obj *gobj)
{
    int32_t ret;

    /*
     * For each object, when rotation occurs, its size must be recalculated.
     * Since the root coordinate does not change, the width and height
     * will be adjusted according to the logical rotation.
     */
    ret = g_obj_rot_calc_size(gobj);
    if (ret) {
        return -EINVAL;
    }
    lv_obj_set_size(gobj->obj, gobj->pos.w, gobj->pos.h);

    /*
     * For an object placed inside a parent, its new center point must be
     * recalculated based on the logical rotation. Using this new center,
     * the width and height can then be updated accordingly.
     */
    if (gobj->aln.align == LV_ALIGN_DEFAULT) {
        ret= g_obj_rot_calc_center(gobj);
        if (ret) {
            return -EINVAL;
        }
        lv_obj_set_pos(gobj->obj, gobj->pos.x_mid - (gobj->pos.w / 2), \
                       gobj->pos.y_mid - (gobj->pos.h / 2));
    } else {
        // Update object alignment according to logical rotation
        ret = g_obj_rot_calc_align(gobj);
        if (ret) {
            return -EINVAL;
        }
        lv_obj_align_to(gobj->obj, gobj->aln.base, gobj->aln.align, \
                        gobj->aln.x, gobj->aln.y);
    }

    return 0;
}

static int32_t g_transform_obj_rotate(g_obj *gobj)
{
    int32_t ret;
    int32_t scr_rot = g_get_scr_rot_dir();
    int32_t rot_val = 0;

    ret = g_obj_rot_calc_size(gobj);
    if (ret) {
        return -EINVAL;
    }

    ret= g_obj_rot_calc_center(gobj);
    if (ret) {
        return -EINVAL;
    }

    if (scr_rot == LV_DISPLAY_ROTATION_0) {
        rot_val = 0;
        lv_obj_set_style_transform_rotation(gobj->obj, rot_val, 0);
        lv_obj_set_pos(gobj->obj, gobj->pos.x_mid - (gobj->pos.w / 2), \
                       gobj->pos.y_mid - (gobj->pos.h / 2));
    } else if (scr_rot == LV_DISPLAY_ROTATION_90) {
        rot_val = 900;
        lv_obj_set_style_transform_rotation(gobj->obj, rot_val, 0);
        lv_obj_set_pos(gobj->obj, gobj->pos.x_mid + (gobj->pos.w / 2), \
                       gobj->pos.y_mid - (gobj->pos.h / 2));
    } else if (scr_rot == LV_DISPLAY_ROTATION_180) {
        rot_val = 1800;
        lv_obj_set_style_transform_rotation(gobj->obj, rot_val, 0);
        lv_obj_set_pos(gobj->obj, gobj->pos.x_mid + (gobj->pos.w / 2), \
                       gobj->pos.y_mid + (gobj->pos.h / 2));
    } else if (scr_rot == LV_DISPLAY_ROTATION_270) {
        rot_val = 2700;
        lv_obj_set_style_transform_rotation(gobj->obj, rot_val, 0);
        lv_obj_set_pos(gobj->obj, gobj->pos.x_mid - (gobj->pos.w / 2), \
                       gobj->pos.y_mid + (gobj->pos.h / 2));
    }

    return 0;
}

static int32_t g_obj_rotate(g_obj *gobj)
{
    int32_t ret;
    int32_t scr_rot = g_get_scr_rot_dir();

    if (!gobj) {
        LOG_ERROR("Invalid g object");
        return -EINVAL;
    }

    if (gobj->pos.rot == scr_rot) {
        return 0;
    }

    // TODO: check obj type and update flex flow, scale...
    // Text, icon, switch will be rotate
    // Frame, button, slider will be resize and relocation
    switch (gobj->type) {
        case OBJ_BASE:
        case OBJ_BTN:
        case OBJ_SLIDER:
            ret = g_base_obj_rotate(gobj);
            break;
        case OBJ_LABEL:
        case OBJ_SWITCH:
        case OBJ_ICON:
            ret = g_transform_obj_rotate(gobj);
            break;

        case OBJ_CONTAINER:
            /*
             * The container maintains its original offset (0,0)
             * for scrolling purposes.
             */
            ret = g_obj_rot_calc_size(gobj);
            if (!ret)
                lv_obj_set_size(gobj->obj, gobj->pos.w, gobj->pos.h);
            break;
        default:
            LOG_WARN("Unknown G object type: %d", gobj->type);
            break;
    }

    if (ret) {
        return ret;
    }

    gobj->pos.rot = scr_rot;

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
    g_obj *gobj = NULL;
    lv_obj_t *lobj = NULL;

    LV_ASSERT_NULL(par);
    LOG_TRACE("Create obj id %d", id);


    switch (type) {
        case OBJ_BASE:
        case OBJ_CONTAINER:
            lobj = lv_obj_create(par);
            break;
        case OBJ_BTN:
            lobj = lv_btn_create(par);
            break;
        case OBJ_SLIDER:
            lobj = lv_slider_create(par);
            break;
        case OBJ_LABEL:
        case OBJ_ICON:
            lobj = lv_label_create(par);
            break;
        case OBJ_SWITCH:
            lobj = lv_switch_create(par);
            break;
        default:
            LOG_ERROR("G Object type %d - id %d invalid", type, id);
            lobj = NULL;
            break;

    }

    LV_ASSERT_NULL(lobj);

    gobj = gf_register_obj(par, lobj, id);
    gobj->type = type;
    gobj->pos.rot = ROT_0;
    gobj->aln.align = LV_ALIGN_DEFAULT;

    LV_ASSERT_NULL(gobj);

    return gobj->obj;
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

void gf_gobj_set_pos(lv_obj_t *lobj, int32_t x_ofs, int32_t y_ofs)
{
    g_obj *gobj = NULL;
    LV_ASSERT_NULL(lobj);

    lv_obj_set_pos(lobj, x_ofs, y_ofs);

    gobj = lobj->user_data;
    if (!gobj->pos.w)
        LOG_WARN("Cannot calculate the center x");
    if (!gobj->pos.h)
        LOG_WARN("Cannot calculate the center y");
    gobj->pos.x_mid = x_ofs + (gobj->pos.w / 2);
    gobj->pos.y_mid = y_ofs + (gobj->pos.h / 2);
}

void gf_gobj_align_to(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                      int32_t x_ofs, int32_t y_ofs)
{
    g_obj *gobj = NULL;
    LV_ASSERT_NULL(lobj);

    lv_obj_align_to(lobj, base, align, x_ofs, y_ofs);

    gobj = lobj->user_data;
    LV_ASSERT_NULL(gobj);
    gobj->aln.align = align;
    gobj->aln.base = base;
    gobj->aln.x = x_ofs;
    gobj->aln.y = y_ofs;
}

lv_obj_t * gf_create_box(lv_obj_t *par, uint32_t id, int32_t x, int32_t y, \
                         uint32_t w, uint32_t h, lv_color_t color)
{
    lv_obj_t *lobj = gf_create_gobj_type(par, OBJ_BASE, id);

    gf_gobj_set_size(lobj, w, h);
    gf_gobj_set_pos(lobj, x, y);

    lv_obj_set_style_pad_all(lobj, 0, 0);
    lv_obj_set_style_pad_gap(lobj, 0, 0);

    lv_obj_set_style_bg_color(lobj, color, 0);
    return lobj;
}

lv_obj_t * gf_create_container(lv_obj_t *par, uint32_t id, int32_t x, \
                               int32_t y, uint32_t w, uint32_t h, \
                               lv_color_t color)
{
    lv_obj_t *lobj = gf_create_gobj_type(par, OBJ_CONTAINER, id);

    gf_gobj_set_size(lobj, w, h);
    gf_gobj_set_pos(lobj, x, y);

    lv_obj_set_style_pad_all(lobj, 0, 0);
    lv_obj_set_style_pad_gap(lobj, 0, 0);

    lv_obj_set_style_bg_color(lobj, color, 0);
    return lobj;
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

#if defined(TEST)
/*
 * TESTING *********************************************************************
 */

lv_obj_t *pl_main_box = NULL;
lv_obj_t *pl_child_box = NULL;
lv_obj_t *pl_child_box_corner1 = NULL;
lv_obj_t *pl_child_box_corner2 = NULL;
lv_obj_t *pl_child_box_corner2_text_wrapper = NULL;
lv_obj_t *pl_child_box_corner2_text_box = NULL;
lv_obj_t *pl_container_box = NULL;

lv_obj_t *pl_text_box = NULL;
lv_obj_t *pl_text_wrapper = NULL;
lv_obj_t *pl_switch = NULL;
lv_obj_t *pl_switch_wrapper = NULL;
lv_obj_t *pl_icon = NULL;
lv_obj_t *pl_icon_wrapper = NULL;
lv_obj_t *pl_btn = NULL;
lv_obj_t *pl_slider = NULL;

void sample_rot(int32_t angle)
{
    // TODO: implement recursive rotate from parent
    g_set_scr_rot_dir(angle);
    g_obj_rotate(pl_child_box->user_data);
    g_obj_rotate(pl_child_box_corner1->user_data);
    g_obj_rotate(pl_child_box_corner2->user_data);
    g_obj_rotate(pl_child_box_corner2_text_wrapper->user_data);
    g_obj_rotate(pl_child_box_corner2_text_box->user_data);
    g_obj_rotate(pl_container_box->user_data);
    g_obj_rotate(pl_text_wrapper->user_data);
    g_obj_rotate(pl_text_box->user_data);
    g_obj_rotate(pl_switch_wrapper->user_data);
    g_obj_rotate(pl_switch->user_data);
    g_obj_rotate(pl_icon_wrapper->user_data);
    g_obj_rotate(pl_icon->user_data);
    g_obj_rotate(pl_btn->user_data);
    g_obj_rotate(pl_slider->user_data);


    int32_t w, h;
    lv_obj_update_layout(pl_container_box);
    w = lv_obj_get_width(pl_container_box);
    h = lv_obj_get_height(pl_container_box);

    int32_t rot_dir = g_get_scr_rot_dir();

    // Adjust screen scroll accordingly.
    if (rot_dir == ROT_0) {
        lv_obj_scroll_to(pl_child_box, 0, 0, LV_ANIM_OFF);
    } else if (rot_dir == ROT_90) {
        lv_obj_scroll_to(pl_child_box, w, 0, LV_ANIM_OFF);
    } else if (rot_dir == ROT_180) {
        lv_obj_scroll_to(pl_child_box, 0, h, LV_ANIM_OFF);
    } else if (rot_dir == ROT_270) {
        lv_obj_scroll_to(pl_child_box, 0, 0, LV_ANIM_OFF);
    }
}

int32_t get_random_0_3(void)
{
    int32_t val;

    val = rand() % 4;
    return val;
}
static void btn_handler(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);  // Get the button object
    lv_obj_t *par = lv_obj_get_parent(btn);
    g_obj *gobj = NULL;

    // gf_refresh_all_layer();

    gobj = btn->user_data;
    LOG_DEBUG("ID %d: Taskbar button clicked", gobj->id);
    sample_rot(get_random_0_3());

}

void create_dynamic_ui()
{
    int8_t l_align = 30;

    // Main box as screen background
    pl_main_box = gf_create_box(lv_screen_active(), 0, 0, 0, 1024, 600, \
                                lv_color_hex(0x000000));

    pl_child_box_corner1 = gf_create_box(pl_main_box, 0, 0, 0, 80, 90, \
                                         lv_color_hex(0x0000FF));
    gf_gobj_align_to(pl_child_box_corner1, pl_main_box, \
                     LV_ALIGN_BOTTOM_MID,  0, 0);
    pl_child_box_corner2 = gf_create_box(pl_main_box, 0, 0, 0, 200, 200, \
                                         lv_color_hex(0x00FF00));
    gf_gobj_align_to(pl_child_box_corner2, pl_main_box, \
                     LV_ALIGN_BOTTOM_RIGHT, 0, 0);



    pl_child_box_corner2_text_wrapper = gf_create_box(\
            pl_child_box_corner2, 0, 0, 0, 150, 40, lv_color_hex(0xFFFFFF));
    lv_obj_clear_flag(pl_child_box_corner2_text_wrapper, LV_OBJ_FLAG_SCROLLABLE);
    gf_gobj_align_to(pl_child_box_corner2_text_wrapper, pl_child_box_corner2, \
                     LV_ALIGN_CENTER, 10, 25);
    pl_child_box_corner2_text_box = gf_create_textbox(\
            pl_child_box_corner2_text_wrapper, 0, 10, 10, "CORNER");
    // TODO: verify text box alignment
    // gf_gobj_align_to(pl_child_box_corner2_text_box, \
    //                  pl_child_box_corner2_text_wrapper, \
    //                  LV_ALIGN_CENTER, 0, 0);


    // Child box as a menu bar
    pl_child_box = gf_create_box(pl_main_box, 0, 32, 51, 400, 500, \
                                 lv_color_hex(0xFFFFFF));
    // Container for all sub components
    pl_container_box = gf_create_container(pl_child_box, 0, 10, 10, 370, 800, \
                                 lv_color_hex(0xFFEE8C));




    pl_text_wrapper = gf_create_box(pl_container_box, 0, l_align, 25, 150, 40, \
                                 lv_color_hex(0xFFFFFF));
    lv_obj_clear_flag(pl_text_wrapper, LV_OBJ_FLAG_SCROLLABLE);

    gf_gobj_align_to(pl_text_wrapper, pl_container_box, LV_ALIGN_TOP_LEFT, 30, 25);
    pl_text_box = gf_create_textbox(pl_text_wrapper, 0, 10, 10, "Go001 hahaha");




    pl_switch_wrapper = gf_create_box(pl_container_box, 0, l_align, 85, 80, 50, \
                                 lv_color_hex(0xFFFFFF));
    lv_obj_clear_flag(pl_switch_wrapper, LV_OBJ_FLAG_SCROLLABLE);
    gf_gobj_align_to(pl_switch_wrapper, pl_text_wrapper, LV_ALIGN_OUT_BOTTOM_MID, 15, 25);
    pl_switch = gf_create_switch(pl_switch_wrapper, 0, 10, 10, 60, 30);


    pl_icon_wrapper = gf_create_box(pl_container_box, 0, l_align, 150, 50, 50, \
                                 lv_color_hex(0xFFFFFF));
    lv_obj_clear_flag(pl_icon_wrapper, LV_OBJ_FLAG_SCROLLABLE);
    gf_gobj_align_to(pl_icon_wrapper, pl_switch_wrapper, LV_ALIGN_OUT_BOTTOM_MID, 30, 25);
    pl_icon = gf_create_sym(pl_icon_wrapper, 0, 10, 10, &terminal_icons_32, \
                            ICON_TOOLBOX_SOLID, lv_color_hex(0xFFFF00));



    pl_btn = gf_create_btn(pl_container_box, 0, l_align, 230, 80, 50);
    gf_gobj_align_to(pl_btn, pl_icon_wrapper, LV_ALIGN_OUT_BOTTOM_RIGHT, 10, 25);
    pl_slider = gf_create_slider(pl_container_box, 0, l_align, 300, 100, 20);
    gf_gobj_align_to(pl_slider, pl_btn, LV_ALIGN_OUT_RIGHT_MID, 30, 0);


    gf_register_handler(pl_btn, 0, btn_handler, LV_EVENT_CLICKED);
}
#endif
