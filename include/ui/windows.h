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

/*********************
 *      DEFINES
 *********************/
#define SETTING_PAGE_NAME               "pages.setting"
#define SETTING_MENU_BAR_HOR_WIDTH      30      // %
#define SETTING_PAD_TOP                 1       // %
#define SETTING_PAD_BOT                 1       // %
#define SETTING_PAD_LEFT                1       // %
#define SETTING_PAD_MID_HOR             1       // %
#define SETTING_PAD_MID_VER             2       // %
#define SETTING_PAD_RIGHT               1       // %
#define SETTING_WIDTH                   (100 - SETTING_PAD_LEFT - \
                                         SETTING_PAD_RIGHT)      // %
#define SETTING_SYM_FONTS               &terminal_icons_20

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
lv_obj_t *create_setting_window(ctx_t *ctx);

/**********************
 *      MACROS
 **********************/

#endif /* G_WINDOWS_H */
