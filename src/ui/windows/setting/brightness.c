/**
 * @file brightness.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL defined locally will override the global setting in this file"
#endif
#include "log.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <lvgl.h>
#include "list.h"
#include "ui/ui_core.h"
#include "ui/fonts.h"
#include "ui/comps.h"
#include "ui/windows.h"
#include "ui/widget.h"
#include "sched/workqueue.h"
#include "comm/cmd_payload.h"

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
static lv_obj_t *brightness_slider = NULL;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void set_brightness_runtime_slider(int32_t value)
{
    lv_slider_set_value(brightness_slider, value, LV_ANIM_OFF);
}

static int32_t req_current_brightness()
{
    remote_cmd_t *cmd;
    int32_t ret = 0;

    cmd = create_remote_cmd();
    if (!cmd)
        return -ENOMEM;

    remote_cmd_init(cmd, COMP_NAME, COMP_ID, OP_GET_BRIGHTNESS, \
                    WORK_PRIO_NORMAL, WORK_DURATION_SHORT);

    /* Command data will be auto-released after work completes */
    ret = create_remote_task(WORK_PRIO_HIGH, cmd);
    return ret;
}

static void switch_auto_brightness_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *lobj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *manual_brightness = get_obj_by_name("BRIGHTNESS-MANUAL", \
                                       &get_meta(lv_screen_active())->child);
        LV_LOG_USER("State: %s\n", lv_obj_has_state(lobj, LV_STATE_CHECKED) ? "On" : "Off");
        if (lv_obj_has_state(lobj, LV_STATE_CHECKED)) {
            lv_obj_add_flag(manual_brightness, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_clear_flag(manual_brightness, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

static int32_t create_brightness_setting_items(lv_obj_t *par)
{
    lv_obj_t *group, *sym, *label, *swit;
    const char *desc = "Manual and auto brightness setting";

    if (!par)
        return -EINVAL;

    /* Section: Brightness description */
    group = create_vertical_flex_group(par, "BRIGHTNESS-ABOUT");
    if (!group)
        return -ENOMEM;

    sym = create_symbol_box(group, NULL, &terminal_icons_48, \
                            ICON_CIRCLE_HALF_STROKE_SOLID);
    if (!sym)
        return -EIO;

    label = create_text_box(group, NULL, &lv_font_montserrat_24, desc);
    if (!label)
        return -EIO;

    /* Section: Auto brightness toggle */
    group = create_horizontal_flex_group(par, "BRIGHTNESS-AUTO");
    if (!group)
        return -ENOMEM;

    label = create_text_box(group, NULL, &lv_font_montserrat_24, "Auto");
    if (!label)
        return -EIO;

    swit = create_switch_box(group, NULL);
    if (!swit)
        return -EIO;
    lv_obj_add_event_cb(get_box_child(swit), \
                        switch_auto_brightness_event_handler, \
                        LV_EVENT_ALL, NULL);

    /* Section: Auto brightness toggle */
    group = create_horizontal_flex_group(par, "BRIGHTNESS-MANUAL");
    if (!group)
        return -ENOMEM;

    label = create_text_box(group, NULL, &lv_font_montserrat_24, "Manual");
    if (!label)
        return -EIO;

    brightness_slider = create_slider(group, NULL);
    if (!brightness_slider)
        return -EIO;

    set_size(brightness_slider, LV_PCT(70), 20);
    req_current_brightness();


    /* Section: Spacer (flex filler) */
    lv_obj_t *filler = create_box(par, "BRIGHTNESS-FILLER");
    if (!filler)
        return -EIO;

    set_size(filler, LV_PCT(100), LV_PCT(100));

    LOG_DEBUG("Brightness setting items created");
    return 0;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t *create_brightness_setting(lv_obj_t *menu, lv_obj_t *par, const char *name)
{
    lv_obj_t *page;

    page = create_menu_page(menu, par, name);
    if (!page)
        return NULL;

    create_brightness_setting_items(page);

    return page;
}

int32_t res_current_brightness(remote_cmd_t *cmd)
{
    int32_t ret = 0;

    LOG_TRACE("Brightness current value response key: [%s] - value: [%d]", \
              cmd->entries[0].key, cmd->entries[0].value.i32);

    /* Update UI asynchronously to avoid blocking caller thread */
    lv_async_call(set_brightness_runtime_slider, cmd->entries[0].value.i32);

    return ret;
}
