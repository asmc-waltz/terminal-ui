/**
 * @file obj.c
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

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

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
g_ctx_t *app_ctx = NULL;

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
/*
 * gf_register_obj - Register a new object into the hierarchy
 * @par: Parent lv_obj_t (NULL for root object)
 * @obj: lv_obj_t to register
 * @id:  ID to assign for this object
 *
 * This function creates a wrapper (g_obj_t) for the given LVGL object and
 * links it into the hierarchy. If @par is NULL, the object is added at the
 * root level; otherwise, it is added as a child of @par. The user_data of
 * @obj is set to point to the created g_obj_t.
 *
 * Return: Pointer to the created g_obj_t on success, NULL on failure.
 */
g_obj_t *gf_register_obj(lv_obj_t *par, lv_obj_t *obj, uint32_t id)
{
    struct list_head *parent_list = NULL;
    g_obj_t *new_obj = NULL;

    if (!obj)
        return NULL;

    parent_list = (!par) ? &app_ctx->objs :
        &((g_obj_t *)par->user_data)->child;

    new_obj = malloc(sizeof(g_obj_t));
    if (!new_obj)
        return NULL;

    new_obj->id = id;
    new_obj->obj = obj;
    obj->user_data = new_obj;

    INIT_LIST_HEAD(&new_obj->child);
    list_add_tail(&new_obj->node, parent_list);

    return new_obj;
}

/*
 * gf_get_obj - Find LVGL object by its ID
 * @req_id:   ID of the object to find
 * @head_lst: Pointer to the list to start scanning (NULL for root list)
 *
 * This function recursively searches for an object with the specified ID in
 * the given list (or the global root list if head_lst is NULL). If found, the
 * function returns the associated lv_obj_t pointer.
 *
 * Return: Pointer to lv_obj_t if found, NULL otherwise.
 */
lv_obj_t *gf_get_obj(uint32_t req_id, struct list_head *head_lst)
{
    struct list_head *scan_list = NULL;
    g_obj_t *obj = NULL;
    lv_obj_t *found = NULL;

    if (!head_lst) {
        LOG_TRACE("Scan from root object");
        scan_list = &app_ctx->objs;
    } else {
        scan_list = head_lst;
    }

    list_for_each_entry(obj, scan_list, node) {
        if (!obj->id)
            continue;

        if (obj->id == req_id)
            return obj->obj;

        found = gf_get_obj(req_id, &obj->child);
        if (found)
            return found;
    }

    return NULL;
}

#define ID_NOID                         0
/*
 * gf_remove_obj_and_child - Remove object and its child objects
 * @req_id:   ID of the object to remove; use ID_NOID to remove all children
 * @head_lst: Pointer to the list to start scanning (NULL for root list)
 *
 * This function searches the given list (or the global root list if head_lst
 * is NULL) for an object with ID matching req_id. If found, the object and all
 * of its child objects are deleted recursively. When req_id is ID_NOID, all
 * child objects under head_lst are removed.
 *
 * Return: true if the object with req_id was found and deleted, false otherwise.
 */
bool gf_remove_obj_and_child(uint32_t req_id, struct list_head *head_lst)
{
    struct list_head *scan_list = NULL;
    g_obj_t *obj = NULL;
    g_obj_t *tmp = NULL;

    if (!head_lst) {
        LOG_TRACE("Scan from root object");
        scan_list = &app_ctx->objs;
    } else {
        LOG_TRACE("Scan from parent");
        scan_list = head_lst;
    }

    list_for_each_entry_safe(obj, tmp, scan_list, node) {
        if (!obj->id)
            continue;

        if (obj->id == req_id || req_id == ID_NOID) {
            if (obj->id == req_id)
                LOG_TRACE("### ID %d: Found object — deleting...", obj->id);

            /* Delete all children first */
            gf_remove_obj_and_child(ID_NOID, &obj->child);

            if (lv_obj_is_valid(obj->obj)) {
                LOG_TRACE("ID %d: deleting lvgl object", obj->id);
                lv_obj_delete(obj->obj);
            }

            list_del(&obj->node);
            free(obj);

            if (req_id != ID_NOID) {
                LOG_TRACE("ID %d: Object and children deleted", req_id);
                return true;
            }

            continue;
        }

        if (gf_remove_obj_and_child(req_id, &obj->child))
            return true;
    }

    return false;
}
