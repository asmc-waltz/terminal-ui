/**
 * @file ui.h
 *
 */

#ifndef G_UI_H
#define G_UI_H
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
lv_obj_t *gf_create_keyboard(lv_obj_t *par, const char *name, \
                             int32_t keyboard_w, int32_t keyboard_h);
/**********************
 *      MACROS
 **********************/

#endif /* G_UI_H */
