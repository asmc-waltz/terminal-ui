/**
 * @file setting.c
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
#include "main.h"

/*********************
 *      DEFINES
 *********************/
#define SETTING_USED_HEIGHT             (TOP_BAR_PAD_TOP + TOP_BAR_HEIGHT + \
                                         SETTING_PAD_TOP + \
                                         SETTING_PAD_BOT) // %
#define SETTING_CONTAINTER_ALIGN        SETTING_PAD_TOP // %

#define SETTING_MENU_BAR_HOR_WIDTH      30 // %
#define SETTING_MENU_BAR_VER_WIDTH      40 // %

#define SETTING_MENU_HOR_ALIGN          (SETTING_PAD_TOP + SETTING_PAD_BOT) // %
#define SETTING_MENU_VER_ALIGN          (SETTING_PAD_TOP + SETTING_PAD_BOT) // %

#define SETTING_CONT_BG_COLOR           0x636D7A
#define SETTING_MENU_BG_COLOR           0x8F9DB0
#define SETTING_MENU_BTN_BG_COLOR       0xADBACC

#define SETTING_DETAIL_BG_COLOR         SETTING_MENU_BG_COLOR
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

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t *create_setting_window(ctx_t *ctx)
{
    lv_obj_t *par;

    if (!ctx || !ctx->scr.now.obj)
        return NULL;

    par = ctx->scr.now.obj;

    return NULL;
}
