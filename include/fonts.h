/**
 * @file fonts.h
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <lvgl.h>

/*********************
 * DEFINES
 *********************/
#define ANGLE_DOWN_SOLID 0xf107
#define ANGLES_DOWN_SOLID 0xf103
#define ANGLES_UP_SOLID 0xf102
#define ANGLE_UP_SOLID 0xf106 
#define ARROW_DOWN_SOLID 0xf063
#define ARROW_UP_SOLID 0xf062
#define BATTERY_EMPTY_SOLID 0xf244
#define BATTERY_FULL_SOLID 0xf240
#define BATTERY_HALF_SOLID 0xf242
#define BATTERY_QUARTER_SOLID 0xf243
#define BATTERY_THREE_QUARTERS_SOLID 0xf241
#define BELL_SLASH_SOLID 0xf1f6
#define BELL_SOLID 0xf0f3
#define BOLT_SOLID 0xf0e7
#define CALENDAR_DAYS_SOLID 0xf073
#define CHECK_SOLID 0xf00c
#define CIRCLE_CHECK_SOLID 0xf058
#define CIRCLE_HALF_STROKE_SOLID 0xf042
#define CIRCLE_INFO_SOLID 0xf05a
#define CIRCLE_MINUS_SOLID 0xf056
#define CIRCLE_UP 0xf35b
#define CIRCLE_PLUS_SOLID 0xf055
#define CLOCK_SOLID 0xf017
#define COMMENT_SOLID 0xf075
#define DOWNLOAD_SOLID 0xf019
#define ETHERNET_SOLID 0xf796
#define FILTER_SOLID 0xf0b0
#define GEAR_SOLID 0xf013
#define HEADPHONES_SOLID 0xf025
#define HOURGLASS_END_SOLID 0xf253
#define HOURGLASS_HALF_SOLID 0xf252
#define HOURGLASS_SOLID 0xf254
#define HOURGLASS_START_SOLID 0xf251
#define LOCATION_DOT_SOLID 0xf3c5
#define LOCK_OPEN_SOLID 0xf3c1
#define LOCK_SOLID 0xf023
#define MAGNIFYING_GLASS_SOLID 0xf002
#define MICROPHONE_SLASH_SOLID 0xf131
#define MICROPHONE_SOLID 0xf130
#define MINUS_SOLID 0xf068
#define NETWORK_WIRED_SOLID 0xf6ff
#define PHONE_SOLID 0xf095
#define PLANE_SOLID 0xf072
#define PLUG_CIRCLE_BOLT_SOLID 0xe55b
#define PLUG_SOLID 0xf1e6
#define PLUS_SOLID 0x2b
#define POWER_OFF_SOLID 0xf011
#define ROTATE_SOLID 0xf2f1
#define SATELLITE_DISH_SOLID 0xf7c0
#define SATELLITE_SOLID 0xf7bf
#define SHARE_NODES_SOLID 0xf1e0
#define SHIELD_SOLID 0xf132
#define SHIELD_HALVED 0xf3ed
#define SIGNAL_SOLID 0xf012
#define TEMPERATURE_HALF_SOLID 0xf2c9
#define TOOLBOX_SOLID 0xf552
#define TOWER_BROADCAST_SOLID 0xf519
#define TOWER_CELL_SOLID 0xe585
#define UPLOAD_SOLID 0xf093
#define VOLUME_HIGH_SOLID 0xf028
#define VOLUME_LOW_SOLID 0xf027
#define VOLUME_OFF_SOLID 0xf026
#define VOLUME_XMARK_SOLID 0xf6a9
#define WIFI_SOLID 0xf1eb

/**********************
 *     ICONS
 **********************/
