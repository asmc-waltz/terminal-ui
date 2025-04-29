/**
 * @file style.h
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <lvgl.h>
#include <list.h>

/*********************
 *      DEFINES
 *********************/
typedef struct {
    struct list_head node;
    char id;
    lv_style_t style;
} g_style;

enum {
	STY_BG = 1,
	STY_TASKBAR,
    STY_BG_ICON_79,
    STY_SYM_48,
};
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/
extern lv_style_t icons_size_20;
extern lv_style_t icons_size_32;
extern lv_style_t sts_bar_style;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*=====================
 * Setter functions
 *====================*/
/*=====================
 * Getter functions
 *====================*/
lv_style_t * gf_get_lv_style(char style_id);
/*=====================
 * Other functions
 *====================*/
void styles_init(void);

/**********************
 *      MACROS
 **********************/

