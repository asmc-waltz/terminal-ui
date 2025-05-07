/**
 * @file screens.h
 *
 */

#ifndef G_SCREENS_H
#define G_SCREENS_H
/*********************
 *      INCLUDES
 *********************/
#include <lvgl.h>

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
void gf_create_home_screen();

void gf_create_setting_menu(lv_obj_t *par);
void gf_hide_setting();
void gf_show_setting();
/**********************
 *      MACROS
 **********************/

#endif /* G_SCREENS_H */
