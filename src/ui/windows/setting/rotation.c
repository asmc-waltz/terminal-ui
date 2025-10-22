/**
 * @file rotation.c
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
#include "main.h"

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
static lv_obj_t *rotation_switch = NULL;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void switch_rotation_enable_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *lobj = lv_event_get_target(e);
    bool enable;
    int32_t ret;

    if (code != LV_EVENT_VALUE_CHANGED)
        return;

    enable = lv_obj_has_state(lobj, LV_STATE_CHECKED);
    LOG_TRACE("Auto rotation: %s", enable ? "On" : "Off");

    ret = create_remote_simple_task(WORK_PRIO_NORMAL, WORK_DURATION_SHORT, \
                                    enable ? OP_ENABLE_IMU : OP_DISABLE_IMU);
    if (ret)
        LOG_ERROR("%s IMU sensor failed, ret %d", \
                  enable ? "Enable" : "Disable", ret);
}

static int32_t create_rotation_setting_items(lv_obj_t *par)
{
    lv_obj_t *group, *child_group, *sym, *label, *switch_box;
    const char *desc = "Control screen orientation or enable\n"
                       "auto rotation based on device position";

    if (!par)
        return -EINVAL;

    /* Section: rotation description */
    group = create_vertical_flex_group(par, "ROTATION-ABOUT");
    if (!group)
        return -ENOMEM;

    sym = create_symbol_box(group, NULL, &terminal_icons_48, \
                            ICON_ROTATE_SOLID);

    if (!sym)
        return -EIO;

    label = create_text_box(group, NULL, &lv_font_montserrat_24, desc);
    if (!label)
        return -EIO;

    /* Rotation mode toggle */
    group = create_horizontal_flex_group(group, "ROTATION-SWITCH");
    if (!group)
        return -ENOMEM;

    set_border_side(group, LV_BORDER_SIDE_TOP);
    lv_obj_set_style_border_width(group, 2, 0);
    lv_obj_set_style_border_color(group, lv_color_black(), 0);

    label = create_text_box(group, NULL, \
                            &lv_font_montserrat_24, "Rotation");
    if (!label)
        return -EIO;

    switch_box = create_switch_box(group, NULL);
    if (!switch_box)
        return -EIO;
    lv_obj_add_event_cb(get_box_child(switch_box), \
                        switch_rotation_enable_handler, \
                        LV_EVENT_ALL, NULL);
    rotation_switch = get_box_child(switch_box);

    /* Section: Spacer (flex filler) */
    lv_obj_t *filler = create_box(par, "FILLER");
    if (!filler)
        return -EIO;

    set_size(filler, LV_PCT(100), LV_PCT(100));

    LOG_DEBUG("Rotation setting items created");
    return 0;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t *create_rotation_setting(lv_obj_t *menu, lv_obj_t *par, const char *name)
{
    int32_t ret;
    lv_obj_t *page;

    page = create_menu_page(menu, par, name);
    if (!page)
        return NULL;

    ret = create_rotation_setting_items(page);
    if (ret) {
        LOG_ERROR("Setting page [%s] create failed, ret %d", \
                  get_name(page), ret);
        remove_obj_and_child(get_meta(page)->id, &get_meta(par)->child);
        return NULL;
    }

    return page;
}

int32_t handle_imu_rotation_state(remote_cmd_t *cmd)
{
    ctx_t *ctx;
    lv_obj_t *screen;
    int32_t state, roll, pitch, yaw;
    int8_t rotation = -1;
    static int8_t prev_rot = ROTATION_0;

    if (!cmd)
        return -EINVAL;

    ctx = get_ctx();
    if (!ctx)
        return -EIO;

    state = cmd->entries[0].value.i32;
    roll  = cmd->entries[1].value.i32;
    pitch = cmd->entries[2].value.i32;
    yaw   = cmd->entries[3].value.i32;

    /* Determine rotation angle from IMU data */
    if (roll < -45)
        rotation = ROTATION_270;
    else if (roll > 45)
        rotation = ROTATION_90;

    if (pitch < -45)
        rotation = ROTATION_180;
    else if (pitch > 45)
        rotation = ROTATION_0;

    /* Sync switch state if available */
    if (lv_obj_is_valid(rotation_switch)) {
        if (state)
            lv_obj_add_state(rotation_switch, LV_STATE_CHECKED);
        else
            lv_obj_remove_state(rotation_switch, LV_STATE_CHECKED);
    }

    /* Apply new rotation if changed */
    if (rotation != -1 && rotation != prev_rot) {
        prev_rot = rotation;
        set_scr_rotation(rotation);

        /* Avoid blocking caller thread */
        screen = ctx->scr.now.obj;
        if (lv_obj_is_valid(screen))
            lv_async_call(refresh_object_tree_layout, screen);

        LOG_INFO("Rotation changed to %d (roll=%d, pitch=%d, yaw=%d)",
                 rotation, roll, pitch, yaw);
    }

    return 0;
}
