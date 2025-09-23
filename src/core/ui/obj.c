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
#include "log.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
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
/*
 * register_obj - Register a new object into the hierarchy
 * @par: Parent lv_obj_t (NULL for root object)
 * @obj: lv_obj_t to register
 * @name: name to assign for this object
 *
 * This function creates a wrapper (g_obj) for the given LVGL object and
 * links it into the hierarchy. If @par is NULL, the object is added at the
 * root level; otherwise, it is added as a child of @par. The user_data of
 * @obj is set to point to the created g_obj.
 *
 * Return: Pointer to the created gobj_t on success, NULL on failure.
 */
gobj_t *register_obj(lv_obj_t *par, lv_obj_t *obj, const char *name)
{
    struct list_head *parent_list;
    gobj_t *new_obj;
    obj_ctx_t *obj_ctx = &get_ctx()->objs;

    if (!obj)
        return NULL;

    new_obj = calloc(1, sizeof(gobj_t));
    if (!new_obj)
        return NULL;

    if (name) {
        new_obj->name = strdup(name);
        if (!new_obj->name) {
            free(new_obj);
            return NULL;
        }
    } else {
        new_obj->name = NULL;
    }

    new_obj->id = obj_ctx->next_id++;
    new_obj->obj = obj;
    obj->user_data = new_obj;
    new_obj->par = (!par) ? NULL : (gobj_t *)par->user_data;


    INIT_LIST_HEAD(&new_obj->child);

    parent_list = (!par) ? &obj_ctx->list:
        &((gobj_t *)par->user_data)->child;

    list_add_tail(&new_obj->node, parent_list);

    return new_obj;
}

/*
 * get_obj_by_id - Find LVGL object by its ID
 * @req_id:   ID of the object to find
 * @head_lst: Pointer to the list to start scanning (NULL for root list)
 *
 * This function recursively searches for an object with the specified ID in
 * the given list (or the global root list if head_lst is NULL). If found, the
 * function returns the associated lv_obj_t pointer.
 *
 * Return: Pointer to lv_obj_t if found, NULL otherwise.
 */
lv_obj_t *get_obj_by_id(uint32_t req_id, struct list_head *head_lst)
{
    struct list_head *scan_list;
    gobj_t *obj;
    lv_obj_t *found = NULL;
    obj_ctx_t *obj_ctx = &get_ctx()->objs;

    if (head_lst || obj_ctx) {
        scan_list = head_lst ? head_lst : &obj_ctx->list;
    } else {
        return NULL;
    }

    list_for_each_entry(obj, scan_list, node) {
        if (!obj->id)
            continue;

        if (obj->id == req_id)
            return obj->obj;

        found = get_obj_by_id(req_id, &obj->child);
        if (found)
            return found;
    }

    return NULL;
}

/* Find object by name */
lv_obj_t *get_obj_by_name(const char *name, struct list_head *head_lst)
{
    struct list_head *scan_list;
    gobj_t *obj;
    lv_obj_t *found = NULL;
    obj_ctx_t *obj_ctx = &get_ctx()->objs;

    if (!name)
        return NULL;

    if (head_lst || obj_ctx) {
        scan_list = head_lst ? head_lst : &obj_ctx->list;
    } else {
        return NULL;
    }

    list_for_each_entry(obj, scan_list, node) {
        LOG_TRACE("Finding name %s: checking %d %s", name, obj->id, obj->name);
        if (!obj->id) {
            LOG_TRACE("Finding name %s: check %d %s: CONTINUE", \
                      name, obj->id, obj->name);
            continue;
        }

        if (obj->name && strcmp(obj->name, name) == 0) {

            LOG_TRACE("Finding name %s: check %d %s: OBJ FOUND", \
                      name, obj->id, obj->name);
            return obj->obj;
        }

        found = get_obj_by_name(name, &obj->child);
        if (found) {
            LOG_TRACE("Finding name %s: check list %d %s: OBJ FROM DEEPER LEVEL", \
                      name, get_gobj(found)->id, get_gobj(found)->name);
            return found;
        }
    }

    return NULL;
}

/*
 * remove_obj_and_child_by_name - Remove object (by name) and its children
 * @name:    Name of the object to remove
 * @head_lst: Pointer to the list to start scanning (NULL for root list)
 *
 * This function searches for an object with the given name in the hierarchy.
 * If found, the object and all of its children are deleted recursively.
 *
 * Return:
 *   0   → object found by name and deleted
 *  -1   → not found or invalid args
 */
int32_t remove_obj_and_child_by_name(const char *name, \
                                        struct list_head *head_lst)
{
    struct list_head *scan_list;
    gobj_t *obj;
    obj_ctx_t *obj_ctx = &get_ctx()->objs;

    if (!name)
        return -1;

    if (head_lst || obj_ctx) {
        scan_list = head_lst ? head_lst : &obj_ctx->list;
    } else {
        return -1;
    }

    list_for_each_entry(obj, scan_list, node) {
        if (!obj->id)
            continue;

        if (obj->name && strcmp(obj->name, name) == 0) {
            LOG_TRACE("Removing object by name: %s (ID %u)", name, obj->id);
            return remove_obj_and_child(obj->id, scan_list);
        }

        int32_t ret = remove_obj_and_child_by_name(name, &obj->child);
        if (ret == 0)   /* found and deleted by name */
            return 0;
    }

