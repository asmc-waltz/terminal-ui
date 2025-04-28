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
static void btn_setting_handler(lv_event_t *e)
{
    lv_obj_t * btn = lv_event_get_target(e);  // Get the button object
    LV_LOG_USER("Button was clicked!");

    lv_obj_invalidate(lv_screen_active());
    lv_obj_invalidate(lv_layer_bottom());
    lv_obj_invalidate(lv_layer_top());
    if (kb_visible) {
        hidden_keyboard();
    } else {
        show_keyboard();
    }
}

static void btn_keyboard_handler(lv_event_t *e)
{
    lv_obj_t * btn = lv_event_get_target(e);  // Get the button object
    LV_LOG_USER("Button was clicked!");

    lv_obj_invalidate(lv_screen_active());
    lv_obj_invalidate(lv_layer_bottom());
    lv_obj_invalidate(lv_layer_top());
    if (kb_visible) {
        hidden_keyboard();
    } else {
        show_keyboard();
    }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void home_screen(lv_style_t *style) {
    lv_obj_t * bg = g_create_background(lv_layer_bottom(), style, 1024, 600);

    g_create_status_bar(lv_screen_active());
    gf_create_taskbar(lv_screen_active());
    gf_show_taskbar();

    return bg;
}


