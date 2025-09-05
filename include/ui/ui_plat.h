/**
 * @file ui_plat.h
 *
 */

#ifndef G_UI_PLAT_H
#define G_UI_PLAT_H
/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>

#include <lvgl.h>
#include <list.h>
#include <ui/ui_core.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    ID_NONE = 0,
    ID_COMM,
    ID_LAYER_SYS,
    ID_LAYER_TOP,
    ID_LAYER_ACT,
    ID_LAYER_BOT,
    ID_BG,
    ID_SRC_HOME,
    ID_STATUS_BAR,
        ID_STATUS_BAR_LEFT_CTR,
            ID_STATUS_BAR_SIGNAL_STRENGTH,
            ID_STATUS_BAR_SIGNAL_TYPE,
            ID_STATUS_BAR_ETHERNET,
            ID_STATUS_BAR_WIFI,
        ID_STATUS_BAR_MID_CTR,
            ID_STATUS_BAR_CLOCK,
        ID_STATUS_BAR_RIGHT_CTR,
            ID_STATUS_BAR_ALERT,
            ID_STATUS_BAR_POWER,
    ID_TASK_BAR,
        ID_TASK_BAR_PHONE,
        ID_TASK_BAR_MESSAGE,
        ID_TASK_BAR_TOOLBOX,
        ID_TASK_BAR_SETTING,
    ID_HOME_INDICATOR,
    ID_SETTING_MAIN_CTR,
        ID_SETTING_MAIN_TITLE,
        ID_SETTING_SUB_CRT_SEARCH,
            ID_SETTING_SEACH,
        ID_SETTING_SUB_CRT_WIRELESS,
            ID_SETTING_AIRPLANE,
                ID_SETTING_AIRPLANE_SW,
            ID_SETTING_WIFI,
                ID_SETTING_WIFI_STATUS,
            ID_SETTING_HOSTSPOT,
            ID_SETTING_BLUETOOTH,
                ID_SETTING_BLUETOOTH_STATUS,
            ID_SETTING_CELLULAR,
                ID_SETTING_CELLULAR_STATUS,
        ID_SETTING_SUB_CRT_NETWORK,
            ID_SETTING_ETHERNET,
                ID_SETTING_ETHERNET_SW,
            ID_SETTING_NETWORK,
            ID_SETTING_FILTER,
                ID_SETTING_FILTER_SW,
        ID_SETTING_SUB_CRT_PHONE,
            ID_SETTING_PHONE,
            ID_SETTING_MESSAGE,
        ID_SETTING_SUB_CRT_GENERAL,
            ID_SETTING_ALERT_AND_HAPTIC,
                ID_SETTING_ALERT_AND_HAPTIC_SW,
            ID_SETTING_AUDIO,
            ID_SETTING_MICROPHONE,
            ID_SETTING_ROTATE,
                ID_SETTING_ROTATE_SW,
            ID_SETTING_DATE_TIME,
            ID_SETTING_GNSS,
            ID_SETTING_BRIGHTNESS,
                ID_SETTING_BRIGHTNESS_CTR,
                    ID_SETTING_BRIGHTNESS_AUTO,
                    ID_SETTING_BRIGHTNESS_BAR,
                    ID_SETTING_BRIGHTNESS_AUTO_RANGE,
            ID_SETTING_POWER,
            ID_SETTING_ABOUT,
    ID_SETTING_SUB_CTR,
        ID_SETTING_SUB_TITLE,
    ID_CONTROL_CENTER,
    ID_SYSTEM_STATUS,
} g_id;

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
g_handler * gf_register_handler(lv_obj_t *par, uint32_t id, lv_event_cb_t event_cb, lv_event_code_t filter);

/**********************
 *      MACROS
 **********************/

#endif /* G_UI_PLAT_H */
