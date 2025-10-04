/**
 * @file obj_list.c
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
int32_t set_gobj_list_layout(lv_obj_t *lobj, int8_t flow)
{
    gobj_t *gobj;

    if (!lobj)
        return -EINVAL;

    if (flow <= LIST_NONE || flow >= LIST_END)
        return -EINVAL;

    gobj = get_gobj(lobj);
    if (!gobj)
        return -EIO;

    gobj->align.list = flow;

    lv_obj_set_scrollbar_mode(lobj, LV_SCROLLBAR_MODE_OFF);

    return 0;
}

/**
 * Return the "next" ref when traversing in revert order:
 * - If lobj is NOT the last child -> return next sibling
 * - Else -> return parent
 *
 * Returns NULL on invalid args or if index is invalid.
 */
lv_obj_t *get_revert_obj_ref(lv_obj_t *par, lv_obj_t *lobj)
{
    lv_obj_t *ref = NULL;
    int32_t obj_after;
    int32_t idx;

    if (!par || !lobj)
        return NULL;

    idx = lv_obj_get_index(lobj);
    if (idx < 0)
        return NULL;

    /* if lobj is not last child, use next sibling */
    if (lv_obj_get_child(par, -1) != lobj) {
        obj_after = idx + 1;
        ref = lv_obj_get_child(par, obj_after);
        if (!ref)
            return NULL;
    } else {
        /* last child -> use parent as ref */
        ref = par;
    }

    return ref;
}

/**
 * Return the "previous" ref when traversing in normal order:
 * - If lobj is NOT the first child -> return prev sibling
 * - Else -> return parent
 *
 * Returns NULL on invalid args or if index is invalid.
 */
lv_obj_t *get_normal_obj_ref(lv_obj_t *par, lv_obj_t *lobj)
{
    lv_obj_t *ref = NULL;
    int32_t obj_before;
    int32_t idx;

    if (!par || !lobj)
        return NULL;

    idx = lv_obj_get_index(lobj);
    if (idx < 0)
        return NULL;

    if (lv_obj_get_child(par, 0) != lobj) {
        obj_before = idx - 1;
        ref = lv_obj_get_child(par, obj_before);
        if (!ref)
            return NULL;
    } else {
        /* first child -> use parent as ref */
        ref = par;
    }

    return ref;
}

/**
 * Compute align for revert traversal.
 */
int32_t get_revert_obj_align(lv_obj_t *par, lv_obj_t *lobj, int8_t flow)
{
    int32_t align;

    if (!par || !lobj)
        return -EINVAL;

    if (lv_obj_get_child(par, -1) != lobj) {
        if (flow == LIST_COLUMN) {
            align = LV_ALIGN_OUT_BOTTOM_MID;
        } else if (flow == LIST_ROW) {
            align = LV_ALIGN_OUT_RIGHT_MID;
        }
    } else {
        // Last obj get ref
        if (flow == LIST_COLUMN) {
            align = LV_ALIGN_TOP_MID;
        } else if (flow == LIST_ROW) {
            align = LV_ALIGN_LEFT_MID;
        }
    }

    return align;
}
/**
 * Compute align for normal traversal.
 */
int32_t get_normal_obj_align(lv_obj_t *par, lv_obj_t *lobj, int8_t flow)
{
    int32_t align;

    if (!par || !lobj)
        return -EINVAL;

    if (lv_obj_get_child(par, 0) != lobj) {
        if (flow == LIST_COLUMN) {
            align = LV_ALIGN_OUT_BOTTOM_MID;
        } else if (flow == LIST_ROW) {
            align = LV_ALIGN_OUT_RIGHT_MID;
        }
    } else {
        // First obj get align
        if (flow == LIST_COLUMN) {
            align = LV_ALIGN_TOP_MID;
        } else if (flow == LIST_ROW) {
            align = LV_ALIGN_LEFT_MID;
        }
    }

    return align;
}

/**
 * Align a child object inside a gobj list based on effective list
 * and the current screen rotation.
 *
 * Returns 0 on success, negative errno on failure.
 */
