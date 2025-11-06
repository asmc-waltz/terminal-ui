/**
 * @file bluetooth.c
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
static void switch_bluetooth_enable_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *lobj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        LOG_TRACE("Switch state: %s\n", \
                  lv_obj_has_state(lobj, LV_STATE_CHECKED) ? "On" : "Off");
        if (lv_obj_has_state(lobj, LV_STATE_CHECKED)) {
            ;
        } else {
            ;
        }
    }
}

static int32_t create_setting_items(lv_obj_t *par)
{
    lv_obj_t *group, *child_group, *sym, *label, *swit;
    const char *desc = "Enable or disable Bluetooth,\n"
                       "scan for nearby devices,\n"
                       "and manage pairing settings";

    if (!par)
        return -EINVAL;

    /* Section: Bluetooth description */
    group = create_vertical_flex_group(par, "BLUETOOTH-ABOUT");
    if (!group)
        return -ENOMEM;

    sym = create_symbol_box(group, NULL, &lv_font_montserrat_48, \
                            LV_SYMBOL_BLUETOOTH);
    if (!sym)
        return -EIO;

    label = create_text_box(group, NULL, &lv_font_montserrat_24, desc);
    if (!label)
        return -EIO;

    /* Bluetooth mode toggle */
    group = create_horizontal_flex_group(group, "BLUETOOTH-SWITCH");
    if (!group)
        return -ENOMEM;

    set_border_side(group, LV_BORDER_SIDE_TOP);
    lv_obj_set_style_border_width(group, 2, 0);
    lv_obj_set_style_border_color(group, lv_color_black(), 0);

    label = create_text_box(group, NULL, \
                            &lv_font_montserrat_24, "Bluetooth");
    if (!label)
        return -EIO;

    swit = create_switch_box(group, NULL);
    if (!swit)
        return -EIO;
    lv_obj_add_event_cb(get_box_child(swit), \
                        switch_bluetooth_enable_handler, \
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
lv_obj_t *create_bluetooth_setting(lv_obj_t *par, const char *name)
{
    int32_t ret;
    lv_obj_t *container, *view;
    lv_obj_t *menu;
    menu_view_t *v_ctx;
    char name_buf[64];

    snprintf(name_buf, sizeof(name_buf), "%s_BLUETOOTH", name);
    v_ctx = create_menu_view(par, name_buf, true, false);
    if (!v_ctx)
        goto err_view;

    container = v_ctx->container;
    view = v_ctx->view;
    if (!container || !view)
        return NULL;

    menu = create_menu(view);
    if (!menu) {
        LOG_ERROR("[%s] create menu bar failed, ret %d", get_name(view), ret);
        goto err_view;
    }

    lv_obj_add_flag(menu, LV_OBJ_FLAG_SCROLLABLE);

    ret = create_setting_items(menu);
    if (ret) {
        LOG_ERROR("Setting menu bar [%s] create failed, ret %d", \
                  get_name(menu), ret);
        goto err_view;
    }


    return container;

err_view:
    remove_obj_and_child(get_meta(container)->id, &get_meta(par)->child);
    return NULL;
}
