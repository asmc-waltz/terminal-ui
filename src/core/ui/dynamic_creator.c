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

#include <list.h>

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

#define ROTATION_0                      LV_DISPLAY_ROTATION_0
#define ROTATION_90                     LV_DISPLAY_ROTATION_90
#define ROTATION_180                    LV_DISPLAY_ROTATION_180
#define ROTATION_270                    LV_DISPLAY_ROTATION_270
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
static int32_t g_scr_rot_dir = ROTATION_0;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
/*
 * Recalculate object's midpoint (x_mid, y_mid) when parent size
 * or screen rotation changes.
 *
 * The function preserves the relative placement rules used in the
 * original implementation: midpoint values are distances from the
 * parent's left/top edges (not the child's left/top corner).
 *
 * This version:
 *  - covers all old_rot -> scr_rot combinations (12 mappings),
 *  - validates inputs,
 *  - computes new midpoint using local variables,
 *  - validates result before updating gobj state (atomic update),
 *  - logs clearly.
 *
 * Returns:
 *  0        -> success (gobj updated)
 *  -EINVAL  -> bad input
 *  -ERANGE  -> computed midpoint is out of new parent bounds
 */
static int32_t g_obj_get_center(g_obj *gobj, int32_t par_w, int32_t par_h)
{
    int32_t new_x_mid = -1;
    int32_t new_y_mid = -1;
    int32_t scr_rot;
    int32_t old_rot;
    int32_t old_pw;
    int32_t old_ph;
    int32_t L; /* distance from old left edge to object's center */
    int32_t T; /* distance from old top edge  to object's center */
    int32_t R; /* distance from old right edge to object's center */
    int32_t B; /* distance from old bottom edge to object's center */

    if (!gobj) {
        LOG_ERROR("null gobj");
        return -EINVAL;
    }

    scr_rot = g_get_scr_rot_dir();
    old_rot = gobj->pos.rot;

    /* nothing to do if rotation unchanged */
    if (scr_rot == old_rot)
        return 0;

    /* sanity check rotation values (expect 0..3 mapping to 0/90/180/270) */
    if (old_rot < ROTATION_0 || old_rot > ROTATION_270 ||
        scr_rot < ROTATION_0 || scr_rot > ROTATION_270) {
        LOG_ERROR("invalid rot old=%d new=%d",
                  old_rot, scr_rot);
        return -EINVAL;
    }

    /* cache old parent geometry and gaps */
    old_pw = gobj->pos.par_w;
    old_ph = gobj->pos.par_h;
    L = gobj->pos.x_mid;
    T = gobj->pos.y_mid;
    R = old_pw - L;
    B = old_ph - T;

    LOG_TRACE("obj id=%d - old_rot=%d -> scr_rot=%d, "
              "old_pw=%d old_ph=%d, L=%d T=%d R=%d B=%d, "
              "new_par=(%d,%d)", gobj->id,
              old_rot, scr_rot, old_pw, old_ph, L, T, R, B, par_w, par_h);

    /* === mapping table: old_rot -> scr_rot ===
     * We keep the exact equations used originally (explicit 12 cases).
     * Each expression computes new center coordinates relative to new parent.
     */
    switch (old_rot) {
    case ROTATION_0:
        if (scr_rot == ROTATION_90) {
            new_x_mid = par_w - T;
            new_y_mid = L;
        } else if (scr_rot == ROTATION_180) {
            new_x_mid = par_w - L;
            new_y_mid = par_h - T;
        } else if (scr_rot == ROTATION_270) {
            new_x_mid = T;
            new_y_mid = par_h - L;
        }
        break;
    case ROTATION_90:
        if (scr_rot == ROTATION_0) {
            new_x_mid = T;
            new_y_mid = old_pw - L; /* old_pw == parent's width before rotation */
        } else if (scr_rot == ROTATION_180) {
            new_x_mid = par_w - T;
            new_y_mid = par_h - (old_pw - L);
        } else if (scr_rot == ROTATION_270) {
            new_x_mid = par_w - L;
            new_y_mid = par_h - T;
        }
        break;
    case ROTATION_180:
        if (scr_rot == ROTATION_0) {
            new_x_mid = par_w - L;
            new_y_mid = par_h - T;
        } else if (scr_rot == ROTATION_90) {
            new_x_mid = par_w - (old_ph - T);
            new_y_mid = old_pw - L;
        } else if (scr_rot == ROTATION_270) {
            new_x_mid = old_ph - T;
            new_y_mid = par_h - (old_pw - L);
        }
        break;
    case ROTATION_270:
        if (scr_rot == ROTATION_0) {
            new_x_mid = old_ph - T;
            new_y_mid = L;
        } else if (scr_rot == ROTATION_90) {
            new_x_mid = par_w - L;
            new_y_mid = par_h - T;
        } else if (scr_rot == ROTATION_180) {
            new_x_mid = par_w - (old_ph - T);
            new_y_mid = par_h - L;
        }
        break;
    default:
        /* unreachable due to earlier validation */
        return -EINVAL;
    }

    /* ensure mapping was computed */
    if (new_x_mid < 0 || new_y_mid < 0) {
        LOG_ERROR("mapping not produced or negative: x=%d y=%d",
                  new_x_mid, new_y_mid);
        return -ERANGE;
    }

    /* bounds check against new parent size before mutating state */
    if (new_x_mid < 0 || new_x_mid > par_w ||
        new_y_mid < 0 || new_y_mid > par_h) {
        LOG_WARN("computed midpoint out of bounds: "
                 "x=%d (0..%d) y=%d (0..%d)",
                 new_x_mid, par_w, new_y_mid, par_h);
        return -ERANGE;
    }

    /* Atomic update of gobj position state */
    gobj->pos.x_mid = new_x_mid;
    gobj->pos.y_mid = new_y_mid;
    gobj->pos.par_w = par_w;
    gobj->pos.par_h = par_h;
    gobj->pos.rot = scr_rot;

    LOG_TRACE("success new_mid=(%d,%d) new_par=(%d,%d) rot=%d",
              new_x_mid, new_y_mid, par_w, par_h, scr_rot);

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

static void g_obj_rot_90_set_aln(g_obj *gobj)
{
    int8_t align = LV_ALIGN_DEFAULT;

    switch (gobj->aln.align) {
        case LV_ALIGN_TOP_LEFT:
            align = LV_ALIGN_TOP_RIGHT;
            break;
        case LV_ALIGN_TOP_MID:
            align = LV_ALIGN_RIGHT_MID;
            break;
        case LV_ALIGN_TOP_RIGHT:
            align = LV_ALIGN_BOTTOM_RIGHT;
            break;
        case LV_ALIGN_LEFT_MID:
            align = LV_ALIGN_TOP_MID;
            break;
        case LV_ALIGN_CENTER:
            align = LV_ALIGN_CENTER;
            break;
        case LV_ALIGN_RIGHT_MID:
            align = LV_ALIGN_BOTTOM_MID;
            break;
        case LV_ALIGN_BOTTOM_LEFT:
            align = LV_ALIGN_TOP_LEFT;
            break;
        case LV_ALIGN_BOTTOM_MID:
            align = LV_ALIGN_LEFT_MID;
            break;
        case LV_ALIGN_BOTTOM_RIGHT:
            align = LV_ALIGN_BOTTOM_LEFT;
            break;

        /* Outside aligns on TOP side -> RIGHT side */
        case LV_ALIGN_OUT_TOP_LEFT:
            align = LV_ALIGN_OUT_RIGHT_TOP;
            break;
        case LV_ALIGN_OUT_TOP_MID:
            align = LV_ALIGN_OUT_RIGHT_MID;
            break;
        case LV_ALIGN_OUT_TOP_RIGHT:
            align = LV_ALIGN_OUT_RIGHT_BOTTOM;
            break;
        /* Outside aligns on BOTTOM side -> LEFT side */
        case LV_ALIGN_OUT_BOTTOM_LEFT:
            align = LV_ALIGN_OUT_LEFT_TOP;
            break;
        case LV_ALIGN_OUT_BOTTOM_MID:
            align = LV_ALIGN_OUT_LEFT_MID;
            break;
        case LV_ALIGN_OUT_BOTTOM_RIGHT:
            align = LV_ALIGN_OUT_LEFT_BOTTOM;
            break;
        /* Outside aligns on LEFT side -> TOP side */
        case LV_ALIGN_OUT_LEFT_TOP:
            align = LV_ALIGN_OUT_TOP_RIGHT;
            break;
        case LV_ALIGN_OUT_LEFT_MID:
            align = LV_ALIGN_OUT_TOP_MID;
            break;
        case LV_ALIGN_OUT_LEFT_BOTTOM:
            align = LV_ALIGN_OUT_TOP_LEFT;
            break;
        /* Outside aligns on RIGHT side -> BOTTOM side */
        case LV_ALIGN_OUT_RIGHT_TOP:
            align = LV_ALIGN_OUT_BOTTOM_RIGHT;
            break;
        case LV_ALIGN_OUT_RIGHT_MID:
            align = LV_ALIGN_OUT_BOTTOM_MID;
            break;
        case LV_ALIGN_OUT_RIGHT_BOTTOM:
            align = LV_ALIGN_OUT_BOTTOM_LEFT;
            break;
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
    int8_t cur_rot;
    int8_t scr_rot;
    int8_t rot_cnt;

    if (!gobj)
        return -EINVAL;

    cur_rot = gobj->pos.rot;
    scr_rot = g_get_scr_rot_dir();

    if (cur_rot < ROTATION_0 || cur_rot > ROTATION_270 ||
        scr_rot < ROTATION_0 || scr_rot > ROTATION_270)
        return -EINVAL;

    rot_cnt = (scr_rot - cur_rot + 4) % 4;

    for (int8_t i = 0; i < rot_cnt; i++) {
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

    if (scr_rot == ROTATION_0) {
        rot_val = 0;
        lv_obj_set_style_transform_rotation(gobj->obj, rot_val, 0);
        lv_obj_set_pos(gobj->obj, gobj->pos.x_mid - (gobj->pos.w / 2), \
                       gobj->pos.y_mid - (gobj->pos.h / 2));
    } else if (scr_rot == ROTATION_90) {
        rot_val = 900;
        lv_obj_set_style_transform_rotation(gobj->obj, rot_val, 0);
        lv_obj_set_pos(gobj->obj, gobj->pos.x_mid + (gobj->pos.w / 2), \
                       gobj->pos.y_mid - (gobj->pos.h / 2));
    } else if (scr_rot == ROTATION_180) {
        rot_val = 1800;
        lv_obj_set_style_transform_rotation(gobj->obj, rot_val, 0);
        lv_obj_set_pos(gobj->obj, gobj->pos.x_mid + (gobj->pos.w / 2), \
                       gobj->pos.y_mid + (gobj->pos.h / 2));
    } else if (scr_rot == ROTATION_270) {
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

static int32_t gf_rotate_all(g_obj *gobj)
{
    g_obj *p_obj;
    int32_t ret;
    struct list_head *par_list;

    par_list = &gobj->child;

    list_for_each_entry(p_obj, par_list, node) {
        ret = g_obj_rotate(p_obj);
        if (ret < 0)
            return ret;

        ret = gf_rotate_all(p_obj);
        if (ret < 0)
            return ret;
    }

    return 0;
}

static lv_obj_t *gf_create_gobj_type(lv_obj_t *par, int32_t type, uint32_t id)
{
    g_obj *gobj = NULL;
    lv_obj_t *lobj = NULL;

    LV_ASSERT_NULL(par);

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
    gobj->pos.rot = ROTATION_0;
    gobj->aln.align = LV_ALIGN_DEFAULT;

    LV_ASSERT_NULL(gobj);

    return gobj->obj;
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

lv_obj_t * gf_create_box(lv_obj_t *par, uint32_t id)
{
    lv_obj_t *lobj = gf_create_gobj_type(par, OBJ_BASE, id);


    lv_obj_set_style_pad_all(lobj, 0, 0);
    lv_obj_set_style_pad_gap(lobj, 0, 0);

    return lobj;
}

lv_obj_t * gf_create_container(lv_obj_t *par, uint32_t id)
{
    lv_obj_t *lobj = gf_create_gobj_type(par, OBJ_CONTAINER, id);
    lv_obj_set_style_pad_all(lobj, 0, 0);
    lv_obj_set_style_pad_gap(lobj, 0, 0);
    return lobj;
}

/*
 * Fn: gf_create_text
 *     gf_create_sym
 * The textbox uses transform rotation instead of layout change like other
 * components, so its root coordinate changes. This makes it difficult to
 * apply normal object alignment as with other components. Additionally, the
 * transform affects the logical size, impacting its parent. The workaround is
 * to create a textbox container with scrolling disabled. Inside this box, we
 * manually align the text or symbol, while the container itself can align to
 * another object as normal.
 */
lv_obj_t * gf_create_text(lv_obj_t *par, uint32_t id, int32_t x, int32_t y, \
                             const char *txt_str)
{
    int32_t w, h;
    lv_obj_t *lobj = gf_create_gobj_type(par, OBJ_LABEL, id);
    LV_ASSERT_NULL(lobj);
    lv_obj_set_style_text_font(lobj, &lv_font_montserrat_18, 0);
    lv_label_set_text_fmt(lobj, "%s", txt_str);
    lv_obj_update_layout(lobj);
    w = lv_obj_get_width(lobj);
    h = lv_obj_get_height(lobj);
    gf_gobj_set_size(lobj, w, h);
    gf_gobj_set_pos(lobj, x, y);
    return lobj;
}

lv_obj_t * gf_create_sym(lv_obj_t *par, uint32_t id, int32_t x, int32_t y, \
                         const lv_font_t *font, const char *index, \
                         lv_color_t color)
{
    lv_obj_t *lobj = gf_create_gobj_type(par, OBJ_ICON, id);
    LV_ASSERT_NULL(lobj);
    lv_obj_set_style_text_font(lobj, font, 0);
    lv_obj_set_style_text_color(lobj, color, 0);
    lv_label_set_text(lobj, index);
    gf_gobj_get_size(lobj);
    gf_gobj_set_pos(lobj, x, y);
    return lobj;
}

lv_obj_t * gf_create_switch(lv_obj_t *par, uint32_t id)
{
    lv_obj_t *lobj = gf_create_gobj_type(par, OBJ_SWITCH, id);
    LV_ASSERT_NULL(lobj);
    return lobj;
}


lv_obj_t * gf_create_btn(lv_obj_t *par, uint32_t id)
{
    lv_obj_t *lobj = gf_create_gobj_type(par, OBJ_BTN, id);
    LV_ASSERT_NULL(lobj);
    return lobj;
}

lv_obj_t * gf_create_slider(lv_obj_t *par, uint32_t id)
{
    lv_obj_t *lobj = gf_create_gobj_type(par, OBJ_SLIDER, id);
    LV_ASSERT_NULL(lobj);
    return lobj;
}

int32_t gf_rotate_obj_tree(g_obj *gobj)
{
    int32_t ret;

    ret = g_obj_rotate(gobj);
    if (ret < 0)
        return ret;

    ret = gf_rotate_all(gobj);
    if (ret < 0)
        return ret;

    return 0;
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
    // g_set_scr_rot_dir(ROTATION_0);
    g_set_scr_rot_dir(ROTATION_90);
    // g_set_scr_rot_dir(ROTATION_180);
    // g_set_scr_rot_dir(ROTATION_270);

    // Retrieve rotation state.
    rot_dir = g_get_scr_rot_dir();

    // Adjust screen size accordingly.
    if (rot_dir == ROTATION_0) {
        g_set_scr_size(1024, 600);
    } else if (rot_dir == ROTATION_90) {
        g_set_scr_size(600, 1024);
    } else if (rot_dir == ROTATION_180) {
        g_set_scr_size(1024, 600);
    } else if (rot_dir == ROTATION_270) {
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

#if defined(TEST)
/*
 * TESTING *********************************************************************
 */

lv_obj_t *ex_scr = NULL;
lv_obj_t *ex_window = NULL;
lv_obj_t *ex_mid_box = NULL;
lv_obj_t *ex_corner_box = NULL;
lv_obj_t *ex_text_box2 = NULL;
lv_obj_t *ex_text_2 = NULL;
lv_obj_t *ex_comp_cont = NULL;
lv_obj_t *ex_text_box1 = NULL;
lv_obj_t *ex_text1 = NULL;
lv_obj_t *ex_sw_box = NULL;
lv_obj_t *ex_sw1 = NULL;
lv_obj_t *ex_sym_box1 = NULL;
lv_obj_t *ex_sym1 = NULL;
lv_obj_t *ex_btn1 = NULL;
lv_obj_t *ex_slider1 = NULL;

void sample_rot(int32_t angle)
{
    g_set_scr_rot_dir(angle);

    gf_rotate_obj_tree(ex_window->user_data);
    gf_rotate_obj_tree(ex_mid_box->user_data);
    gf_rotate_obj_tree(ex_corner_box->user_data);

    int32_t w, h;
    lv_obj_update_layout(ex_comp_cont);
    w = lv_obj_get_width(ex_comp_cont);
    h = lv_obj_get_height(ex_comp_cont);

    int32_t rot_dir = g_get_scr_rot_dir();

    // Adjust screen scroll accordingly.
    if (rot_dir == ROTATION_0) {
        lv_obj_scroll_to(ex_window, 0, 0, LV_ANIM_OFF);
    } else if (rot_dir == ROTATION_90) {
        lv_obj_scroll_to(ex_window, w, 0, LV_ANIM_OFF);
    } else if (rot_dir == ROTATION_180) {
        lv_obj_scroll_to(ex_window, 0, h, LV_ANIM_OFF);
    } else if (rot_dir == ROTATION_270) {
        lv_obj_scroll_to(ex_window, 0, 0, LV_ANIM_OFF);
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
    LOG_DEBUG("ID %d: Test button clicked", gobj->id);
    sample_rot(get_random_0_3());

}

void create_dynamic_ui()
{
    int8_t l_align = 30;

    // Main box as screen background
    ex_scr = gf_create_box(lv_screen_active(), 0);
    gf_gobj_set_size(ex_scr, 1024, 600);
    gf_gobj_set_pos(ex_scr, 0, 0);
    lv_obj_set_style_bg_color(ex_scr, lv_color_hex(0x000000), 0);
    //--------------------------------------------------------------------------
    ex_mid_box = gf_create_box(ex_scr, 0);
    gf_gobj_set_size(ex_mid_box, 80, 90);
    lv_obj_set_style_bg_color(ex_mid_box, lv_color_hex(0x0000FF), 0);
    gf_gobj_align_to(ex_mid_box, ex_scr, LV_ALIGN_BOTTOM_MID, 0, 0);
    //--------------------------------------------------------------------------
    ex_corner_box = gf_create_box(ex_scr, 0);
    gf_gobj_set_size(ex_corner_box, 200, 200);
    lv_obj_set_style_bg_color(ex_corner_box, lv_color_hex(0x00FF00), 0);
    gf_gobj_align_to(ex_corner_box, ex_scr, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    //--------------------------------------------------------------------------
    ex_text_box2 = gf_create_box(ex_corner_box, 0);
    gf_gobj_set_size(ex_text_box2, 150, 40);
    lv_obj_set_style_bg_color(ex_text_box2, lv_color_hex(0xFFFFFF), 0);
    lv_obj_clear_flag(ex_text_box2, LV_OBJ_FLAG_SCROLLABLE);
    gf_gobj_align_to(ex_text_box2, ex_corner_box, LV_ALIGN_CENTER, 10, 25);
    //--------------------------------------------------------------------------
    ex_text_2 = gf_create_text(ex_text_box2, 0, 10, 10, "CORNER");
    //--------------------------------------------------------------------------
    // Child box as a menu bar
    ex_window = gf_create_box(ex_scr, 0);
    gf_gobj_set_size(ex_window, 400, 500);
    gf_gobj_set_pos(ex_window, 32, 51);
    lv_obj_set_style_bg_color(ex_window, lv_color_hex(0xFFFFFF), 0);
    //--------------------------------------------------------------------------
    // Container for all sub components
    ex_comp_cont = gf_create_container(ex_window, 0);
    gf_gobj_set_size(ex_comp_cont, 370, 1200);
    gf_gobj_set_pos(ex_comp_cont, 10, 10);
    lv_obj_set_style_bg_color(ex_comp_cont, lv_color_hex(0xFFEE8C), 0);
    //--------------------------------------------------------------------------
    ex_text_box1 = gf_create_box(ex_comp_cont, 0);
    gf_gobj_set_size(ex_text_box1, 150, 40);
    lv_obj_set_style_bg_color(ex_text_box1, lv_color_hex(0xFFFFFF), 0);
    lv_obj_clear_flag(ex_text_box1, LV_OBJ_FLAG_SCROLLABLE);
    gf_gobj_align_to(ex_text_box1, ex_comp_cont, LV_ALIGN_TOP_LEFT, 30, 25);
    //--------------------------------------------------------------------------
    ex_text1 = gf_create_text(ex_text_box1, 0, 10, 10, "ASMC WALTZ");
    //--------------------------------------------------------------------------
    ex_sw_box = gf_create_box(ex_comp_cont, 0);
    gf_gobj_set_size(ex_sw_box, 80, 50);
    lv_obj_set_style_bg_color(ex_sw_box, lv_color_hex(0xFFFFFF), 0);
    lv_obj_clear_flag(ex_sw_box, LV_OBJ_FLAG_SCROLLABLE);
    gf_gobj_align_to(ex_sw_box, ex_text_box1, LV_ALIGN_OUT_BOTTOM_MID, 15, 25);
    //--------------------------------------------------------------------------
    ex_sw1 = gf_create_switch(ex_sw_box, 0);
    gf_gobj_set_size(ex_sw1, 60, 30);
    gf_gobj_set_pos(ex_sw1, 10, 10);
    //--------------------------------------------------------------------------
    ex_sym_box1 = gf_create_box(ex_comp_cont, 0);
    gf_gobj_set_size(ex_sym_box1, 50, 50);
    lv_obj_set_style_bg_color(ex_sym_box1, lv_color_hex(0xFFFFFF), 0);
    lv_obj_clear_flag(ex_sym_box1, LV_OBJ_FLAG_SCROLLABLE);
    gf_gobj_align_to(ex_sym_box1, ex_sw_box, LV_ALIGN_OUT_BOTTOM_MID, 30, 25);
    //--------------------------------------------------------------------------
    ex_sym1 = gf_create_sym(ex_sym_box1, 0, 10, 10, &terminal_icons_32, \
                            ICON_TOOLBOX_SOLID, lv_color_hex(0xFFFF00));
    //--------------------------------------------------------------------------
    ex_btn1 = gf_create_btn(ex_comp_cont, 0);
    gf_gobj_set_size(ex_btn1, 80, 50);
    gf_gobj_align_to(ex_btn1, ex_sym_box1, LV_ALIGN_OUT_BOTTOM_RIGHT, 10, 25);
    gf_register_handler(ex_btn1, 0, btn_handler, LV_EVENT_CLICKED);
    //--------------------------------------------------------------------------
    ex_slider1 = gf_create_slider(ex_comp_cont, 0);
    gf_gobj_set_size(ex_slider1, 100, 20);
    gf_gobj_align_to(ex_slider1, ex_btn1, LV_ALIGN_OUT_RIGHT_MID, 30, 0);
}
#endif
