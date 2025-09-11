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
#include <ui/ui_core.h>
#include <ui/ui.h>

/*********************
 *      DEFINES
 *********************/
#define TOP_BAR_WIDTH                   98      // %
#define TOP_BAR_HEIGHT                  7       // %
#define TOP_BAR_PAD_TOP                 1       // %
#define TOP_BAR_PAD_BOT                 1       // %
#define TOP_BAR_PAD_LEFT                1       // %
#define TOP_BAR_PAD_RIGHT               1       // %
#define TOP_BAR_SYM_ALN                 5       // pixel
#define TOP_BAR_SYM_FONTS               &terminal_icons_32


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
lv_obj_t *create_keyboard(lv_obj_t *par);
lv_obj_t *create_top_bar(lv_obj_t *par);

/**********************
 *      MACROS
 **********************/

#endif /* G_COMPS_H */
