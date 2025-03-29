/**
 * @file screens.h
 *
 */

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
extern bool kb_visible;
extern lv_obj_t *glob_kb;

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
void home_screen(lv_style_t *style);
void keyboard_create();
void hidden_keyboard();
void show_keyboard();

/**********************
 *      MACROS
 **********************/

