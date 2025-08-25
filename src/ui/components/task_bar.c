/**
 * @file task_bar.c
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

#include <stdint.h>

#include <lvgl.h>
#include <ui/gmisc.h>
#include <ui/fonts.h>
#include <ui/style.h>
#include <ui/screens.h>

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
static void app_handler(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);  // Get the button object
    lv_obj_t *par = lv_obj_get_parent(btn);
    g_obj *pg_obj = NULL;

    gf_refresh_all_layer();

    pg_obj = btn->user_data;
    LOG_DEBUG("ID %d: Taskbar button clicked", pg_obj->id);
    if (pg_obj->id != ID_TASK_BAR_SETTING)
    {
        return;
    }

    pg_obj = par->user_data;

    if (pg_obj->visible) {
        gf_hide_taskbar();
        gf_show_home_indicator();
        gf_show_setting();
    }
}

static lv_obj_t * sf_create_app_icon(lv_obj_t *par, uint32_t id, \
                                     uint32_t bg_color, const char *symbol)
{
    LV_ASSERT_NULL(par);
    lv_style_t *p_style = NULL;

    p_style = gf_get_lv_style(STY_BG_ICON_79);
    lv_obj_t *button = gf_create_btn_bg(par, id, p_style, bg_color);

    p_style = gf_get_lv_style(STY_SYM_48);
    lv_obj_t *setting_symbol = gf_create_symbol(button, p_style, symbol);

    return button;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t * gf_create_taskbar(lv_obj_t *parent)
{
    lv_obj_t *p_taskbar = NULL;
    lv_obj_t *btn;
    p_taskbar = gf_create_obj(parent, ID_TASK_BAR);
    LV_ASSERT_NULL(p_taskbar);

    lv_style_t *p_style = NULL;
    p_style = gf_get_lv_style(STY_TASKBAR);

    lv_obj_add_style(p_taskbar, p_style, 0);
    lv_obj_set_style_flex_flow(p_taskbar, LV_FLEX_FLOW_ROW, 0);
    lv_obj_set_style_layout(p_taskbar, LV_LAYOUT_FLEX, 0);
    lv_obj_set_style_bg_opa(p_taskbar, LV_OPA_30, LV_PART_MAIN);
    lv_obj_remove_flag(p_taskbar, LV_OBJ_FLAG_SCROLLABLE);
    // The size of the taskbar dynamically adjusts based on the number of icons.
    lv_obj_set_size(p_taskbar, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    btn = sf_create_app_icon(p_taskbar, ID_TASK_BAR_PHONE, 0x03BF1F, ICON_PHONE_SOLID);
    gf_register_handler(btn, ID_TASK_BAR_PHONE, app_handler, LV_EVENT_CLICKED);

    btn = sf_create_app_icon(p_taskbar, ID_TASK_BAR_MESSAGE, 0x03BF1F, ICON_COMMENT_SOLID);
    gf_register_handler(btn, ID_TASK_BAR_MESSAGE, app_handler, LV_EVENT_CLICKED);

    btn = sf_create_app_icon(p_taskbar, ID_TASK_BAR_TOOLBOX, 0xFFAE3B, ICON_TOOLBOX_SOLID);
    gf_register_handler(btn, ID_TASK_BAR_TOOLBOX, app_handler, LV_EVENT_CLICKED);

    btn = sf_create_app_icon(p_taskbar, ID_TASK_BAR_SETTING, 0x4F8DFF, ICON_GEAR_SOLID);
    gf_register_handler(btn, ID_TASK_BAR_SETTING, app_handler, LV_EVENT_CLICKED);

    // Align it to bottom-middle AFTER children are added
    lv_obj_align_to(p_taskbar, parent, LV_ALIGN_BOTTOM_MID, 0, -10);

    gf_hide_taskbar();

    return p_taskbar;
}

void gf_hide_taskbar(void)
{
    lv_obj_t *pl_obj = NULL;
    g_obj *pg_obj = NULL;

    pl_obj = gf_get_obj(ID_TASK_BAR, NULL);
    LV_ASSERT_NULL(pl_obj);
    pg_obj = pl_obj->user_data;

    lv_obj_add_flag(pl_obj, LV_OBJ_FLAG_HIDDEN);
    pg_obj->visible = false;
}

void gf_show_taskbar(void)
{
    lv_obj_t *pl_obj = NULL;
    g_obj *pg_obj = NULL;

    pl_obj = gf_get_obj(ID_TASK_BAR, NULL);
    LV_ASSERT_NULL(pl_obj);
    pg_obj = pl_obj->user_data;

    lv_obj_remove_flag(pl_obj, LV_OBJ_FLAG_HIDDEN);
    pg_obj->visible = true;
}

void gf_delete_taskbar(void)
{
    gf_remove_obj_and_child(ID_TASK_BAR, NULL);
}
