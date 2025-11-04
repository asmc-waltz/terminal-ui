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
typedef struct {
    lv_obj_t *l_container;
    lv_obj_t *selected_opt;
    lv_obj_t *r_container;
    lv_obj_t *detail_pane;
    lv_obj_t *(*create_detail_pane_cb)(lv_obj_t*, lv_obj_t *, const char *);
    bool detail_visible;
    bool split_view;
} menu_ctx_t;

typedef struct {
    lv_obj_t *menu;
    lv_obj_t *(*create_detail_pane_cb)(lv_obj_t *, lv_obj_t *, const char *);
} item_ctx_t;

typedef struct menu_view {
    /* Root object of this menu */
    lv_obj_t *menu;

    /*
     * Optional control bar of the submenu.
     * Appear only when this menu lives inside another menu (nested mode).
     * Provide back-navigation or action buttons.
     */
    lv_obj_t *sub_ctrl;

    /* Sub menu container, if any (NULL for leaf menu) */
    lv_obj_t *sub_menu;

    /* Optional: link to parent view if this is nested menu */
    struct menu_view *parent;
} menu_view_t;

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
                             lv_obj_t *(*create_detail_pane_cb)(lv_obj_t *, \
                                                         lv_obj_t *, \
                                                         const char *));

/*=====================
 * Getter functions
 *====================*/

/*=====================
 * Other functions
 *====================*/
lv_obj_t *create_vertical_flex_group(lv_obj_t *par, const char *name);
lv_obj_t *create_vscroll_flex_group(lv_obj_t *par, const char *name);
lv_obj_t *create_horizontal_flex_group(lv_obj_t *par, const char *name);
lv_obj_t *create_hscroll_flex_group(lv_obj_t *par, \
                                                const char *name);

lv_obj_t *create_menu_view(lv_obj_t *par, const char *name, bool split_view);
lv_obj_t *create_menu_bar(lv_obj_t *menu);
lv_obj_t *create_menu_group(lv_obj_t *par, const char *name);

lv_obj_t *create_option_cell(lv_obj_t *par, const char *name);
int32_t create_basic_item(lv_obj_t *item, \
                          const lv_font_t *sym_font, const char *sym_index, \
                          const lv_font_t *title_font, const char *title);
lv_obj_t *create_menu_item(lv_obj_t *par, \
                           const lv_font_t *sym_font, const char *sym_index, \
                           const lv_font_t *title_font, const char *title);

int32_t set_item_menu_page(lv_obj_t *lobj, lv_obj_t *menu, \
                           lv_obj_t *(*create_detail_pane_cb)(lv_obj_t *, \
                                                        lv_obj_t *, \
                                                        const char *));
lv_obj_t *create_menu_page(lv_obj_t *menu, lv_obj_t *par, \
                           const char *name);

lv_obj_t *create_sub_menu_view(lv_obj_t *menu, lv_obj_t *par, \
                               const char *name, \
                               lv_obj_t *(*sub_menu_creator)(lv_obj_t *, \
                                                          const char *, bool));

/**********************
 *      MACROS
 **********************/

#endif /* G_WIDGET_H */