    return -1;
}

#define ID_NOID                         0
/*
 * remove_obj_and_child - Remove an object and all its children
 * @req_id:   ID of the object to remove; use ID_NOID to remove all children
 * @head_lst: Pointer to the list to start scanning (NULL for root list)
 *
 * This function searches for an object with the specified ID in the given list
 * (or the global root list if head_lst is NULL). If found, the object and all
 * of its child objects are deleted recursively. When req_id is ID_NOID, all
 * child objects under head_lst are removed.
 *
 * Return:
 *   - If req_id is a specific ID:
 *       0   → object found and deleted
 *      -1   → object not found
 *
 *   - If req_id == ID_NOID:
 *       >=0 → number of objects deleted
 *       -1  → error (invalid context)
 */
int32_t remove_obj_and_child(uint32_t req_id, struct list_head *head_lst)
{
    struct list_head *scan_list = NULL;
    gobj_t *obj = NULL;
    gobj_t *tmp = NULL;
    obj_ctx_t *obj_ctx = &get_ctx()->objs;
    int32_t removed = 0;

    if (head_lst || obj_ctx) {
        scan_list = head_lst ? head_lst : &obj_ctx->list;
    } else {
        return -1;
    }

    LOG_TRACE("Removing object%s",
        head_lst ? " (scan from parent)" : " (scan from root)");

    list_for_each_entry_safe(obj, tmp, scan_list, node) {
        if (!obj->id)
            continue;

        if (obj->id == req_id || req_id == ID_NOID) {
            if (obj->id == req_id)
                LOG_TRACE("ID %u: found, deleting...", obj->id);

            /* Remove all children first */
            int32_t child_removed = remove_obj_and_child(ID_NOID,
                &obj->child);
            if (child_removed > 0)
                removed += child_removed;

            if (lv_obj_is_valid(obj->obj)) {
                LOG_TRACE("ID %u: deleting LVGL object", obj->id);
                lv_obj_delete(obj->obj);
            }

            LOG_TRACE("DELETE obj ID %d - name %s", obj->id,
                obj->name ? obj->name : "(null)");
            list_del(&obj->node);
            if (obj->name)
                free(obj->name);
            free(obj);

            removed++;

            if (req_id != ID_NOID) {
                LOG_TRACE("ID %u: object and children deleted", req_id);
                return 0;   /* Specific ID → report success */
            }

            continue;
        }

        /* Recursive search in children */
        int32_t ret = remove_obj_and_child(req_id, &obj->child);
        if (req_id != ID_NOID && ret == 0)
            return 0;       /* Found and deleted by ID */
        if (req_id == ID_NOID && ret > 0)
            removed += ret;
    }

    if (req_id == ID_NOID)
        return removed; /* Return total number of objects removed */
    return -1;          /* Specific ID not found */
}

/*
 * remove_children - Remove all children of a given parent object
 * @parent: Pointer to the parent g_obj
 *
 * This function removes all child objects of the given parent, including
 * their descendants, but does not delete the parent itself.
 *
 * Return:
 *   >=0 → number of objects deleted
 *   -1  → error (invalid parent or context)
 */
int32_t remove_children(gobj_t *par)
{
    if (!par)
        return -1;

    return remove_obj_and_child(ID_NOID, &par->child);
}

/**
 * init_ui_object_list - Allocate and initialize the global ui object list 
 */
int32_t init_ui_object_ctx(ctx_t *ctx)
{
    if (ctx == NULL)
        return -EINVAL;

    INIT_LIST_HEAD(&ctx->objs.list);

    return 0;
}

/**
 * destroy_ui_object_ctx - Free the global application context and all resources
 * @ctx: Pointer to the application context to destroy
 *
 * This function removes all registered objects and handlers from the given
 * context, then frees the context itself. Safe to call with NULL pointer.
 */
void destroy_ui_object_ctx(ctx_t *ctx)
{
    if (!ctx)
        return;

    remove_obj_and_child(ID_NOID, &ctx->objs.list);
    ctx->objs.next_id = 1;
}

gobj_t *get_gobj(lv_obj_t *lobj)
{
    if (!lobj)
        return NULL;
    return lobj->user_data;
}

gobj_t *get_gobj_parent(lv_obj_t *lobj)
{
    gobj_t *gobj;

    if (!lobj)
        return NULL;

    gobj = lobj->user_data;
    if (!gobj)
        return NULL;

    return gobj->par;
}

void set_gobj_data(lv_obj_t *lobj, void *data)
{
    gobj_t *gobj;

    if (!lobj) {
        LOG_ERROR("lv_obj_t object invalid");
        return;
    }

    gobj = lobj->user_data;
    if (!gobj) {
        LOG_ERROR("gobj_t object invalid");
        return;
    }

    gobj->obj_data = data;
}

void *get_gobj_data(lv_obj_t *lobj)
{
    gobj_t *gobj;

    if (!lobj)
        return NULL;

    gobj = lobj->user_data;
    if (!gobj)
        return NULL;

    return gobj->obj_data;
}
