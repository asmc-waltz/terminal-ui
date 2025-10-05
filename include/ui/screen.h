/**
 * @file screen.h
 *
 */

#ifndef G_SCREEN_H
#define G_SCREEN_H
/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>

#include <lvgl.h>

/*********************
 *      DEFINES
 *********************/
#define UI_LVGL_TIMER_MS                5

#define DISP_WIDTH                      1024
#define DISP_HEIGHT                     600

#define DRM_CARD                        "/dev/dri/card0"
#define DRM_CONNECTOR_ID                32

#define TOUCH_EVENT_FILE                "/dev/input/event1"

#define LAYOUT_SETTING                  "layout.setting"
/**********************
 *      TYPEDEFS
 **********************/
typedef struct ctx ctx_t;

typedef struct scr_space {
    lv_obj_t *obj;
} space_t;

typedef struct screen {
    lv_display_t *drm_disp;
    lv_indev_t *touch_event;
    int32_t width;
    int32_t height;
    int32_t rotation;
    space_t now;
} scr_ctx_t;

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*=====================
 * Setter functions
 *====================*/

/*=====================
 * Getter functions
 *====================*/

/*=====================
 * Other functions
 *====================*/
lv_obj_t *create_common_screen(ctx_t *ctx, lv_obj_t *par, const char *name);

/**********************
 *      MACROS
 **********************/

#endif /* G_SCREEN_H */