#define ICON_ANGLE_DOWN_SOLID                   "\xEF\x84\x87"
#define ICON_ANGLES_DOWN_SOLID                  "\xEF\x84\x83"
#define ICON_ANGLES_UP_SOLID                    "\xEF\x84\x82"
#define ICON_ANGLE_UP_SOLID                     "\xEF\x84\x86"
#define ICON_ARROW_DOWN_SOLID                   "\xEF\x81\xA3"
#define ICON_ARROW_UP_SOLID                     "\xEF\x81\xA2"
#define ICON_BATTERY_EMPTY_SOLID                "\xEF\x89\x84"
#define ICON_BATTERY_FULL_SOLID                 "\xEF\x89\x80"
#define ICON_BATTERY_HALF_SOLID                 "\xEF\x89\x82"
#define ICON_BATTERY_QUARTER_SOLID              "\xEF\x89\x83"
#define ICON_BATTERY_THREE_QUARTERS_SOLID       "\xEF\x89\x81"
#define ICON_BELL_SLASH_SOLID                   "\xEF\x87\xB6"
#define ICON_BELL_SOLID                         "\xEF\x83\xB3"
#define ICON_BOLT_SOLID                         "\xEF\x83\xA7"
#define ICON_CALENDAR_DAYS_SOLID                "\xEF\x81\xB3"
#define ICON_CHECK_SOLID                        "\xEF\x80\x8C"
#define ICON_CIRCLE_CHECK_SOLID                 "\xEF\x81\x98"
#define ICON_CIRCLE_HALF_STROKE_SOLID           "\xEF\x81\x82"
#define ICON_CIRCLE_INFO_SOLID                  "\xEF\x81\x9A"
#define ICON_CIRCLE_MINUS_SOLID                 "\xEF\x81\x96"
#define ICON_CIRCLE_UP                          "\xEF\x8D\x9B"
#define ICON_CIRCLE_PLUS_SOLID                  "\xEF\x81\x95"
#define ICON_CLOCK_SOLID                        "\xEF\x80\x97"
#define ICON_COMMENT_SOLID                      "\xEF\x81\xB5"
#define ICON_DOWNLOAD_SOLID                     "\xEF\x80\x99"
#define ICON_ETHERNET_SOLID                     "\xEF\x9E\x96"
#define ICON_FILTER_SOLID                       "\xEF\x82\xB0"
#define ICON_GEAR_SOLID                         "\xEF\x80\x93"
#define ICON_HEADPHONES_SOLID                   "\xEF\x80\xA5"
#define ICON_HOURGLASS_END_SOLID                "\xEF\x89\x93"
#define ICON_HOURGLASS_HALF_SOLID               "\xEF\x89\x92"
#define ICON_HOURGLASS_SOLID                    "\xEF\x89\x94"
#define ICON_HOURGLASS_START_SOLID              "\xEF\x89\x91"
#define ICON_LOCATION_DOT_SOLID                 "\xEF\x8F\x85"
#define ICON_LOCK_OPEN_SOLID                    "\xEF\x8F\x81"
#define ICON_LOCK_SOLID                         "\xEF\x80\xA3"
#define ICON_MAGNIFYING_GLASS_SOLID             "\xEF\x80\x82"
#define ICON_MICROPHONE_SLASH_SOLID             "\xEF\x84\xB1"
#define ICON_MICROPHONE_SOLID                   "\xEF\x84\xB0"
#define ICON_MINUS_SOLID                        "\xEF\x81\xA8"
#define ICON_NETWORK_WIRED_SOLID                "\xEF\x9B\xBF"
#define ICON_PHONE_SOLID                        "\xEF\x82\x95"
#define ICON_PLANE_SOLID                        "\xEF\x81\xB2"
#define ICON_PLUG_CIRCLE_BOLT_SOLID             "\xEE\x95\x9B"
#define ICON_PLUG_SOLID                         "\xEF\x87\xA6"
#define ICON_PLUS_SOLID                         "\x2B"
#define ICON_POWER_OFF_SOLID                    "\xEF\x80\x91"
#define ICON_ROTATE_SOLID                       "\xEF\x8B\xB1"
#define ICON_SATELLITE_DISH_SOLID               "\xEF\x9F\x80"
#define ICON_SATELLITE_SOLID                    "\xEF\x9E\xBF"
#define ICON_SHARE_NODES_SOLID                  "\xEF\x87\xA0"
#define ICON_SHIELD_SOLID                       "\xEF\x84\xB2"
#define ICON_SHIELD_HALVED                      "\xEF\x8F\xAD"
#define ICON_SIGNAL_SOLID                       "\xEF\x80\x92"
#define ICON_TEMPERATURE_HALF_SOLID             "\xEF\x8B\x89"
#define ICON_TOOLBOX_SOLID                      "\xEF\x95\x92"
#define ICON_TOWER_BROADCAST_SOLID              "\xEF\x94\x99"
#define ICON_TOWER_CELL_SOLID                   "\xEE\x96\x85"
#define ICON_UPLOAD_SOLID                       "\xEF\x82\x93"
#define ICON_VOLUME_HIGH_SOLID                  "\xEF\x80\xA8"
#define ICON_VOLUME_LOW_SOLID                   "\xEF\x80\xA7"
#define ICON_VOLUME_OFF_SOLID                   "\xEF\x80\xA6"
#define ICON_VOLUME_XMARK_SOLID                 "\xEF\x9A\xA9"
#define ICON_WIFI_SOLID                         "\xEF\x87\xAB"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/
extern lv_font_t terminal_icons_20;
extern lv_font_t terminal_icons_32;
extern lv_font_t terminal_icons_48;

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
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

