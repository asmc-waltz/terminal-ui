/**
 * @file menu.h
 *
 */

#ifndef G_MENU_H
#define G_MENU_H
/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>

#include <lvgl.h>
#include "../ui_core.h"

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

typedef struct view_ctn view_ctn_t;

/*
 * Window data structure contains all information about one menu window,
 * including its panes (controller, menu, etc.), the selected option,
 * and visibility state. Child windows reuse the same control pane and holder.
 */
typedef struct win_ctn {
    lv_obj_t *container;
    lv_obj_t *menu;             /* Left-side menu container (static element) */
    lv_obj_t *overlay_menu;     /* Sub menu - render on top of menu*/
    lv_obj_t *selected_opt;
    lv_obj_t *(*create_window_cb)(lv_obj_t *, const char *, view_ctn_t *);
    bool visible;
} win_ctn_t;

/*
 * Internal data of a menu view. Stored inside the holder (grid layout)
 * and manages left/right window layout for split-view mode.
 */
typedef struct view_ctn {
    lv_obj_t *container;    /* Top-level container holding all subwindows */
    lv_obj_t *view_ctrl;    /* Optional: control for nested menu navigation */
    lv_obj_t *view;         /* Grid layout for main content (split view) */

    view_conf_t cfg;        /* Configuration for view appearance/behavior */

    win_ctn_t l_ctn;      /* Left-side window context (menu options) */
    win_ctn_t r_ctn;      /* Right-side window context (detail/content) */

    /*
     * Pointer to the currently active container (either left or right).
     * Child menus are created inside this container.
     */
    win_ctn_t *opened_ctn;
    view_ctn_t *par_v_ctx;
} view_ctn_t;

/*
 * Each option inside a menu window. The menu option data stores
 * the view it belongs to, and the callback to create its child
 * window when interacted.
 */
typedef struct {
    lv_obj_t *(*create_window_cb)(lv_obj_t *, const char *, view_ctn_t *);
    view_ctn_t *view_ctx;
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
int32_t set_and_load_window(lv_obj_t *view, \
                            lv_obj_t *(*create_window_cb)(lv_obj_t *, \
                                                          const char *, \
                                                          view_ctn_t *));

int32_t set_item_menu_page(lv_obj_t *lobj, lv_obj_t *view, \
                           lv_obj_t *(* create_window_cb)(lv_obj_t *, \
                                                          const char *, \
                                                          view_ctn_t *));

static inline int32_t set_par_v_ctx(view_ctn_t *v_ctx, view_ctn_t *par_v_ctx)
{
    if (!v_ctx || !par_v_ctx)
        return -EINVAL;
    else
        v_ctx->par_v_ctx = par_v_ctx;
    return 0;
}
/*=====================
 * Getter functions
 *====================*/
static inline view_ctn_t *get_view_ctx(lv_obj_t *lobj) {
    return lobj ? (view_ctn_t *)get_internal_data(lobj) : NULL;
}

static inline menu_opt_t *get_opt_ctx(lv_obj_t *lobj) {
    return lobj ? (menu_opt_t *)get_internal_data(lobj) : NULL;
}

static inline lv_obj_t *get_view_container(view_ctn_t *v_ctx) {
    return v_ctx ? (lv_obj_t *)v_ctx->container : NULL;
}

static inline lv_obj_t *get_view(view_ctn_t *v_ctx) {
    return v_ctx ? (lv_obj_t *)v_ctx->view : NULL;
}

static inline lv_obj_t *get_menu(view_ctn_t *v_ctx) {
    return v_ctx ? (lv_obj_t *)((win_ctn_t )v_ctx->l_ctn).menu : NULL;
}

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
int32_t create_basic_menu_option(lv_obj_t *item, \
                                 const lv_font_t *sym_font, \
                                 const char *sym_index, \
                                 const lv_font_t *title_font, \
                                 const char *title);
lv_obj_t *create_menu_option(lv_obj_t *par, \
                             const lv_font_t *sym_font, const char *sym_index, \
                             const lv_font_t *title_font, const char *title);


view_ctn_t *create_menu_view(lv_obj_t *par, const char *name, \
                              bool ctrl, bool split);

static inline bool is_overlay_on_parent(lv_obj_t *par, view_ctn_t *par_v_ctx)
{
    if (!par || !par_v_ctx)
        return false;

    /* Check if the given parent object matches parent's left container */
    if (par == par_v_ctx->l_ctn.container) {
        LOG_DEBUG("[%s] Overlay menu detected on parent [%s]", \
                 get_name(par), get_name(par_v_ctx->l_ctn.container));
        return true;
    }

    return false;
}

view_ctn_t *create_common_menu_view(lv_obj_t *par, const char *name, \
                                    view_ctn_t *par_v_ctx, bool split);
/**********************
 *      MACROS
 **********************/

#endif /* G_MENU_H */
