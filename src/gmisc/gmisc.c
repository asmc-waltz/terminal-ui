/**
 * @file gmisc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <gmisc.h>

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
g_handler * gf_register_handler(uint32_t id, lv_event_cb_t event_cb, lv_event_code_t filter)
{
    lv_obj_t *pl_obj = NULL;
    g_obj *pg_obj = NULL;
    g_handler *p_handler = NULL;

    pl_obj = gf_get_obj(id, NULL);
    LV_ASSERT_NULL(pl_obj);

    // LV_LOG_USER("Register handler id %d", id);
    p_handler = malloc(sizeof(g_handler));
    LV_ASSERT_NULL(p_handler);
    p_handler->id = id;
    p_handler->obj = pl_obj;

    pg_obj = pl_obj->user_data;

    list_add_tail(&p_handler->node, &global_data->handler_list);
    lv_obj_add_event_cb(pl_obj, event_cb, filter, pg_obj);

    return p_handler;
}

g_obj * gf_register_obj(lv_obj_t *par, lv_obj_t *obj, uint32_t id)
{
    g_obj *p_obj = NULL;
    struct list_head *par_list;

    if (par == NULL) {
        // LV_LOG_USER("Root object is detect -> add this one into the global data");
        par_list = &global_data->obj_list;
    } else {
        // LV_LOG_USER("Normal object is detect -> add this one into parrent data as child");
        par_list = &((g_obj *)par->user_data)->child;
    }

    // LV_LOG_USER("Register obj id %d", id);
    p_obj = malloc(sizeof(g_obj));
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
    g_obj *p_obj = NULL;
    lv_obj_t *obj = NULL;

    LV_ASSERT_NULL(par);
    // LV_LOG_USER("Create obj id %d", id);
    obj = lv_obj_create(par);
    LV_ASSERT_NULL(obj);

    p_obj = gf_register_obj(par, obj, id);
    LV_ASSERT_NULL(p_obj);

    return p_obj->obj;
}

lv_obj_t * gf_get_obj(uint32_t req_id, struct list_head *head_lst)
{
    g_obj *p_obj = NULL;
    lv_obj_t *lv_obj = NULL;
    struct list_head *par_list;

    if (head_lst == NULL) {
        // LV_LOG_USER("Scan from Root object");
        par_list = &global_data->obj_list;
    } else {
        // LV_LOG_USER("Scan from parent");
        par_list = head_lst;
    }

    list_for_each_entry(p_obj, par_list, node)
    if (p_obj->id) {
        if (p_obj->id == req_id) {
            // LV_LOG_USER("REQ obj id %d is detected", p_obj->id);
            break;
        } else if (p_obj->id != req_id) {
            // LV_LOG_USER("Scan inside obj id %d", p_obj->id);
            lv_obj = gf_get_obj(req_id, &p_obj->child);
            if (lv_obj != NULL) {
                // LV_LOG_USER("obj id %d is the expeted one", ((g_obj *)(lv_obj->user_data))->id);
                return lv_obj;
            } else {
                // LV_LOG_USER("obj is NOT the expeted one");
                continue;
            }
        }
    }

    return p_obj->id == req_id ? p_obj->obj : NULL;
}

void gf_remove_obj(uint32_t req_id)
{
    g_obj *p_obj = NULL;
    g_obj *p_obj_tmp = NULL;

    list_for_each_entry_safe(p_obj, p_obj_tmp, &global_data->obj_list, node)
    if (p_obj->id) {
        if (p_obj->id != req_id) {
            continue;
        }

        // LV_LOG_USER("REQ obj id %d is detected", p_obj->id);
        if (lv_obj_is_valid(p_obj->obj)) {
            lv_obj_delete(p_obj->obj);
        }

        list_del(&p_obj->node);
        free(p_obj);
        break;
    }
}

void gf_refresh_all_layer(void)
{
    lv_obj_invalidate(lv_layer_sys());
    lv_obj_invalidate(lv_layer_top());
    lv_obj_invalidate(lv_screen_active());
    lv_obj_invalidate(lv_layer_bottom());
}


