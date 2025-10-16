/**
 * @file widget.h
 *
 */

#ifndef G_WIDGET_H
#define G_WIDGET_H
/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>

#include <lvgl.h>
#include "ui/ui_core.h"

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
int32_t set_active_menu_page(lv_obj_t *menu, \
                             lv_obj_t *(*create_page_cb)(lv_obj_t *, \
                                                         lv_obj_t *, \
                                                         const char *));

/*=====================
 * Getter functions
 *====================*/

/*=====================
 * Other functions
 *====================*/
lv_obj_t *create_vertical_flex_group(lv_obj_t *par, const char *name);
lv_obj_t *create_horizontal_flex_group(lv_obj_t *par, const char *name);

lv_obj_t *create_menu(lv_obj_t *par, const char *name);
lv_obj_t *create_menu_bar(lv_obj_t *menu);
lv_obj_t *create_menu_group(lv_obj_t *par, const char *name);
lv_obj_t *create_menu_item(lv_obj_t *menu, lv_obj_t *menu_bar, \
                           const char *sym_index, const char *title,\
                           lv_obj_t *(* create_page_cb)(lv_obj_t *, \
                                                        lv_obj_t *, \
                                                        const char *));
lv_obj_t *create_menu_page(lv_obj_t *menu, lv_obj_t *par, \
                           const char *name);

/**********************
 *      MACROS
 **********************/

#endif /* G_WIDGET_H */
