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
    g_ctx_t *ctx = gf_get_app_ctx();

    LV_ASSERT_NULL(obj);
    LOG_TRACE("Register handler id %d", id);
    p_handler = malloc(sizeof(g_handler));
    LV_ASSERT_NULL(p_handler);
    p_handler->id = id;
    p_handler->obj = obj;

    pg_obj = obj->user_data;

    list_add_tail(&p_handler->node, &ctx->handlers);
    lv_obj_add_event_cb(obj, event_cb, filter, pg_obj);

    return p_handler;
}