int32_t align_gobj_list_item(lv_obj_t *par, lv_obj_t *lobj, int32_t x_ofs, \
                             int32_t y_ofs)
{
    gobj_t *gobj_par;
    int32_t flow;
    lv_obj_t *ref = NULL;
    int32_t align;
    int32_t scr_rot;

    if (!par || !lobj)
        return -EINVAL;

    gobj_par = get_gobj(par);
    if (!gobj_par)
        return -EIO;

    flow = gobj_par->align.list;
    if (flow == LIST_NONE) {
        LOG_ERROR("Unable to add list object into normal parent");
        return -EIO;
    }

    scr_rot = get_scr_rotation();

    /* choose ref and align according to rotation groups */
    if (scr_rot == ROTATION_0 || scr_rot == ROTATION_270) {
        ref = get_normal_obj_ref(par, lobj);
        if (!ref) {
            LOG_ERROR("Unable to find ref object for %s",
                      get_gobj(lobj) ? get_gobj(lobj)->name : "(null)");
            return -EIO;
        }

        align = get_normal_obj_align(par, lobj, flow);
    } else if (scr_rot == ROTATION_90 || scr_rot == ROTATION_180) {
        ref = get_revert_obj_ref(par, lobj);
        if (!ref) {
            LOG_ERROR("Unable to find ref object for %s",
                      get_gobj(lobj) ? get_gobj(lobj)->name : "(null)");
            return -EIO;
        }

        align = get_revert_obj_align(par, lobj, flow);
    } else {
        /* unexpected rotation value */
        LOG_ERROR("Unsupported rotation %d", scr_rot);
        return -EIO;
    }

    LOG_TRACE("Align object: %s to %s",
              get_gobj(lobj) ? get_gobj(lobj)->name : "(null)",
              get_gobj(ref) ? get_gobj(ref)->name : "(null)");

    /*
     * Unlike normal objects where rotation does not affect alignment,
     * rotating by 90 or 180 degrees will cause the alignment value
     * to become negative.
     *
     * For list objects, however, the alignment is adjusted to the
     * root coordinate system, so the alignment value must remain positive.
     */
    set_gobj_align(lobj, ref, align, abs(x_ofs), abs(y_ofs));

    return 0;
}

/**
 * Walk children in normal order and align them.
 *
 * Returns 0 on success, negative errno on invalid args.
 */
int32_t update_normal_list_obj_align(gobj_t *gobj)
{
    lv_obj_t *par;
    lv_obj_t *child;
    int32_t child_cnt, ret;
    int32_t i;

    par = gobj ? get_lobj(gobj) : NULL;
    if (!par)
        return -EINVAL;

    child_cnt = lv_obj_get_child_count(par);

    LOG_TRACE("Child count = %d", child_cnt);

    for (i = 0; i < child_cnt; i++) {
        child = lv_obj_get_child(par, i);
        if (!child) {
            LOG_WARN("Null child at index %d", i);
            continue;
        }

        LOG_TRACE("NORMAL Walk object, %s",
                  get_gobj(child) ?
                  get_gobj(child)->name : "(null)");

        ret = align_gobj_list_item(par, child, get_gobj(child)->align.x, \
                                   get_gobj(child)->align.y);
        if (ret) {
            LOG_ERROR("Unable to align object %s", get_gobj(child)->name);
        }
    }

    return 0;
}
/**
 * Walk children in reverse order and align them.
 *
 * Returns 0 on success, negative errno on invalid args.
 */
int32_t update_revert_list_obj_align(gobj_t *gobj)
{
    lv_obj_t *par;
    lv_obj_t *child;
    int32_t child_cnt, ret;
    int32_t i;

    par = gobj ? get_lobj(gobj) : NULL;
    if (!par)
        return -EINVAL;

    child_cnt = lv_obj_get_child_count(par);

    LOG_TRACE("Child count = %d", child_cnt);

    for (i = child_cnt - 1; i >= 0; i--) {
        child = lv_obj_get_child(par, i);
        if (!child) {
            LOG_WARN("Null child at index %d", i);
            continue;
        }

        LOG_TRACE("REVERT Walk object, %s",
                  get_gobj(child) ?
                  get_gobj(child)->name : "(null)");

        ret = align_gobj_list_item(par, child, get_gobj(child)->align.x, \
                                   get_gobj(child)->align.y);
        if (ret) {
            LOG_ERROR("Unable to align object %s", get_gobj(child)->name);
        }
    }

    return 0;
}

