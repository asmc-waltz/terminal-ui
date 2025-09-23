/**
 * @file ui.h
 *
 */

#ifndef G_UI_H
#define G_UI_H
/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>

#include <lvgl.h>
#include "list.h"
#include "ui/ui_core.h"

/*********************
 *      DEFINES
 *********************/
#define UI_LVGL_TIMER_MS                5

#define DISP_WIDTH                      1024
#define DISP_HEIGHT                     600

#define DRM_CARD                        "/dev/dri/card0"
#define DRM_CONNECTOR_ID                32

#define TOUCH_EVENT_FILE                "/dev/input/event1"

/**********************
 *      TYPEDEFS
 **********************/

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
int32_t ui_main_init(ctx_t *ctx);
void ui_main_deinit(ctx_t *ctx);

/**********************
 *      MACROS
 **********************/

#endif /* G_UI_H */
