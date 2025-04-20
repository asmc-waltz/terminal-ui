/**
 * @file style.h
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
extern lv_style_t icons_size_20;
extern lv_style_t icons_size_32;
extern lv_style_t icons_size_48;

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
void style_curved_lines_init(lv_style_t *style);
void icons_20_style_init(void);
void icons_32_style_init(void);
void icons_48_style_init(void);

/**********************
 *      MACROS
 **********************/

