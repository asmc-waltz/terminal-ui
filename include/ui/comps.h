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
#include "ui/screen.h"

/*********************
 *      DEFINES
 *********************/
#define COMPS_TOP_BAR                   "comps.top_bar"
#define TOP_BAR_SYM_FONTS               &terminal_icons_32

#define COMPS_KEYBOARD                  "comps.keyboard"
#define KEYBOARD_CHAR_FONTS             &lv_font_montserrat_24

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
lv_obj_t *create_top_bar(ctx_t *ctx);

lv_obj_t *create_keyboard(lv_obj_t *par);
void remove_keyboard(ctx_t *ctx);

/**********************
 *      MACROS
 **********************/

#endif /* G_COMPS_H */
