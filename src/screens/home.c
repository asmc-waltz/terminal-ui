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
void btn_setting(lv_obj_t *parent, lv_style_t *style) {
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_set_size(btn, 79, 79);

    lv_obj_set_pos(btn, 700, 30);

    lv_obj_t * label = lv_label_create(btn);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x4F8DFF), 0);
    lv_obj_add_style(label, &icons_size_48, 0);
    lv_obj_center(label);
    lv_obj_set_size(label, 48, 48);
    lv_label_set_text(label, ICON_GEAR_SOLID);
    lv_obj_add_event_cb(btn, btn_setting_handler,LV_EVENT_CLICKED, NULL);
}


void btn_keyboard(lv_obj_t *parent, lv_style_t *style) {
    lv_obj_t * btn = lv_btn_create(parent);

    lv_obj_set_size(btn, 79, 79);
    lv_obj_set_pos(btn, 500, 30);
    lv_obj_t * label = lv_label_create(btn);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x4F8DFF), 0);
    lv_obj_add_style(label, &icons_size_48, 0);
    lv_obj_center(label);
    lv_obj_set_size(label, 48, 48);
    lv_label_set_text(label, ICON_KEYBOARD);

    lv_obj_add_event_cb(btn, btn_keyboard_handler,LV_EVENT_CLICKED, NULL);
}

void home_screen(lv_style_t *style) {
    lv_obj_t * bg = g_create_background(lv_layer_bottom(), style, 1024, 600);

    // btn_setting(lv_screen_active(), style);
    // btn_keyboard(lv_screen_active(), style);

    g_create_status_bar(lv_screen_active());
    g_create_task_bar(lv_screen_active());

    return bg;
}


