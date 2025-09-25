/**
 * @file layout.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define LOG_LEVEL LOG_LEVEL_TRACE
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
int32_t set_gobj_list_flow(lv_obj_t *lobj, int8_t flow)
{
    if (!lobj)
        return -EINVAL;

    if (flow <= FLEX_NONE || flow >= FLEX_END)
        return -EINVAL;

    get_gobj(lobj)->aln.flex = flow;

    return 0;
}

lv_obj_t *get_revert_align_ref_flex_obj_lst(lv_obj_t *par, lv_obj_t *lobj)
{
    lv_obj_t *ref;

    if (lv_obj_get_child(par, -1) != lobj) {
        //  obj get obj after as ref
        int32_t obj_after = (int32_t)lv_obj_get_index(lobj) + 1;
        ref = lv_obj_get_child(par, obj_after);
    } else {
        // Last obj get ref
        ref = par;
    }

    return ref;
}

lv_obj_t *get_normal_align_ref_flex_obj_lst(lv_obj_t *par, lv_obj_t *lobj)
{
    lv_obj_t *ref;

    if (lv_obj_get_child(par, 0) != lobj) {
        // obj get obj before as ref
        int32_t obj_before = (int32_t)lv_obj_get_index(lobj) - 1;
        ref = lv_obj_get_child(par, obj_before);
    } else {
        // First obj get ref
        ref = par;
    }

    return ref;
}


int32_t get_revert_align_flex_obj_lst(lv_obj_t *par, lv_obj_t *lobj, int8_t flow)
{
    int32_t align;

    if (lv_obj_get_child(par, -1) != lobj) {
        if (flow == FLEX_COLUMN) {
            align = LV_ALIGN_OUT_BOTTOM_MID;
        } else if (flow == FLEX_ROW) {
            align = LV_ALIGN_OUT_RIGHT_MID;
        }
    } else {
        // Last obj get ref
        if (flow == FLEX_COLUMN) {
            align = LV_ALIGN_TOP_MID;
        } else if (flow == FLEX_ROW) {
            align = LV_ALIGN_LEFT_MID;
        }
    }

    return align;
}

int32_t get_normal_align_flex_obj_lst(lv_obj_t *par, lv_obj_t *lobj, int8_t flow)
{
    int32_t align;

    if (lv_obj_get_child(par, 0) != lobj) {
        if (flow == FLEX_COLUMN) {
            align = LV_ALIGN_OUT_BOTTOM_MID;
        } else if (flow == FLEX_ROW) {
            align = LV_ALIGN_OUT_RIGHT_MID;
        }
    } else {
        // First obj get align
        if (flow == FLEX_COLUMN) {
            align = LV_ALIGN_TOP_MID;
        } else if (flow == FLEX_ROW) {
            align = LV_ALIGN_LEFT_MID;
        }
    }

    return align;
}

int32_t align_obj_in_gobj_list(lv_obj_t *par, lv_obj_t *lobj)
{
    gobj_t *gobj_par = get_gobj(par);

    int32_t flow = gobj_par->aln.flex;

    if (flow == FLEX_NONE) {
        LOG_ERROR("Unable to add list object into normal parent");
        return -EIO;
    }




    lv_obj_t *ref;
    int32_t align;

    int32_t scr_rot = get_scr_rotation();

    if (scr_rot == ROTATION_0 || scr_rot == ROTATION_270) {

        ref = get_normal_align_ref_flex_obj_lst(par, lobj);
        if (!ref) {
            LOG_ERROR("Unable to find ref object for %s", get_gobj(lobj)->name);
            return -EIO;
        }

        align = get_normal_align_flex_obj_lst(par, lobj, flow);
    } else if (scr_rot == ROTATION_90  || scr_rot == ROTATION_180) {
        ref = get_revert_align_ref_flex_obj_lst(par, lobj);
        if (!ref) {
            LOG_ERROR("Unable to find ref object for %s", get_gobj(lobj)->name);
            return -EIO;
        }

        align = get_revert_align_flex_obj_lst(par, lobj, flow);
    }








    LOG_TRACE("Align object: %s to %s", get_gobj(lobj)->name, get_gobj(ref)->name);
    align_gobj_to(lobj, ref, align, 0, 0);
}



int32_t update_revert_obj_list_align(gobj_t *gobj_par)
{
    lv_obj_t *par = gobj_par->obj;
    int32_t child_cnt = lv_obj_get_child_count(par);
    int32_t scr_rot = get_scr_rotation();

    LOG_TRACE("Child count = %d", child_cnt);

    for (int32_t i = child_cnt - 1; i >= 0; i--) {
        lv_obj_t * sibling = lv_obj_get_child(par, i);
        LOG_TRACE("REVERT Walk object, %s", ((gobj_t *)sibling->user_data)->name);


        align_obj_in_gobj_list(par, sibling);
    }
}


int32_t update_normal_obj_list_align(gobj_t *gobj_par)
{

    lv_obj_t *par = gobj_par->obj;
    int32_t child_cnt = lv_obj_get_child_count(par);
    int32_t scr_rot = get_scr_rotation();

    LOG_TRACE("Child count = %d", child_cnt);

    for (int32_t i = 0; i < child_cnt; i++) {
        lv_obj_t * sibling = lv_obj_get_child(par, i);
        LOG_TRACE("NORMAL Walk object, %s", ((gobj_t *)sibling->user_data)->name);


        align_obj_in_gobj_list(par, sibling);
    }
}




int32_t update_list_object_rotation_alignment(gobj_t *gobj)
{
    int32_t scr_rot = get_scr_rotation();
    lv_obj_t *lobj = gobj->obj;

    if (scr_rot == ROTATION_0) {
        gobj->aln.flex = FLEX_COLUMN;
        update_normal_obj_list_align(gobj);
        lv_obj_set_scroll_dir(lobj, LV_DIR_TOP | LV_DIR_BOTTOM);
    } else if (scr_rot == ROTATION_90) {
        gobj->aln.flex = FLEX_ROW;
        update_revert_obj_list_align(gobj);
        lv_obj_set_scroll_dir(lobj, LV_DIR_LEFT | LV_DIR_RIGHT);
    } else if (scr_rot == ROTATION_180) {
        gobj->aln.flex = FLEX_COLUMN;
        update_revert_obj_list_align(gobj);
        lv_obj_set_scroll_dir(lobj, LV_DIR_TOP | LV_DIR_BOTTOM);
    } else if (scr_rot == ROTATION_270) {
        gobj->aln.flex = FLEX_ROW;
        update_normal_obj_list_align(gobj);
        lv_obj_set_scroll_dir(lobj, LV_DIR_LEFT | LV_DIR_RIGHT);
    }
    
    lv_obj_update_layout(lobj);

    lv_obj_t *first_child = lv_obj_get_child(lobj, 0);
    lv_obj_scroll_to_view(first_child, LV_ANIM_OFF);
}
