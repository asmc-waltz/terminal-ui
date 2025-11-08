/**
 * @file hotspot.c
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
#include "ui/widget/menu.h"

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
static void switch_hotspot_enable_handler(lv_event_t *e)
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
    lv_obj_t *group, *child_group, *sym, *label, *switch_box;
    int32_t ret;

    const char *desc = "Enable or disable Wi-Fi hotspot\n"
                       "and manage sharing settings";

    if (!par)
        return -EINVAL;

    /* Section: Hostspot description */
    group = create_vertical_flex_group(par, "HOTSPOT-ABOUT");
    if (!group)
        return -ENOMEM;

    sym = create_symbol_box(group, NULL, &terminal_icons_48, \
                            ICON_SHARE_NODES_SOLID);
    if (!sym)
        return -EIO;

    label = create_text_box(group, NULL, &lv_font_montserrat_24, desc);
    if (!label)
        return -EIO;

    /* Hostspot mode toggle */
    group = create_horizontal_flex_group(group, "HOTSPOT-SWITCH");
    if (!group)
        return -ENOMEM;

    set_border_side(group, LV_BORDER_SIDE_TOP);
    lv_obj_set_style_border_width(group, 2, 0);
    lv_obj_set_style_border_color(group, lv_color_black(), 0);

    label = create_text_box(group, NULL, \
                            &lv_font_montserrat_24, "Hotspot");
    if (!label)
        return -EIO;

    switch_box = create_switch_box(group, NULL);
    if (!switch_box)
        return -EIO;
    lv_obj_add_event_cb(get_box_child(switch_box), \
                        switch_hotspot_enable_handler, \
                        LV_EVENT_ALL, NULL);

    /* Section: Spacer (flex filler) */
    ret = create_setting_filler(par);
    if (ret < 0)
        return ret;

    LOG_DEBUG("Hotspot setting items created");
    return 0;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t *create_hotspot_setting(lv_obj_t *par, const char *name, \
                                 view_ctn_t *par_v_ctx)
{
    lv_obj_t *container, *view, *menu;
    view_ctn_t *v_ctx;
    char name_buf[64];
    int32_t ret;

    snprintf(name_buf, sizeof(name_buf), "%s.#HOTSPOT", name);

    v_ctx = create_common_menu_view(par, name_buf, par_v_ctx, false);
    if (!v_ctx) {
        LOG_ERROR("[%s] create menu view failed, ret %d", name, ret);
        goto err_ctx;
    }

    container = get_view_container(v_ctx);
    view = get_view(v_ctx);
    menu = get_menu(v_ctx);

    ret = create_setting_items(menu);
    if (ret) {
        LOG_ERROR("Setting menu bar [%s] create failed, ret %d", \
                  get_name(menu), ret);
        goto err_view;
    }

    if (container)
        return container;
    return view;

err_view:
    if (container)
        remove_obj_and_child(get_meta(container)->id, \
                             &get_meta(par)->child);
    else
        remove_obj_and_child(get_meta(view)->id, &get_meta(par)->child);
    free(v_ctx);
err_ctx:
    return NULL;
}
