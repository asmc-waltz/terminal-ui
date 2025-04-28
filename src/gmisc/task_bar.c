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
bool sv_taskbar_vis = false;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *sp_taskbar;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void btn_phone_handler(lv_event_t *event)
{
    lv_obj_t *btn_phone = lv_event_get_target(event);  // Get the button object
    LV_LOG_USER("Button phone was clicked!");
    gf_refresh_all_layer();

    if (sv_taskbar_vis)
        gf_hide_taskbar();
}

static void btn_message_handler(lv_event_t *event)
{
    lv_obj_t *btn_message = lv_event_get_target(event);  // Get the button object
    LV_LOG_USER("Button message was clicked!");
    gf_refresh_all_layer();

    if (sv_taskbar_vis)
        gf_hide_taskbar();
}

static void btn_toolbox_handler(lv_event_t *event)
{
    lv_obj_t *btn_toolbox = lv_event_get_target(event);  // Get the button object
    LV_LOG_USER("Button tool box was clicked!");
    gf_refresh_all_layer();

    if (sv_taskbar_vis)
        gf_hide_taskbar();
}

static void btn_setting_handler(lv_event_t *event)
{
    lv_obj_t *btn_setting = lv_event_get_target(event);  // Get the button object
    LV_LOG_USER("Button setting was clicked!");
    gf_refresh_all_layer();

    if (sv_taskbar_vis)
        gf_hide_taskbar();
}

static lv_obj_t * create_phone_icon(lv_obj_t *parent, lv_style_t *icon_style)
{
    lv_obj_t *btn_phone = lv_btn_create(parent);
    lv_obj_t *icon_phone = lv_label_create(btn_phone);

    lv_obj_add_style(btn_phone, icon_style, 0);
    lv_obj_set_size(btn_phone, 79, 79);

    lv_obj_set_style_bg_color(btn_phone, lv_color_hex(0x03BF1F), 0);
    lv_obj_center(icon_phone);
    lv_obj_set_size(icon_phone , 48, 48);
    lv_label_set_text(icon_phone, ICON_PHONE_SOLID);

    lv_obj_add_event_cb(btn_phone, btn_phone_handler, LV_EVENT_CLICKED, NULL);
}

static lv_obj_t * create_message_icon(lv_obj_t *parent, lv_style_t *icon_style)
{
    lv_obj_t *btn_message = lv_btn_create(parent);
    lv_obj_t *icon_message = lv_label_create(btn_message);

    lv_obj_add_style(btn_message, icon_style, 0);
    lv_obj_set_size(btn_message, 79, 79);
    lv_obj_set_style_bg_color(btn_message, lv_color_hex(0x03BF1F), 0);

    lv_obj_center(icon_message);
    lv_obj_set_size(icon_message, 48, 48);
    lv_label_set_text(icon_message, ICON_COMMENT_SOLID);

    lv_obj_add_event_cb(btn_message, btn_message_handler, LV_EVENT_CLICKED, NULL);
}

static lv_obj_t * create_toolbox_icon(lv_obj_t *parent, lv_style_t *icon_style)
{
    lv_obj_t *btn_toolbox = lv_btn_create(parent);
    lv_obj_t *icon_toolbox = lv_label_create(btn_toolbox);

    lv_obj_add_style(btn_toolbox, icon_style, 0);
    lv_obj_set_size(btn_toolbox, 79, 79);
    lv_obj_set_style_bg_color(btn_toolbox, lv_color_hex(0xFFAE3B), 0);

    lv_obj_center(icon_toolbox);
    lv_obj_set_size(icon_toolbox, 48, 48);
    lv_label_set_text(icon_toolbox, ICON_TOOLBOX_SOLID);

    lv_obj_add_event_cb(btn_toolbox, btn_toolbox_handler, LV_EVENT_CLICKED, NULL);
}

static lv_obj_t * create_setting_icon(lv_obj_t *parent, lv_style_t *icon_style)
{
    lv_obj_t *btn_setting = lv_btn_create(parent);
    lv_obj_t *icon_setting = lv_label_create(btn_setting);

    lv_obj_add_style(btn_setting, icon_style, 0);
    lv_obj_set_size(btn_setting, 79, 79);
    lv_obj_set_style_bg_color(btn_setting, lv_color_hex(0x4F8DFF), 0);

    lv_obj_center(icon_setting);
    lv_obj_set_size(icon_setting, 48, 48);
    lv_label_set_text(icon_setting, ICON_GEAR_SOLID);

    lv_obj_add_event_cb(btn_setting, btn_setting_handler, LV_EVENT_CLICKED, NULL);
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t * gf_create_taskbar(lv_obj_t *parent)
{
    sp_taskbar = lv_obj_create(parent);

    lv_obj_add_style(sp_taskbar, &task_bar_style, 0);
    lv_obj_set_style_flex_flow(sp_taskbar, LV_FLEX_FLOW_ROW, 0);
    lv_obj_set_style_layout(sp_taskbar, LV_LAYOUT_FLEX, 0);
    lv_obj_set_style_bg_opa(sp_taskbar, LV_OPA_30, LV_PART_MAIN);
    lv_obj_remove_flag(sp_taskbar, LV_OBJ_FLAG_SCROLLABLE);
    // The size of the taskbar dynamically adjusts based on the number of icons.
    lv_obj_set_size(sp_taskbar, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    create_phone_icon(sp_taskbar, &icons_size_48);
    create_message_icon(sp_taskbar, &icons_size_48);
    create_toolbox_icon(sp_taskbar, &icons_size_48);
    create_setting_icon(sp_taskbar, &icons_size_48);

    // Align it to bottom-middle AFTER children are added
    lv_obj_align_to(sp_taskbar, parent, LV_ALIGN_BOTTOM_MID, 0, 0);

    gf_hide_taskbar();

    return sp_taskbar;
}

void gf_hide_taskbar(void)
{
    lv_obj_add_flag(sp_taskbar, LV_OBJ_FLAG_HIDDEN);
    sv_taskbar_vis = false;
}

void gf_show_taskbar(void)
{
    lv_obj_remove_flag(sp_taskbar, LV_OBJ_FLAG_HIDDEN);
    sv_taskbar_vis = true;
}

void gf_delete_taskbar(void)
{
    if(lv_obj_is_valid(sp_taskbar))
        lv_obj_delete(sp_taskbar);
}
