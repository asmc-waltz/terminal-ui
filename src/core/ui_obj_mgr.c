/**
 * @file ui_obj_mgr.c
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
#include <ui/ui_plat.h>

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
g_handler * gf_register_handler(lv_obj_t *obj, uint32_t id, lv_event_cb_t event_cb, lv_event_code_t filter)
{
    g_obj_t *pg_obj = NULL;
    g_handler *p_handler = NULL;

    LV_ASSERT_NULL(obj);
    LOG_TRACE("Register handler id %d", id);
    p_handler = malloc(sizeof(g_handler));
    LV_ASSERT_NULL(p_handler);
    p_handler->id = id;
    p_handler->obj = obj;

    pg_obj = obj->user_data;

    list_add_tail(&p_handler->node, &global_data->handler_list);
    lv_obj_add_event_cb(obj, event_cb, filter, pg_obj);

    return p_handler;
}

g_obj_t * gf_register_obj(lv_obj_t *par, lv_obj_t *obj, uint32_t id)
{
    g_obj_t *p_obj = NULL;
    struct list_head *par_list;

    if (par == NULL) {
        LOG_TRACE("Root object is detect -> add this one into the global data");
        par_list = &global_data->obj_list;
    } else {
        LOG_TRACE("Normal object is detect -> add this one into parrent data as child");
        par_list = &((g_obj_t *)par->user_data)->child;
    }

    LOG_TRACE("Register OBJ id %d", id);
    p_obj = malloc(sizeof(g_obj_t));
    LV_ASSERT_NULL(p_obj);
    p_obj->id = id;
    p_obj->obj = obj;
    p_obj->obj->user_data = p_obj;

    INIT_LIST_HEAD(&p_obj->child);
    list_add_tail(&p_obj->node, par_list);

    return p_obj;
}

lv_obj_t * gf_create_obj(lv_obj_t *par, uint32_t id)
{
    g_obj_t *p_obj = NULL;
    lv_obj_t *obj = NULL;

    LV_ASSERT_NULL(par);
    LOG_TRACE("Create obj id %d", id);
    obj = lv_obj_create(par);
    LV_ASSERT_NULL(obj);

    p_obj = gf_register_obj(par, obj, id);
    LV_ASSERT_NULL(p_obj);

    return p_obj->obj;
}

lv_obj_t * gf_get_obj(uint32_t req_id, struct list_head *head_lst)
{
    g_obj_t *p_obj = NULL;
    lv_obj_t *lv_obj = NULL;
    struct list_head *par_list;

    if (head_lst == NULL) {
        LOG_TRACE("Scan from Root object");
        par_list = &global_data->obj_list;
    } else {
        LOG_TRACE("Scan from parent");
        par_list = head_lst;
    }

    list_for_each_entry(p_obj, par_list, node)
    if (p_obj->id) {
        if (p_obj->id == req_id) {
            LOG_TRACE("OBJ id %d is detected", p_obj->id);
            break;
        } else if (p_obj->id != req_id) {
            LOG_TRACE("Scan inside obj id %d", p_obj->id);
            lv_obj = gf_get_obj(req_id, &p_obj->child);
            if (lv_obj != NULL) {
                LOG_TRACE("OBJ id %d is the expeted one", ((g_obj_t *)(lv_obj->user_data))->id);
                return lv_obj;
            } else {
                LOG_TRACE("OBJ is NOT the expeted one");
                continue;
            }
        }
    }

    return p_obj->id == req_id ? p_obj->obj : NULL;
}

/*
 * ID_NONE is used to delete child objs that are not related to req_id
 */
bool gf_remove_obj_and_child(uint32_t req_id, struct list_head *head_lst)
{
    bool deleted = false;
    g_obj_t *p_obj = NULL;
    g_obj_t *p_obj_tmp = NULL;
    lv_obj_t *lv_obj = NULL;
    struct list_head *par_list = NULL;

    if (head_lst == NULL) {
        LOG_TRACE("Scan from Root object");
        par_list = &global_data->obj_list;
    } else {
        LOG_TRACE("Scan from parent");
        par_list = head_lst;
    }

    list_for_each_entry_safe(p_obj, p_obj_tmp, par_list, node)
    if (p_obj->id) {
        if (p_obj->id == req_id || req_id == ID_NONE) {
            if (p_obj->id == req_id) {
                LOG_TRACE("### ID %d: Object is detected — initiating deletion |--->", p_obj->id);
            }

            LOG_TRACE("*** ID %d: Scanning object for child object deletion ***", p_obj->id);

            if (!gf_remove_obj_and_child(ID_NONE, &p_obj->child)) {
                LOG_TRACE("*** ID %d: No more child objects to delete ***", p_obj->id);
            }

            if (lv_obj_is_valid(p_obj->obj)) {
                LOG_TRACE("ID %d: deleting lvgl object", p_obj->id);
                lv_obj_delete(p_obj->obj);
            }

            list_del(&p_obj->node);
            LOG_TRACE("ID %d is deleted from list global data", p_obj->id);
            free(p_obj);

            if (req_id != ID_NONE) {
                LOG_TRACE("ID %d: Object and all child objects have been deleted", req_id);
                return true;
            } else {
                LOG_TRACE("Continuing the deletion of child objects");
                continue;
            }

        } else if (p_obj->id != req_id) {
            if (gf_remove_obj_and_child(req_id, &p_obj->child)) {
                LOG_TRACE("### ID %d: Object and all child objects have been deleted <---|", req_id);
                return true;
            }
            continue;
        } else {
            LOG_TRACE("Warning: This message indicates an unexpected condition");
        }
    }

    return deleted;
}

void gf_refresh_all_layer(void)
{
    lv_obj_invalidate(lv_layer_sys());
    lv_obj_invalidate(lv_layer_top());
    lv_obj_invalidate(lv_screen_active());
    lv_obj_invalidate(lv_layer_bottom());
}

