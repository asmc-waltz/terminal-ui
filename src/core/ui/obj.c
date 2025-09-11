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
#include <string.h>

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
static g_ctx *app_ctx = NULL;
static uint32_t g_next_id = 1;

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
 * @name: name to assign for this object
 *
 * This function creates a wrapper (g_obj) for the given LVGL object and
 * links it into the hierarchy. If @par is NULL, the object is added at the
 * root level; otherwise, it is added as a child of @par. The user_data of
 * @obj is set to point to the created g_obj.
 *
 * Return: Pointer to the created g_obj on success, NULL on failure.
 */
g_obj *gf_register_obj(lv_obj_t *par, lv_obj_t *obj, const char *name)
{
    struct list_head *parent_list;
    g_obj *new_obj;
    g_ctx *ctx = gf_get_app_ctx();

    if (!obj)
        return NULL;

    new_obj = calloc(1, sizeof(g_obj));
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

    new_obj->id = g_next_id++;
    new_obj->obj = obj;
    obj->user_data = new_obj;
    new_obj->par = (!par) ? NULL : (g_obj *)par->user_data;


    INIT_LIST_HEAD(&new_obj->child);

    parent_list = (!par) ? &ctx->objs :
        &((g_obj *)par->user_data)->child;

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
    struct list_head *scan_list;
    g_obj *obj;
    lv_obj_t *found = NULL;
    g_ctx *ctx = gf_get_app_ctx();

    scan_list = head_lst ? head_lst : &ctx->objs;

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

/* Find object by name */
lv_obj_t *gf_get_obj_by_name(const char *name, struct list_head *head_lst)
{
    struct list_head *scan_list;
    g_obj *obj;
    lv_obj_t *found = NULL;
    g_ctx *ctx = gf_get_app_ctx();

    if (!name)
        return NULL;

    scan_list = head_lst ? head_lst : &ctx->objs;

    list_for_each_entry(obj, scan_list, node) {
        if (!obj->id)
            continue;

        if (obj->name && strcmp(obj->name, name) == 0)
            return obj->obj;

        found = gf_get_obj_by_name(name, &obj->child);
        if (found)
            return found;
    }

    return NULL;
}

/*
 * gf_remove_obj_and_child_by_name - Remove object (by name) and its children
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
int32_t gf_remove_obj_and_child_by_name(const char *name, \
                                        struct list_head *head_lst)
{
    g_ctx *ctx = gf_get_app_ctx();
    struct list_head *scan_list;
    g_obj *obj;

    if (!ctx || !name)
        return -1;

    scan_list = head_lst ? head_lst : &ctx->objs;

    list_for_each_entry(obj, scan_list, node) {
        if (!obj->id)
            continue;

        if (obj->name && strcmp(obj->name, name) == 0) {
            LOG_TRACE("Removing object by name: %s (ID %u)", name, obj->id);
            return gf_remove_obj_and_child(obj->id, scan_list);
        }

        int32_t ret = gf_remove_obj_and_child_by_name(name, &obj->child);
        if (ret == 0)   /* found and deleted by name */
            return 0;
    }

    return -1;
}

#define ID_NOID                         0
/*
 * gf_remove_obj_and_child - Remove an object and all its children
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
int32_t gf_remove_obj_and_child(uint32_t req_id, struct list_head *head_lst)
{
    struct list_head *scan_list = NULL;
    g_obj *obj = NULL;
    g_obj *tmp = NULL;
    g_ctx *ctx = gf_get_app_ctx();
    int32_t removed = 0;

    if (!ctx)
        return -1;

    scan_list = head_lst ? head_lst : &ctx->objs;
    LOG_TRACE("Removing object%s",
        head_lst ? " (scan from parent)" : " (scan from root)");

    list_for_each_entry_safe(obj, tmp, scan_list, node) {
        if (!obj->id)
            continue;

        if (obj->id == req_id || req_id == ID_NOID) {
            if (obj->id == req_id)
                LOG_TRACE("ID %u: found, deleting...", obj->id);

            /* Remove all children first */
            int32_t child_removed = gf_remove_obj_and_child(ID_NOID,
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
        int32_t ret = gf_remove_obj_and_child(req_id, &obj->child);
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
 * gf_remove_children - Remove all children of a given parent object
 * @parent: Pointer to the parent g_obj
 *
 * This function removes all child objects of the given parent, including
 * their descendants, but does not delete the parent itself.
 *
 * Return:
 *   >=0 → number of objects deleted
 *   -1  → error (invalid parent or context)
 */
int32_t gf_remove_children(g_obj *par)
{
    if (!par)
        return -1;

    return gf_remove_obj_and_child(ID_NOID, &par->child);
}

/**
 * gf_create_app_ctx - Allocate and initialize the global application context
 *
 * This function creates an application context structure, initializes its
 * internal lists for objects and handlers, and returns a pointer to the
 * newly created context.
 *
 * Return: Pointer to g_ctx on success, NULL on failure.
 */
g_ctx *gf_create_app_ctx(void)
{
    g_ctx *ctx = NULL;

    ctx = calloc(1, sizeof(g_ctx));
    if (!ctx)
        return NULL;

    INIT_LIST_HEAD(&ctx->objs);

    return ctx;
}

/**
 * gf_destroy_app_ctx - Free the global application context and all resources
 * @ctx: Pointer to the application context to destroy
 *
 * This function removes all registered objects and handlers from the given
 * context, then frees the context itself. Safe to call with NULL pointer.
 */

void gf_destroy_app_ctx(g_ctx *ctx)
{
    if (!ctx)
        return;

    gf_remove_obj_and_child(ID_NOID, &ctx->objs);
    free(ctx);
    g_next_id = 1;
}

/**
 * gf_set_app_ctx - Set the global application context
 * @ctx: Pointer to the application context
 *
 * This function stores the given context as the global application context.
 * Must be called after gf_create_app_ctx() and before any context-dependent
 * operations.
 */
void gf_set_app_ctx(g_ctx *ctx)
{
    app_ctx = ctx;
}

/**
 * gf_get_app_ctx - Get the current global application context
 *
 * Return: Pointer to the global application context, or NULL if not set.
 */
g_ctx *gf_get_app_ctx(void)
{
    return app_ctx;
}

g_obj *get_gobj(lv_obj_t *lobj)
{
    if (!lobj)
        return NULL;
    return lobj->user_data;
}

g_obj *get_par_gobj(lv_obj_t *lobj)
{
    g_obj *gobj;

    if (!lobj)
        return NULL;

    gobj = lobj->user_data;
    if (!gobj)
        return NULL;

    return gobj->par;
}
