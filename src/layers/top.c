/**
 * @file top.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <lvgl.h>
#include <layers.h>

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
void gf_config_top_layer() {
    lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_TRANSP, LV_PART_MAIN);
}

