/**
 * @file home.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <screens.h>
#include <style.h>
#include <fonts.h>

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
void gf_create_home_screen(void) {
    lv_obj_t * bg = gf_create_background(lv_layer_bottom(), 1024, 600);

    g_create_status_bar(lv_screen_active());
    gf_create_taskbar(lv_screen_active());
    gf_show_taskbar();

    gf_create_home_indicator(lv_screen_active());
    return bg;
}


