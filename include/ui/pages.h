/**
 * @file pages.h
 *
 */

#ifndef G_PAGES_H
#define G_PAGES_H
/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>

#include <lvgl.h>
#include "ui/ui_core.h"
#include "ui/screen.h"
#include "main.h"

/*********************
 *      DEFINES
 *********************/
#define SETTING_PAGE_NAME               "pages.setting"
#define SETTING_MENU_BAR_HOR_WIDTH      30      // %
#define SETTING_PAD_TOP                 1       // %
#define SETTING_PAD_BOT                 2       // %
#define SETTING_PAD_LEFT                1       // %
#define SETTING_PAD_RIGHT               1       // %
#define SETTING_WIDTH                   (100 - SETTING_PAD_LEFT - \
                                         SETTING_PAD_RIGHT)      // %
#define SETTING_SYM_FONTS               &terminal_icons_20

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
lv_obj_t *create_common_screen(ctx_t *ctx, lv_obj_t *par, const char *name);
lv_obj_t *create_setting_page(ctx_t *ctx);

/**********************
 *      MACROS
 **********************/

#endif /* G_PAGES_H */
