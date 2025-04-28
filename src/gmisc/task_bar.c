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
static lv_obj_t *sp_taskbar;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void taskbar_app_icon_handler(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);  // Get the button object
    lv_obj_t *par = lv_obj_get_parent(btn);
    id_data *par_data = par->user_data;
    LV_LOG_USER("Button was clicked!");
    gf_refresh_all_layer();

    char *name = gf_get_name(btn);
    LV_LOG_USER("Button %s was DETECTED!", name);

    // if (strcmp(name, AIC_TOOLBOX) == 0) {
    // }

    // if (par_data->visible)
    //     gf_hide_taskbar();
}

static lv_obj_t * taskbar_app_icon_create(lv_obj_t *par, uint32_t bg_color, \
                            uint32_t symbol, lv_event_cb_t event_cb, char *name)
{
    LV_ASSERT_NULL(par);
    lv_obj_t *button = create_icon_bg(par, &bg_79, bg_color);
    lv_obj_t *setting_symbol = create_symbol(button, &sym_48, symbol);

    id_data *id_app_data = gf_init_user_data(button);
    LV_ASSERT_NULL(id_app_data);
    gf_set_name(button, name);

    lv_obj_add_event_cb(button, event_cb, LV_EVENT_CLICKED, NULL);
    return button;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t * gf_create_taskbar(lv_obj_t *parent)
{
    sp_taskbar = lv_obj_create(parent);
    id_data *id_taskbar_data = gf_init_user_data(sp_taskbar);
    LV_ASSERT_NULL(id_taskbar_data);

    lv_obj_add_style(sp_taskbar, &task_bar_style, 0);
    lv_obj_set_style_flex_flow(sp_taskbar, LV_FLEX_FLOW_ROW, 0);
    lv_obj_set_style_layout(sp_taskbar, LV_LAYOUT_FLEX, 0);
    lv_obj_set_style_bg_opa(sp_taskbar, LV_OPA_30, LV_PART_MAIN);
    lv_obj_remove_flag(sp_taskbar, LV_OBJ_FLAG_SCROLLABLE);
    // The size of the taskbar dynamically adjusts based on the number of icons.
    lv_obj_set_size(sp_taskbar, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    taskbar_app_icon_create(sp_taskbar, 0x03BF1F, ICON_PHONE_SOLID, \
                            taskbar_app_icon_handler, AIC_PHONE);
    taskbar_app_icon_create(sp_taskbar, 0x03BF1F, ICON_COMMENT_SOLID, \
                            taskbar_app_icon_handler, AIC_MESSAGE);
    taskbar_app_icon_create(sp_taskbar, 0xFFAE3B, ICON_TOOLBOX_SOLID, \
                            taskbar_app_icon_handler, AIC_TOOLBOX);
    taskbar_app_icon_create(sp_taskbar, 0x4F8DFF, ICON_GEAR_SOLID, \
                            taskbar_app_icon_handler, AIC_SETTING);

    // Align it to bottom-middle AFTER children are added
    lv_obj_align_to(sp_taskbar, parent, LV_ALIGN_BOTTOM_MID, 0, 0);

    gf_hide_taskbar();

    return sp_taskbar;
}

void gf_hide_taskbar(void)
{
    id_data *taskbar_data = sp_taskbar->user_data;

    lv_obj_add_flag(sp_taskbar, LV_OBJ_FLAG_HIDDEN);
    taskbar_data->visible = false;
}

void gf_show_taskbar(void)
{
    id_data *taskbar_data = sp_taskbar->user_data;

    lv_obj_remove_flag(sp_taskbar, LV_OBJ_FLAG_HIDDEN);
    taskbar_data->visible = true;
}

void gf_delete_taskbar(void)
{
    if(lv_obj_is_valid(sp_taskbar)) {
        gf_free_user_data(sp_taskbar);
        lv_obj_delete(sp_taskbar);
    }
}
