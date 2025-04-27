/**
 * @file task_bar.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <gmisc.h>
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
static lv_obj_t * create_phone_icon(lv_obj_t *parent, lv_style_t *icon_style) {
    lv_obj_t *btn_phone = lv_btn_create(parent);
    lv_obj_t *icon_phone = lv_label_create(btn_phone);

    lv_obj_add_style(btn_phone, icon_style, 0);
    lv_obj_set_size(btn_phone, 79, 79);

    lv_obj_set_style_bg_color(btn_phone, lv_color_hex(0x03BF1F), 0);
    lv_obj_center(icon_phone);
    lv_obj_set_size(icon_phone , 48, 48);
    lv_label_set_text(icon_phone, ICON_PHONE_SOLID);

    // lv_obj_add_event_cb(btn_phone, btn_phone_handler, LV_EVENT_CLICKED, NULL);
}

static lv_obj_t * create_message_icon(lv_obj_t *parent, lv_style_t *icon_style) {
    lv_obj_t *btn_message = lv_btn_create(parent);
    lv_obj_t *icon_message = lv_label_create(btn_message);

    lv_obj_add_style(btn_message, icon_style, 0);
    lv_obj_set_size(btn_message, 79, 79);
    lv_obj_set_style_bg_color(btn_message, lv_color_hex(0x03BF1F), 0);

    lv_obj_center(icon_message);
    lv_obj_set_size(icon_message, 48, 48);
    lv_label_set_text(icon_message, ICON_COMMENT_SOLID);

    // lv_obj_add_event_cb(btn_phone, btn_message_handler, LV_EVENT_CLICKED, NULL);
}

static lv_obj_t * create_toolbox_icon(lv_obj_t *parent, lv_style_t *icon_style) {
    lv_obj_t *btn_toolbox = lv_btn_create(parent);
    lv_obj_t *icon_toolbox = lv_label_create(btn_toolbox);

    lv_obj_add_style(btn_toolbox, icon_style, 0);
    lv_obj_set_size(btn_toolbox, 79, 79);
    lv_obj_set_style_bg_color(btn_toolbox, lv_color_hex(0xFFAE3B), 0);

    lv_obj_center(icon_toolbox);
    lv_obj_set_size(icon_toolbox, 48, 48);
    lv_label_set_text(icon_toolbox, ICON_TOOLBOX_SOLID);

    // lv_obj_add_event_cb(btn_phone, btn_toolbox_handler, LV_EVENT_CLICKED, NULL);
}

static lv_obj_t * create_setting_icon(lv_obj_t *parent, lv_style_t *icon_style) {
    lv_obj_t *btn_setting = lv_btn_create(parent);
    lv_obj_t *icon_setting = lv_label_create(btn_setting);

    lv_obj_add_style(btn_setting, icon_style, 0);
    lv_obj_set_size(btn_setting, 79, 79);
    lv_obj_set_style_bg_color(btn_setting, lv_color_hex(0x4F8DFF), 0);

    lv_obj_center(icon_setting);
    lv_obj_set_size(icon_setting, 48, 48);
    lv_label_set_text(icon_setting, ICON_GEAR_SOLID);

    // lv_obj_add_event_cb(btn_setting, btn_setting_handler, LV_EVENT_CLICKED, NULL);
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t * g_create_task_bar(lv_obj_t *parent) {
    lv_obj_t *task_bar= lv_obj_create(parent);

    lv_obj_add_style(task_bar, &task_bar_style, 0);
    lv_obj_set_style_flex_flow(task_bar, LV_FLEX_FLOW_ROW, 0);
    lv_obj_set_style_layout(task_bar, LV_LAYOUT_FLEX, 0);
    lv_obj_set_style_bg_opa(task_bar, LV_OPA_30, LV_PART_MAIN);
    lv_obj_remove_flag(task_bar, LV_OBJ_FLAG_SCROLLABLE);
    // The size of the taskbar dynamically adjusts based on the number of icons.
    lv_obj_set_size(task_bar, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    create_phone_icon(task_bar, &icons_size_48);
    create_message_icon(task_bar, &icons_size_48);
    create_toolbox_icon(task_bar, &icons_size_48);
    create_setting_icon(task_bar, &icons_size_48);

    // Align it to bottom-middle AFTER children are added
    lv_obj_align_to(task_bar, parent, LV_ALIGN_BOTTOM_MID, 0, 0);

    return task_bar;
}
