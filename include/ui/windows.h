/**
 * @file window.h
 *
 */

#ifndef G_WINDOWS_H
#define G_WINDOWS_H
/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>

#include <lvgl.h>
#include "ui/ui_core.h"
#include "ui/screen.h"
#include "ui/widget/menu.h"

/*********************
 *      DEFINES
 *********************/
#define WINDOW_SETTING                  "window.setting"

/**********************
 *      TYPEDEFS
 **********************/
typedef struct ctx ctx_t;

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
view_ctn_t *create_common_setting_view(lv_obj_t *par, const char *name,
                                       bool root, bool split);
int32_t create_setting_filler(lv_obj_t *par);

lv_obj_t *create_brightness_setting(lv_obj_t *par, const char *name);
lv_obj_t *create_airplane_setting(lv_obj_t *par, const char *name);
lv_obj_t *create_wifi_setting(lv_obj_t *par, const char *name);
lv_obj_t *create_bluetooth_setting(lv_obj_t *par, const char *name);
lv_obj_t *create_cellular_setting(lv_obj_t *par, const char *name);
lv_obj_t *create_hotspot_setting(lv_obj_t *par, const char *name);
lv_obj_t *create_rotation_setting(lv_obj_t *par, const char *name);

/**********************
 *      MACROS
 **********************/

#endif /* G_WINDOWS_H */
