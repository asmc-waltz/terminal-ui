/**
 * @file ui_controller.c
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
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <lvgl.h>
#include "list.h"
#include "ui/ui_core.h"
#include "ui/fonts.h"
#include "ui/comps.h"
#include "ui/pages.h"
#include "ui/screen.h"

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
static lv_display_t *sf_init_drm_display(const char *file, \
                                         int64_t connector_id)
{
    lv_display_t *disp = NULL;
    int32_t scr_width = 0;
    int32_t scr_height = 0;

    scr_width = get_scr_width();
    scr_height = get_scr_height();
    if (scr_width <= 0 || scr_height <= 0) {
        LOG_FATAL("Display width or height resolution not available");
        return NULL;
    }

    disp = lv_linux_drm_create();
    if (disp == NULL) {
        LOG_FATAL("Failed to initialize the display");
        return NULL;
    }

    lv_display_set_default(disp);
    lv_linux_drm_set_file(disp, file, connector_id);
    lv_display_set_resolution(disp, scr_width, scr_height);

    return disp;
}

static lv_indev_t *sf_init_touch_screen(const char *dev_path, \
                                        lv_display_t *disp)
{
    lv_indev_t *indev = NULL;

    indev = lv_evdev_create(LV_INDEV_TYPE_POINTER, dev_path);
    if (!indev) {
        LOG_FATAL("Failed to initialize touch input device");
        return NULL;
    }

    lv_indev_set_display(indev, disp);

    return indev;
}

static void gtimer_handler(lv_timer_t * timer)
{
    lv_tick_inc(UI_LVGL_TIMER_MS);
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int32_t ui_main_init(ctx_t *ctx)
{
    lv_timer_t *task_timer = NULL;
    int32_t ret;

    ret = init_ui_object_ctx(ctx);
    if (ret) {
        LOG_FATAL("Unable to init ui object list head");
        return ret;
    }

    ctx->objs.next_id = 1;

    set_scr_size(DISP_WIDTH, DISP_HEIGHT);

    // Initialize LVGL and the associated UI hardware
    lv_init();
    ctx->scr.drm_disp = sf_init_drm_display(DRM_CARD, DRM_CONNECTOR_ID);
    if (ctx->scr.drm_disp == NULL) {
        return -EIO;
    }

    ctx->scr.touch_event = sf_init_touch_screen(TOUCH_EVENT_FILE, \
                                                ctx->scr.drm_disp);
    if (ctx->scr.touch_event == NULL) {
        return -EIO;
    }

    task_timer = lv_timer_create(gtimer_handler, UI_LVGL_TIMER_MS,  NULL);
    if (task_timer == NULL) {
        LOG_FATAL("Failed to create timer for LVGL task handler");
        return -ENOMEM;
    }

    // Make lv_timer ready. It will not wait its period.
    lv_timer_ready(task_timer);

    // Initialize LVGL layers as base components
    register_obj(NULL, lv_layer_sys(), NULL);
    register_obj(NULL, lv_layer_top(), NULL);
    register_obj(NULL, lv_screen_active(), NULL);
    register_obj(NULL, lv_layer_bottom(), NULL);

    create_scr_page(lv_screen_active(), "screens.common");
    LOG_DEBUG("size of gobj_t: %d", sizeof(gobj_t));

    return 0;
}

void ui_main_deinit(ctx_t *ctx)
{
    destroy_ui_object_ctx(ctx);
}
