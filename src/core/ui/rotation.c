/**
 * @file rotation.c
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

#include <list.h>
#include <ui/ui_core.h>

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
    g_obj *gobj_par = NULL;

    if (!gobj) {
        LOG_ERROR("Invalid g object");
        return -EINVAL;
    }

    gobj_par = gobj->par;
    if (!gobj_par) {
        LOG_ERROR("Invalid g parent object");
        return -EINVAL;
    }

    g_obj_get_center(gobj, gobj_par->pos.w, gobj_par->pos.h);

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

    // The size and scale calculation depends on alignment values,
    // so we must process these first.
    if (gobj->aln.align != LV_ALIGN_DEFAULT) {
        ret = g_obj_rot_calc_align(gobj);
        if (ret) {
            return -EINVAL;
        }
    }
    /*
     * For each object, when rotation occurs, its size must be recalculated.
     * Since the root coordinate does not change, the width and height
     * will be adjusted according to the logical rotation.
     */
    ret = g_obj_rot_calc_size(gobj);
    if (ret) {
        return -EINVAL;
    }

    if (gobj->scale.ena_h || gobj->scale.ena_w) {
        lv_obj_set_size(gobj->obj, gobj->scale.w, gobj->scale.h);
        LOG_INFO("SET W=%d H=%d", gobj->scale.w, gobj->scale.h);
    } else {
        lv_obj_set_size(gobj->obj, gobj->pos.w, gobj->pos.h);
    }

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

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int32_t g_set_scr_rot_dir(int32_t rot_dir)
{
    g_scr_rot_dir = rot_dir;
    return 0;
}

int32_t g_get_scr_rot_dir()
{
    return g_scr_rot_dir;
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
