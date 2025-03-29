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
lv_obj_t * g_create_panel(lv_obj_t *parent, lv_style_t *style, int32_t w, int32_t h) {
    lv_obj_t *bg = lv_obj_create(parent);
    lv_obj_set_size(bg, w, h);
    lv_obj_add_style(bg, style, 0);
    return bg;
}

lv_obj_t * g_create_background(lv_obj_t *parent, lv_style_t *style, int32_t w, int32_t h) {
    return g_create_panel(parent, style, w, h);
}

