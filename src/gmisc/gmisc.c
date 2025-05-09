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
// #define G_DEBUG LV_LOG_USER
#define G_DEBUG //

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
    g_obj *pg_obj = NULL;
    g_handler *p_handler = NULL;

    LV_ASSERT_NULL(obj);
    G_DEBUG("Register handler id %d", id);
    p_handler = malloc(sizeof(g_handler));
    LV_ASSERT_NULL(p_handler);
    p_handler->id = id;
    p_handler->obj = obj;

    pg_obj = obj->user_data;

    list_add_tail(&p_handler->node, &global_data->handler_list);
    lv_obj_add_event_cb(obj, event_cb, filter, pg_obj);

    return p_handler;
}

g_obj * gf_register_obj(lv_obj_t *par, lv_obj_t *obj, uint32_t id)
{
    g_obj *p_obj = NULL;
    struct list_head *par_list;

    if (par == NULL) {
        G_DEBUG("Root object is detect -> add this one into the global data");
        par_list = &global_data->obj_list;
    } else {
        G_DEBUG("Normal object is detect -> add this one into parrent data as child");
        par_list = &((g_obj *)par->user_data)->child;
    }

    G_DEBUG("Register OBJ id %d", id);
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
    G_DEBUG("Create obj id %d", id);
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
        G_DEBUG("Scan from Root object");
        par_list = &global_data->obj_list;
    } else {
        G_DEBUG("Scan from parent");
        par_list = head_lst;
    }

    list_for_each_entry(p_obj, par_list, node)
    if (p_obj->id) {
        if (p_obj->id == req_id) {
            G_DEBUG("OBJ id %d is detected", p_obj->id);
            break;
        } else if (p_obj->id != req_id) {
            G_DEBUG("Scan inside obj id %d", p_obj->id);
            lv_obj = gf_get_obj(req_id, &p_obj->child);
            if (lv_obj != NULL) {
                G_DEBUG("OBJ id %d is the expeted one", ((g_obj *)(lv_obj->user_data))->id);
                return lv_obj;
            } else {
                G_DEBUG("OBJ is NOT the expeted one");
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

        if (lv_obj_is_valid(p_obj->obj)) {
            G_DEBUG("REQ OBJ id %d is deleting", p_obj->id);
            lv_obj_delete(p_obj->obj);
        }

        list_del(&p_obj->node);
        G_DEBUG("REQ OBJ id %d is deleted", p_obj->id);
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

void gf_create_common_components(void)
{
    gf_create_background(lv_layer_bottom(), 1024, 600);
    gf_create_setting_menu(lv_screen_active());
    gf_create_status_bar(lv_layer_top());
    gf_create_taskbar(lv_layer_top());
    gf_create_home_indicator(lv_layer_top());
    // Initialize the default keyboard that will always be accessible on the top layer.
    gf_keyboard_create();
}
