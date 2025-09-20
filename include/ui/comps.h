/**
 * @file comps.h
 *
 */

#ifndef G_COMPS_H
#define G_COMPS_H
/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>

#include <lvgl.h>
#include "ui/ui_core.h"
#include "ui/ui.h"

/*********************
 *      DEFINES
 *********************/
#define TOP_BAR_NAME                    "comps.top_bar"
#define TOP_BAR_WIDTH                   98      // %
#define TOP_BAR_HEIGHT                  7       // %
#define TOP_BAR_PAD_TOP                 2       // %
#define TOP_BAR_PAD_BOT                 1       // %
#define TOP_BAR_PAD_LEFT                1       // %
#define TOP_BAR_PAD_RIGHT               1       // %
#define TOP_BAR_SYM_ALN                 5       // pixel
#define TOP_BAR_SYM_FONTS               &terminal_icons_32


#define KEYBOAR_NAME                    "comps.keyboard"
#define KEYBOARD_WIDTH                  98      // %
#define HOR_KEYBOARD_HEIGHT             40      // %
#define VER_KEYBOARD_HEIGHT             30      // %
#define KEYBOARD_BOT_PAD                2       // %
#define KEYBOARD_PAD_LEFT               1       // %
#define KEYBOARD_PAD_RIGHT              1       // %
#define KEYBOARD_CHAR_FONTS             &lv_font_montserrat_24


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
int32_t replace_top_bar_symbol(lv_obj_t *par, const char *name, \
                               const char *index);

lv_obj_t *add_top_bar_symbol(lv_obj_t *par, const char *name, \
                             const char *index);

/*=====================
 * Getter functions
 *====================*/

/*=====================
 * Other functions
 *====================*/
lv_obj_t *create_top_bar(lv_obj_t *par);

lv_obj_t *create_keyboard(lv_obj_t *par);
void remove_keyboard(lv_obj_t *par);

/**********************
 *      MACROS
 **********************/

#endif /* G_COMPS_H */