/**
 * Set scroll dir for a list list based on its effective list.
 *
 * Returns 0 on success, -EINVAL for invalid args, -EIO for invalid list.
 */
int32_t set_list_scroll_dir(gobj_t *gobj)
{
    lv_obj_t *lobj;
    int32_t cur_list;

    lobj = gobj ? get_lobj(gobj) : NULL;
    if (!lobj)
        return -EINVAL;

    cur_list = gobj->align.list;
    if (cur_list <= LIST_NONE || cur_list >= LIST_END)
        return -EIO;

    if (cur_list == LIST_COLUMN) {
        lv_obj_set_scroll_dir(lobj, LV_DIR_TOP | LV_DIR_BOTTOM);
    } else if (cur_list == LIST_ROW) {
        lv_obj_set_scroll_dir(lobj, LV_DIR_LEFT | LV_DIR_RIGHT);
    } else {
        /* defensive: unknown flow */
        LOG_WARN("Unknown list %d", cur_list);
        return -EIO;
    }

    return 0;
}

/**
 * Update stored list according to rotation groups.
 *
 * Group A: ROTATION_0, ROTATION_180
 * Group B: ROTATION_90, ROTATION_270
 *
 * If scr_rot and cur_rot are in same group -> keep cur_list.
 * Else swap COLUMN <-> ROW.
 *
 * Returns 0 on success, -EINVAL for invalid args.
 */
int32_t update_list_by_rot(gobj_t *gobj)
{
    int32_t scr_rot;
    int32_t cur_rot;
    int32_t cur_list;
    bool same_group;

    if (!gobj)
        return -EINVAL;

    scr_rot = get_scr_rotation();
    cur_rot = gobj->data.rotation;
    cur_list = gobj->align.list;

    /* validate current list */
    if (cur_list != LIST_COLUMN && cur_list != LIST_ROW) {
        LOG_WARN("gobj has non-standard list %d", cur_list);
        /* keep as-is and return success to avoid forcing invalid value */
        return -EIO;
    }

    same_group = ((scr_rot == ROTATION_0 || scr_rot == ROTATION_180) &&
                  (cur_rot == ROTATION_0 || cur_rot == ROTATION_180)) ||
                 ((scr_rot == ROTATION_90 || scr_rot == ROTATION_270) &&
                  (cur_rot == ROTATION_90 || cur_rot == ROTATION_270));

    if (same_group)
        gobj->align.list = cur_list;
    else
        gobj->align.list = (cur_list == LIST_COLUMN) ? LIST_ROW : LIST_COLUMN;

    return 0;
}

/**
 * Update list alignment according to current screen rotation.
 *
 * - Choose traversal order: normal (0/270) or revert (90/180).
 * - Walk children and align.
 * - Update layout and scroll first child into view if exists.
 *
 * Returns 0 on success, -EINVAL if gobj invalid.
 */
int32_t update_list_align_by_rot(gobj_t *gobj)
{
    lv_obj_t *lobj;
    lv_obj_t *first_child;
    int32_t scr_rot;
    int32_t ret;

    lobj = gobj ? get_lobj(gobj) : NULL;
    if (!lobj)
        return -EINVAL;

    scr_rot = get_scr_rotation();

    if (scr_rot == ROTATION_0 || scr_rot == ROTATION_270) {
        ret = update_normal_list_obj_align(gobj);
        if (ret)
            LOG_WARN("update_normal_align returned %d", ret);
    } else if (scr_rot == ROTATION_90 || scr_rot == ROTATION_180) {
        ret = update_revert_list_obj_align(gobj);
        if (ret)
            LOG_WARN("update_revert_align returned %d", ret);
    } else {
        LOG_ERROR("Unsupported rotation %d", scr_rot);
        return -EIO;
    }

    /* update layout and ensure first child visible if present */
    lv_obj_update_layout(lobj);

    first_child = lv_obj_get_child(lobj, 0);
    if (first_child)
        lv_obj_scroll_to_view(first_child, LV_ANIM_OFF);

    return 0;
}
