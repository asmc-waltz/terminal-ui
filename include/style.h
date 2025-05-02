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

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
	STY_BG = 1,
	STY_TASKBAR,
    STY_BG_ICON_79,
    STY_SYM_48,
    STY_STATUS_BAR,
    STY_BG_ICON_37,
    STY_SYM_20,
    STY_SYM_32,
    STY_HOME_INDICATOR,
    STY_SETTING,
    STY_SETTING_MAIN_CTR,
    STY_SETTING_MAIN_CHILD_CTR,
    STY_SETTING_BTN,
    STY_SETTING_SW,
    STY_SETTING_TXT,
    STY_SETTING_SUB_CTR,
    STY_SETTING_SUB_CHILD_CTR,
} g_style_id;

typedef struct {
    struct list_head node;
    g_style_id id;
    lv_style_t style;
} g_style;

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
