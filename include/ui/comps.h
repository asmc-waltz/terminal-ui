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

#endif /* G_COMPS_H */
