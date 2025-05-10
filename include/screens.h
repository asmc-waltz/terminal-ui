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

void gf_create_control_center(lv_obj_t *par);
void gf_hide_control_center(void);
void gf_show_control_center(void);
void gf_delete_control_center(void);

void gf_create_system_status(lv_obj_t *par);
void gf_hide_system_status(void);
void gf_show_system_status(void);
void gf_delete_system_status(void);
/**********************
 *      MACROS
 **********************/

#endif /* G_SCREENS_H */
