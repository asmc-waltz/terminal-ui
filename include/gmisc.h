/**
 * @file gmisc.h
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <lvgl.h>

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
 * GLOBAL PROTOTYPES
 **********************/
/*=====================
 * Setter functions
 *====================*/
void gf_hide_taskbar();
void gf_show_taskbar();
void gf_delete_taskbar();

/*=====================
 * Getter functions
 *====================*/
/*=====================
 * Other functions
 *====================*/
lv_obj_t * g_create_panel(lv_obj_t *parent, lv_style_t *sty, int32_t w, int32_t h);
lv_obj_t * g_create_background(lv_obj_t *parent, lv_style_t *style, int32_t w, int32_t h);

void gf_refresh_all_layer(void);

lv_obj_t * g_create_status_bar(lv_obj_t *parent);
lv_obj_t * gf_create_taskbar(lv_obj_t *parent);

/**********************
 *      MACROS
 **********************/


