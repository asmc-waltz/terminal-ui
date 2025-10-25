/**
 * @file wifi.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
// #define LOG_LEVEL LOG_LEVEL_TRACE
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

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void switch_wifi_enable_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *lobj = lv_event_get_target(e);
    bool enable;
    int32_t ret;

    if (code != LV_EVENT_VALUE_CHANGED)
        return;

    enable = lv_obj_has_state(lobj, LV_STATE_CHECKED);
    LOG_TRACE("Wi-Fi: %s", enable ? "On" : "Off");

    ret = create_remote_simple_task(WORK_PRIO_NORMAL, WORK_DURATION_SHORT, \
                                    enable ? OP_WIFI_ENABLE : OP_WIFI_DISABLE);
    if (ret)
        LOG_ERROR("%s Wi-Fi failed, ret %d", \
                  enable ? "Enable" : "Disable", ret);
}

static int32_t create_wifi_setting_items(lv_obj_t *par)
{
    lv_obj_t *group, *child_group, *sym, *label, *swit;
    const char *desc = "Connect to available wireless networks\n"
                       "or manage saved connections";

    if (!par)
        return -EINVAL;

    /* Section: Brightness description */
    group = create_vertical_flex_group(par, "WIFI-ABOUT");
    if (!group)
        return -ENOMEM;

    sym = create_symbol_box(group, NULL, &terminal_icons_48, \
                            ICON_WIFI_SOLID);
    if (!sym)
        return -EIO;

    label = create_text_box(group, NULL, &lv_font_montserrat_24, desc);
    if (!label)
        return -EIO;

    /* Airplane mode toggle */
    group = create_horizontal_flex_group(group, "WIFI-SWITCH");
    if (!group)
        return -ENOMEM;

    set_border_side(group, LV_BORDER_SIDE_TOP);
    lv_obj_set_style_border_width(group, 2, 0);
    lv_obj_set_style_border_color(group, lv_color_black(), 0);

    label = create_text_box(group, NULL, \
                            &lv_font_montserrat_24, "Wi-Fi");
    if (!label)
        return -EIO;

    swit = create_switch_box(group, NULL);
    if (!swit)
        return -EIO;
    lv_obj_add_event_cb(get_box_child(swit), \
                        switch_wifi_enable_handler, \
                        LV_EVENT_ALL, NULL);

    /* Section: Spacer (flex filler) */
    lv_obj_t *filler = create_box(par, "FILLER");
    if (!filler)
        return -EIO;

    set_size(filler, LV_PCT(100), LV_PCT(100));

    LOG_DEBUG("Wifi setting items created");
    return 0;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t *create_wifi_setting(lv_obj_t *menu, lv_obj_t *par, const char *name)
{
    int32_t ret;
    lv_obj_t *page;

    page = create_menu_page(menu, par, name);
    if (!page)
        return NULL;

    ret = create_wifi_setting_items(page);
    if (ret) {
        LOG_ERROR("Setting page [%s] create failed, ret %d", \
                  get_name(page), ret);
        remove_obj_and_child(get_meta(page)->id, &get_meta(par)->child);
        return NULL;
    }

    return page;
}
