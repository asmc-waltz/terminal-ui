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
enum container_side {
    CONTAINER_LEFT = 0,
    CONTAINER_RIGHT,
};

/**********************
 *      TYPEDEFS
 **********************/
/*
 * Configuration of a menu view.
 */
typedef struct view_cfg {
    bool ctrl;
    bool split_view;
} view_conf_t;

typedef struct menu_view menu_view_t;

/*
 * Window data structure contains all information about one menu window,
 * including its panes (controller, menu, etc.), the selected option,
 * and visibility state. Child windows reuse the same control pane and holder.
 */
typedef struct window {
    lv_obj_t *container;
    lv_obj_t *menu;         /* Left-side menu container (static element) */
    lv_obj_t *overlay_menu;        /* */
    lv_obj_t *selected_opt;
    lv_obj_t *(*create_window_cb)(lv_obj_t *, const char *);
    bool visible;
} window_t;

/*
 * Internal data of a menu view. Stored inside the holder (grid layout)
 * and manages left/right window layout for split-view mode.
 */
typedef struct menu_view {
    lv_obj_t *container;        /* Top-level container holding all subwindows */
    lv_obj_t *view_ctrl;        /* Optional: control bar for nested menu navigation */
    lv_obj_t *view;             /* Grid layout for main content (split view) */

    view_conf_t cfg;            /* Configuration for view appearance/behavior */

    window_t l_win;             /* Left-side window context (menu options) */
    window_t r_win;             /* Right-side window context (detail/content) */
    window_t *act_win;          /* Currently active window (either left or right) */
} menu_view_t;

/*
 * Each option inside a menu window. The menu option data stores
 * the view it belongs to, and the callback to create its child
 * window when interacted.
 */
typedef struct {
    lv_obj_t *(*create_window_cb)(lv_obj_t *, const char *);
    menu_view_t *view_ctx;
} menu_opt_t;

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*=====================
 * Setter functions
 *====================*/
int32_t set_active_window(lv_obj_t *menu, \
                          lv_obj_t *(*create_window_cb)(lv_obj_t *, \
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

lv_obj_t *create_menu(lv_obj_t *menu);
lv_obj_t *create_menu_group(lv_obj_t *par, const char *name);

lv_obj_t *create_option_cell(lv_obj_t *par, const char *name);
int32_t create_basic_item(lv_obj_t *item, \
                          const lv_font_t *sym_font, const char *sym_index, \
                          const lv_font_t *title_font, const char *title);
lv_obj_t *create_menu_item(lv_obj_t *par, \
                           const lv_font_t *sym_font, const char *sym_index, \
                           const lv_font_t *title_font, const char *title);

int32_t set_item_menu_page(lv_obj_t *lobj, lv_obj_t *view, \
                           lv_obj_t *(* create_window_cb)(lv_obj_t *, \
                                                          const char *));

menu_view_t *create_menu_view(lv_obj_t *par, const char *name, \
                              bool ctrl, bool split);

static inline menu_view_t *get_view_ctx(lv_obj_t *lobj) {
    return lobj ? (menu_view_t *)get_internal_data(lobj) : NULL;
}

static inline menu_opt_t *get_opt_ctx(lv_obj_t *lobj) {
    return lobj ? (menu_opt_t *)get_internal_data(lobj) : NULL;
}

/**********************
 *      MACROS
 **********************/

#endif /* G_WIDGET_H */
