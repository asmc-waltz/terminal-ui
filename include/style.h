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
extern lv_style_t sts_bar_style;
extern lv_style_t task_bar_style;


extern lv_style_t bg_79;
extern lv_style_t sym_48;


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
void style_application_icons_init(void);

/**********************
 *      MACROS
 **********************/

