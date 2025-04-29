/**
 * @file style.h
 *
 */

#ifndef G_STYLE_H
#define G_STYLE_H
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
    STY_STATUS_BAR,
    STY_BG_ICON_37,
    STY_SYM_20,
    STY_SYM_32,
};
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
lv_style_t * gf_get_lv_style(char style_id);
/*=====================
 * Other functions
 *====================*/
void gf_styles_init(void);
void sf_delete_all_style_data(void);

/**********************
 *      MACROS
 **********************/

#endif /* G_STYLE_H */
