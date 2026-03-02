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

#include "comm/cmd_payload.h"

#include <lvgl.h>
#include <crobj.h>

/*********************
 *      DEFINES
 *********************/
#define SETTING_BASED_NAME              "SETTING"

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
int32_t create_setting_filler(lv_obj_t *par);

lv_obj_t *create_brightness_setting(lv_obj_t *par, const char *name, \
                                    view_ctn_t *par_v_ctx);
lv_obj_t *create_airplane_setting(lv_obj_t *par, const char *name, \
                                  view_ctn_t *par_v_ctx);
lv_obj_t *create_wifi_setting(lv_obj_t *par, const char *name, \
                              view_ctn_t *par_v_ctx);
lv_obj_t *create_bluetooth_setting(lv_obj_t *par, const char *name, \
                                   view_ctn_t *par_v_ctx);
lv_obj_t *create_cellular_setting(lv_obj_t *par, const char *name, \
                                  view_ctn_t *par_v_ctx);
lv_obj_t *create_hotspot_setting(lv_obj_t *par, const char *name, \
                                 view_ctn_t *par_v_ctx);
lv_obj_t *create_rotation_setting(lv_obj_t *par, const char *name, \
                                  view_ctn_t *par_v_ctx);

lv_obj_t *create_setting_window(lv_obj_t *par, const char *name);

int32_t handle_imu_rotation_state(remote_cmd_t *cmd);
int32_t handle_backlight_state(remote_cmd_t *cmd);
int32_t handle_wifi_state(remote_cmd_t *cmd);
int32_t handle_wifi_access_point(remote_cmd_t *cmd);
/**********************
 *      MACROS
 **********************/

#endif /* G_WINDOWS_H */
