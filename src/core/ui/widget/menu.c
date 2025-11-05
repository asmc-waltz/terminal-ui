/**
 * @file menu.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL defined locally will override the global setting in this file"
#endif
#include "log.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <lvgl.h>
#include "ui/ui_core.h"
#include "ui/fonts.h"
#include "ui/windows.h"
#include "ui/flex.h"
#include "ui/grid.h"
#include "ui/widget.h"

/*
 * Each Right Window can host a child Menu Container, enabling nested menus.
 *
 * [View/Menu Container]
 * +---------------------------------------------------------------------------+
 * | Window Controller (not needed for root menu)                              |
 * |===========================================================================|
 * |  Left Window - Container      | Right Window -> Nested [Menu Container]   |
 * | +---------------------------+ | +---------------------------------------+ |
 * | | Menu Bar                  | | |           Window Controller           | |
 * | |  ------------             | | +---------------------------------------+ |
 * | |  ------------             | | |           Left / Right Window         | |
 * | |                           | | |               ...                     | |
 * | |                           | | |               (Recursive Structure)   | |
 * | +---------------------------+ | +---------------------------------------+ |
 * +===========================================================================+
 *
 */
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
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
static inline void handle_page_ctrl_pressed(lv_obj_t *lobj)
{
    lv_obj_set_style_text_color(lobj, lv_color_hex(0xFF6633), 0);
}

static inline void handle_page_ctrl_released(lv_obj_t *lobj)
{
    menu_opt_t *opt_ctx;
    menu_view_t *view_ctx;

    if (!lobj)
        return;

    opt_ctx = get_opt_ctx(lobj);
    if (!opt_ctx)
        return -EIO;

    view_ctx = opt_ctx->view_ctx;
    if (!view_ctx)
        return -EIO;

    lv_obj_set_style_text_color(lobj, lv_color_hex(0x000000), 0);

    if (view_ctx->r_win.visible) {
        // TODO: clarify context: sub menu or root menu
        remove_obj_and_child(get_meta(view_ctx->r_win.menu_pane)->id, \
                             &get_meta(view_ctx->view)->child);
        view_ctx->r_win.menu_pane = NULL;
        LOG_TRACE("Clean RIGHT window");
    } else {
        remove_obj_and_child(get_meta(view_ctx->l_win.menu_pane)->id, \
                             &get_meta(view_ctx->view)->child);
        view_ctx->l_win.menu_pane = NULL;
        LOG_TRACE("Clean LEFT window");
    }
}

static inline void handle_page_ctrl_clicked(lv_obj_t *lobj)
{
    LV_LOG_USER("Back [%s] clicked", get_name(lobj));
}

static void page_control_handler(lv_event_t *e)
{
    lv_event_code_t code;
    lv_obj_t *lobj;

    code = lv_event_get_code(e);
    lobj = lv_event_get_target(e);

    switch (code) {
    case LV_EVENT_PRESSED:
        handle_page_ctrl_pressed(lobj);
        break;

    case LV_EVENT_RELEASED:
        handle_page_ctrl_released(lobj);
        break;

    case LV_EVENT_CLICKED:
        handle_page_ctrl_clicked(lobj);
        break;

    default:
        break;
    }
}

static int32_t redraw_page_control(lv_obj_t *lobj)
{
    obj_meta_t *meta;
    int32_t scr_rot;

    meta = lobj ? get_meta(lobj) : NULL;
    if (!meta)
        return -EINVAL;

    scr_rot = get_scr_rotation();

    /* Show control bar only in horizontal orientations */
    if (scr_rot == ROTATION_90 || scr_rot == ROTATION_270)
        lv_obj_clear_flag(lobj, LV_OBJ_FLAG_HIDDEN);
    else
        // lv_obj_add_flag(lobj, LV_OBJ_FLAG_HIDDEN);

    return 0;
}

/*
 * Create a horizontal menu control bar containing optional back and
 * more buttons. The control is hidden by default and only shown when
 * screen rotation requires it.
 */
lv_obj_t *create_menu_view_control(lv_obj_t *view, lv_obj_t *par, \
                                   const char *name, \
                                   bool back_btn_ena, \
                                   bool more_btn_ena)
{
    lv_obj_t *lobj;
    lv_obj_t *back_btn;
    lv_obj_t *more_btn;
    menu_opt_t *opt_ctx;

    if (!par)
        return NULL;

    lobj = create_horizontal_flex_group(par, name);
    if (!lobj)
        return NULL;

    opt_ctx = calloc(1, sizeof(*opt_ctx));
    if (!opt_ctx)
        return NULL;

    /* base layout */
    set_padding(lobj, 10, 10, 10, 10);
    get_meta(lobj)->data.pre_rotate_cb = redraw_page_control;
    // lv_obj_add_flag(lobj, LV_OBJ_FLAG_HIDDEN);

    /* back button */
    back_btn = create_text_box(lobj, NULL, &lv_font_montserrat_24, \
                               back_btn_ena ? "< Back" : " ");
    lv_obj_set_style_text_color(back_btn, lv_color_hex(0x0000ff), 0);
    if (back_btn_ena) {
        lv_obj_add_flag(back_btn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(back_btn, page_control_handler, LV_EVENT_ALL, NULL);
        set_internal_data(back_btn, opt_ctx);
        opt_ctx->view_ctx = get_view_ctx(view);
    }

    /* more button */
    more_btn = create_text_box(lobj, NULL, &lv_font_montserrat_24, \
                               more_btn_ena ? "..." : " ");
    lv_obj_set_style_text_color(more_btn, lv_color_hex(0x0000ff), 0);
    if (more_btn_ena)
        lv_obj_add_flag(more_btn, LV_OBJ_FLAG_CLICKABLE);

    return lobj;
}

static int32_t load_menu_left_opt(lv_obj_t *opt)
{
    lv_obj_t *view;
    menu_view_t *view_ctx;
    menu_opt_t *opt_ctx;
    int32_t ret = 0;

    /* Validate argument and extract option context */
    if (!opt)
        return -EINVAL;

    opt_ctx = get_opt_ctx(opt);
    if (!opt_ctx)
        return -EIO;

    view_ctx = opt_ctx->view_ctx;
    if (!view_ctx)
        return -EIO;

    view = view_ctx->view;
    if (!view)
        return -EIO;

    if (view_ctx->l_win.selected_opt != opt) {
        /* Highlight before load the associated page */
        lv_obj_set_style_bg_color(opt, lv_color_hex(0xFF6633), 0);

        /* Restore normal color and load the associated page */
        if (view_ctx->l_win.selected_opt) {
            lv_obj_set_style_bg_color(view_ctx->l_win.selected_opt, \
                                      lv_color_hex(bg_color(1)), 0);
        }
    }

    /* Remove current active page if switching to another item */
    if (view_ctx->l_win.selected_opt != opt && view_ctx->r_win.menu_pane) {

        LOG_TRACE("Removed pane ?");
        LOG_TRACE("REMOVE ? [%s]", get_name(view_ctx->r_win.menu_pane));
        remove_obj_and_child(get_meta(view_ctx->r_win.menu_pane)->id, \
                             &get_meta(view)->child);
        view_ctx->r_win.menu_pane = NULL;
        LOG_TRACE("Removed the previous menu pane");
    } else {
        LOG_WARN("Option pressed while menu pane is created?");
    }

    /* Create or activate the new menu page */
    ret = set_active_window(view, opt_ctx->create_window_cb);
    if (ret)
        return ret;

    view_ctx->l_win.selected_opt = opt;

    /* Recalculate layout after page activation */
    // return refresh_object_tree_layout(menu_ctx->detail_pane);
    return 0;
}

static void menu_option_event_handler(lv_event_t *e)
{
    lv_event_code_t code;
    lv_obj_t *lobj;
    int32_t ret;

    code = lv_event_get_code(e);
    lobj = lv_event_get_target(e);

    switch (code) {
    case LV_EVENT_PRESSED:
        break;

    case LV_EVENT_RELEASED:
        break;

    case LV_EVENT_CLICKED:
        LOG_TRACE("Option [%s] clicked", get_name(lobj));
        ret = load_menu_left_opt(lobj);
        if (ret) {
            LOG_ERROR("Load option [%s] failed (%d)", get_name(lobj), ret);
        }
        break;

    default:
        break;
    }
}

static int32_t load_pane(lv_obj_t *view, bool split)
{
    menu_view_t *view_ctx;
    lv_obj_t *parent;
    lv_obj_t *page;
    char name_buf[64];
    lv_obj_t *(*create_window_cb)(lv_obj_t *, const char *);

    if (!view)
        return -EINVAL;

    view_ctx = get_view_ctx(view);
    if (!view_ctx)
        return -EIO;

    /* Select valid parent container */
    if (split) {
        create_window_cb = view_ctx->r_win.create_window_cb;
        parent = view_ctx->r_win.container;
    } else {
        create_window_cb = view_ctx->l_win.create_window_cb;
        parent = view_ctx->l_win.container;
    }

    if (!create_window_cb || !lv_obj_is_valid(parent))
        return -EIO;

    snprintf(name_buf, sizeof(name_buf), "%s.WINDOW", get_name(parent));

    /* Create page via callback */
    page = create_window_cb(parent, name_buf);
    if (!page)
        return -EIO;

    if (split) {
        view_ctx->r_win.menu_pane = page;
    } else {
        view_ctx->l_win.menu_pane = page;
    }

    return refresh_object_tree_layout(page);
}

/*
 * Create a container (left or right) for a given view.
 * The left container holds the menu or control pane,
 * while the right container displays detailed content.
 */
static int32_t create_window_container(lv_obj_t *view, enum container_side side)
{
    menu_view_t *view_ctx;
    lv_obj_t *container;
    char name_buf[64];
    int32_t scr_rot;

    if (!view)
        return -EINVAL;

    view_ctx = get_view_ctx(view);
    if (!view_ctx)
        return -EIO;

    snprintf(name_buf, sizeof(name_buf), "%s.%s_CONTAINER", get_name(view), \
             (side == CONTAINER_LEFT) ? "L" : "R");

    container = create_box(view, name_buf);
    if (!container)
        return -EIO;

    if (side == CONTAINER_LEFT) {
        /* Left container: menu bar or control list */
        set_grid_cell_align(container, \
                            LV_GRID_ALIGN_STRETCH, 0, 1, \
                            LV_GRID_ALIGN_STRETCH, 0, 1);
        view_ctx->l_win.container = container;

        // FIXME: temporary visual marker
        lv_obj_set_style_bg_color(container, lv_color_hex(0x4FC3F7), 0);

    } else {
        /* Right container: content/detail area */
        scr_rot = get_scr_rotation();
        if (scr_rot == ROTATION_0) {
            set_grid_cell_align(container, \
                                LV_GRID_ALIGN_STRETCH, 1, 1, \
                                LV_GRID_ALIGN_STRETCH, 0, 1);
        } else {
            set_grid_cell_align(container, \
                                LV_GRID_ALIGN_STRETCH, 0, 1, \
                                LV_GRID_ALIGN_STRETCH, 0, 1);
            get_meta(container)->data.rotation = ROTATION_180;
        }

        view_ctx->r_win.container = container;

        // FIXME: temporary visual marker
        lv_obj_set_style_bg_color(container, lv_color_hex(0xFCCE03), 0);
    }

    return 0;
}

int32_t view_change_cb(lv_obj_t *view)
{
    menu_view_t *view_ctx;
    int32_t scr_rot, ret = 0;
    bool is_vertical;

    if (!view)
        return -EINVAL;

    view_ctx = get_view_ctx(view);
    if (!view_ctx)
        return -EIO;

    scr_rot = get_scr_rotation();
    is_vertical = (scr_rot == ROTATION_90 || scr_rot == ROTATION_270);

    if (is_vertical && view_ctx->cfg.split_view) {
        /* Hide page container in vertical rotation */
        if (view_ctx->r_win.visible) {
            ret = remove_grid_layout_last_row_dsc(view);
            if (ret) {
                LOG_ERROR("Remove layout failed (%d)", ret);
                return ret;
            }

            view_ctx->r_win.visible = false;
            view_ctx->r_win.container = NULL;
            view_ctx->r_win.menu_pane = NULL;
        }
    } else {
        /* Show page container in horizontal rotation */
        if (!view_ctx->r_win.visible) {
            ret = add_grid_layout_col_dsc(view, LV_GRID_FR(65));
            if (ret) {
                LOG_ERROR("Add layout failed (%d)", ret);
                return ret;
            }

            view_ctx->r_win.visible = true;

            /*
             * Active page may exist if user created it while in vertical mode.
             * Remove it to avoid incorrect parent linkage.
             */
            if (view_ctx->r_win.menu_pane) {
                remove_obj_and_child(get_meta(view_ctx->r_win.menu_pane)->id, \
                                     &get_meta(view)->child);
                view_ctx->r_win.menu_pane = NULL;
            }
        }
    }

    /* Apply updated grid layout configuration */
    apply_grid_layout_config(view);

    /* Create or reload page container when visible but not yet initialized */
    if (view_ctx->r_win.visible && !view_ctx->r_win.container) {
        ret = create_window_container(view, CONTAINER_RIGHT);
        if (ret)
            return ret;

        ret = load_pane(view, true);
        if (ret)
            return ret;

        // refresh_object_tree_layout(view_ctx->r_win.menu_pane);
    }

    // TODO: handle the single view rotation when child menu is active

    return 0;
}

static inline int32_t add_grid_row_col(lv_obj_t *lobj, \
                                       lv_coord_t row, \
                                       lv_coord_t col1, \
                                       lv_coord_t col2, \
                                       bool split_view)
{
    int32_t ret;

    ret = add_grid_layout_row_dsc(lobj, row);
    if (ret)
        return ret;

    if (!split_view) {
        lv_coord_t col = row;
        return add_grid_layout_col_dsc(lobj, col);
    }

    ret = add_grid_layout_col_dsc(lobj, col1);
    if (ret)
        return ret;

    return add_grid_layout_col_dsc(lobj, col2);
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t *create_option_cell(lv_obj_t *par, const char *name)
{
    lv_obj_t *opt, *first_child;
    menu_opt_t *opt_ctx;
    char name_buf[100];
    int32_t ret;

    if (!par || !name)
        return NULL;

    snprintf(name_buf, sizeof(name_buf), "%s.%s", get_name(par), name);

    opt = create_horizontal_flex_group(par, name_buf);
    if (!opt)
        return NULL;

    set_size(opt, LV_PCT(100), 50);
    ret = set_padding(opt, 0, 0, 20, 20);
    if (ret)
        LOG_WARN("Page [%s] set padding failed (%d)", get_name(opt), ret);

    lv_obj_set_style_bg_color(opt, lv_color_hex(bg_color(1)), 0);
    lv_obj_add_event_cb(opt, menu_option_event_handler, LV_EVENT_ALL, NULL);

    /* Add border for non-first child */
    first_child = lv_obj_get_child(par, 0);
    if (first_child && first_child != opt) {
        set_border_side(opt, LV_BORDER_SIDE_TOP);
        lv_obj_set_style_border_width(opt, 2, 0);
        lv_obj_set_style_border_color(opt, lv_color_black(), 0);
    }

    opt_ctx = calloc(1, sizeof(*opt_ctx));
    if (!opt_ctx)
        goto err_create;

    set_internal_data(opt, opt_ctx);
    return opt;

err_create:
    remove_obj_and_child(get_meta(opt)->id, &get_meta(par)->child);
    return NULL;
}

int32_t create_basic_item(lv_obj_t *item, \
                          const lv_font_t *sym_font, const char *sym_index, \
                          const lv_font_t *title_font, const char *title)
{
    lv_obj_t *sym, *label;

    if (!item || !sym_index || !title)
        return -EINVAL;

    sym = create_symbol_box(item, NULL, \
                            sym_font ? sym_font : &terminal_icons_32, \
                            sym_index);
    if (!sym) {
        LOG_ERROR("Menu item [%s] create symbol failed", get_name(item));
        return -EIO;
    }

    label = create_text_box(item, NULL, \
                            title_font ? title_font : &lv_font_montserrat_24, \
                            title);
    if (!label) {
        LOG_ERROR("Menu item [%s] create label failed", get_name(item));
        return -EIO;
    }

    return 0;
}

lv_obj_t *create_menu_item(lv_obj_t *par, \
                           const lv_font_t *sym_font, const char *sym_index, \
                           const lv_font_t *title_font, const char *title)
{
    lv_obj_t *item;
    int32_t ret;

    item = create_option_cell(par, title);
    if (!item)
        return NULL;

    ret = create_basic_item(item, sym_font, sym_index, title_font, title);
    if (ret) {
        LOG_WARN("Menu item [%s] basic creation failed (%d)", \
                 get_name(item), ret);
        remove_obj_and_child(get_meta(item)->id, &get_meta(par)->child);
        return NULL;
    }

    return item;
}

int32_t set_item_menu_page(lv_obj_t *lobj, lv_obj_t *view, \
                           lv_obj_t *(* create_window_cb)(lv_obj_t *, \
                                                          const char *))
{
    menu_opt_t *opt_ctx;
    menu_view_t *view_ctx;
    int32_t ret = 0;

    if (!lobj || !view)
        return -EINVAL;

    view_ctx = get_view_ctx(view);
    if (!view_ctx)
        return -EIO;

    opt_ctx = get_opt_ctx(lobj);
    if (!opt_ctx)
        return -EIO;

    opt_ctx->view_ctx = view_ctx ;
    opt_ctx->create_window_cb = create_window_cb;

    return 0;
}
 
/*
 * Create a menu group container inside the menu bar.
 * This container uses a flex layout with vertical flow,
 * and its height depends on the number of child elements.
 */
lv_obj_t *create_menu_group(lv_obj_t *par, const char *name)
{
    lv_obj_t *group;
    char name_buf[100];

    if (!par)
        return NULL;

    sprintf(name_buf, "%s.%s", get_name(par), name);

    group = create_vertical_flex_group(par, name_buf);
    if (!group)
        return NULL;

    set_row_padding(group, 0);

    return group;
}

/*
 * Create the main menu bar — a left-side panel containing menu items.
 * The bar is arranged vertically using a flex layout, and may contain
 * one or more grouped containers.
 */
lv_obj_t *create_menu_bar(lv_obj_t *view)
{
    menu_view_t *view_ctx;
    lv_obj_t *bar;
    char name_buf[100];
    int32_t ret;

    if (!view)
        return NULL;

    view_ctx = get_view_ctx(view);
    if (!view_ctx)
        return NULL;

    sprintf(name_buf, "%s.%s", get_name(view_ctx->l_win.container), "L_BAR");
    bar = create_vscroll_flex_group(view_ctx->l_win.container, name_buf);
    if (!bar)
        return NULL;

    /* Style and layout configuration */
    set_size(bar, LV_PCT(100), LV_PCT(100));
    set_align(bar, view_ctx->l_win.container, LV_ALIGN_CENTER, 0, 0);

    ret = set_padding(bar, 0, 0, 0, 0);
    if (ret)
        LOG_WARN("Page [%s] set padding failed (%d)", get_name(bar), ret);

    lv_obj_set_style_bg_color(bar, lv_color_hex(bg_color(10)), 0);
    view_ctx->l_win.menu_pane = bar;

    return bar;
}

/*
 * Create a menu page under the given parent.
 * Each page uses a flex column layout with a control bar on top.
 */
lv_obj_t *create_menu_page(lv_obj_t *view, lv_obj_t *par, const char *name)
{
    lv_obj_t *page;
    lv_obj_t *control;
    char name_buf[64];
    int32_t ret;

    if (!par)
        return NULL;

    page = create_vscroll_flex_group(par, name);
    if (!page)
        return NULL;

    set_size(page, LV_PCT(100), LV_PCT(100));
    set_align(page, par, LV_ALIGN_CENTER, 0, 0);

    ret = set_padding(page, 0, 0, 0, 0);
    if (ret)
        LOG_WARN("Page [%s] set padding failed (%d)", get_name(page), ret);

    lv_obj_set_style_bg_color(page, lv_color_hex(bg_color(10)), 0);

    snprintf(name_buf, sizeof(name_buf), "%s.CONTROL", name);
    control = create_menu_view_control(view, page, name_buf, true, false);
    if (!control)
        return NULL;

    return page;
}

int32_t set_active_window(lv_obj_t *view, \
                          lv_obj_t *(*create_window_cb)(lv_obj_t *, \
                                                        const char *))
{
    menu_view_t *view_ctx;
    window_t *act_win = NULL;
    int32_t ret = -EIO;

    if (!view)
        return -EINVAL;

    view_ctx = get_view_ctx(view);
    if (!view_ctx)
        return -EIO;

    // TODO: combine
    if (view_ctx->cfg.split_view) {
        act_win = &view_ctx->r_win;
        if (lv_obj_is_valid(act_win->container) && \
            lv_obj_is_valid(act_win->menu_pane)) {
            if (act_win->create_window_cb == create_window_cb) {
                /* Skip if same page callback is already active */
                LOG_TRACE("Menu [%s] is already active -> ignore", \
                          get_name(act_win->menu_pane));
                return 0;
            } else {
                LOG_TRACE("[%s] view is creating new pane", get_name(view));
            }
        } else {
            if (!lv_obj_is_valid(act_win->container)) {
                LOG_ERROR("[%s] view container is not available", \
                          get_name(view));
                return -EIO;
            }
        }

        LOG_TRACE("Split view: Create window in split view mode");
        act_win->create_window_cb = create_window_cb;
        view_ctx->act_win = act_win;
        ret = load_pane(view, true);
        if (ret)
            act_win->create_window_cb = NULL;
    } else {
        // Single view
        act_win = &view_ctx->l_win;
        LOG_TRACE("Single view: Create window in single view mode");
        act_win->create_window_cb = create_window_cb;
        view_ctx->act_win = act_win;
        ret = load_pane(view, false);
        if (ret)
            act_win->create_window_cb = NULL;
    }

    if (ret) {
        LOG_ERROR("[%s] Create menu view failed, ret %d", get_name(view), ret);
        return ret;
    }

    return 0;
}

menu_view_t *create_view_ctx(bool root, bool split)
{
    menu_view_t *v_ctx;

    v_ctx = calloc(1, sizeof(*v_ctx));
    if (!v_ctx) {
        LOG_ERROR("Create menu view context failed");
        return NULL;
    }

    v_ctx->cfg.root = root;
    v_ctx->cfg.split_view = split;
    v_ctx->r_win.visible = split;

    /*
     * When split-view mode is active, the left window shows
     * the main menu and the right window displays details.
     * act_win points to the active window for new submenus.
     */
    if (split) {
        v_ctx->act_win = &v_ctx->r_win;
        LOG_TRACE("Split view: right window enabled by default");
    }

    return v_ctx;
}

/*
 * Create a view container (window) which may include a control bar
 * depending on the parent configuration. The menu view will be
 * created as a child of this container.
 */
lv_obj_t *create_view_container(menu_view_t *v_ctx, \
                                lv_obj_t *par, const char *name)
{
    lv_obj_t *container;
    int32_t scr_rot;
    int32_t ret;

    if (!par)
        return NULL;

    container = create_grid_layout_object(par, name);
    if (!container)
        return NULL;

    set_align(container, par, LV_ALIGN_CENTER, 0, 0);
    set_size(container, LV_PCT(100), LV_PCT(100));

    scr_rot = get_scr_rotation();
    if (scr_rot == ROTATION_180 || scr_rot == ROTATION_270) {
        ret = add_grid_layout_row_dsc(container, LV_GRID_FR(98)) ?: \
              add_grid_layout_row_dsc(container, 50) ?: \
              add_grid_layout_col_dsc(container, LV_GRID_FR(98));
    } else {
        ret = add_grid_layout_row_dsc(container, 50) ?: \
              add_grid_layout_row_dsc(container, LV_GRID_FR(98)) ?: \
              add_grid_layout_col_dsc(container, LV_GRID_FR(98));
    }
    if (ret)
        goto err_container;

    apply_grid_layout_config(container);
    set_grid_layout_align(container, \
                          LV_GRID_ALIGN_SPACE_BETWEEN, \
                          LV_GRID_ALIGN_SPACE_BETWEEN);

    v_ctx->container = container;
    set_internal_data(container, v_ctx);

    return container;

err_container:
    LOG_ERROR("Failed to create grid descriptor for view [%s]", name);
    remove_obj_and_child(get_meta(container)->id, &get_meta(par)->child);
    return NULL;
}


/*
 * Add control bar to active window of the given view context.
 * The control bar supports navigation or local actions.
 */
int32_t set_view_control_ctx(menu_view_t *v_ctx, lv_obj_t *ctrl)
{
    if (!v_ctx || !ctrl)
        return -EINVAL;

    if (!v_ctx->container) {
        LOG_WARN("View control must create inside of a view container");
        return -EIO;
    } else {
        set_grid_cell_align(ctrl, \
                            LV_GRID_ALIGN_STRETCH, 0, 1, \
                            LV_GRID_ALIGN_STRETCH, 0, 1);
    }

    v_ctx->view_ctrl = ctrl;
    set_internal_data(ctrl, v_ctx);

    return 0;
}

/*
 * Set the main view window object for this view context.
 * Handles grid alignment if the view lives inside a container.
 */
int32_t set_view_window_ctx(menu_view_t *v_ctx, lv_obj_t *view)
{
    if (!v_ctx || !view)
        return -EINVAL;

    if (v_ctx->container) {
        set_grid_cell_align(view, \
                            LV_GRID_ALIGN_STRETCH, 0, 1, \
                            LV_GRID_ALIGN_STRETCH, 1, 1);
    }

    v_ctx->view = view;
    set_internal_data(view, v_ctx);

    return 0;
}

/*
 * Create a horizontal menu control bar containing optional back and
 * more buttons. The control is hidden by default and only shown when
 * screen rotation requires it.
 */
lv_obj_t *create_view_control(menu_view_t *v_ctx, \
                              lv_obj_t *par, const char *name, \
                              bool back_ena, bool more_ena)
{
    lv_obj_t *ctrl;
    lv_obj_t *back_btn;
    lv_obj_t *more_btn;
    int32_t ret;

    if (!par)
        return NULL;

    ctrl = create_horizontal_flex_group(par, name);
    if (!ctrl)
        return NULL;

    /* Base layout */
    set_padding(ctrl, 10, 10, 10, 10);
    get_meta(ctrl)->data.pre_rotate_cb = redraw_page_control;
    // lv_obj_add_flag(ctrl, LV_OBJ_FLAG_HIDDEN);

    /* Back button */
    back_btn = create_text_box(ctrl, NULL, &lv_font_montserrat_24, \
                               back_ena ? "< Back" : " ");
    lv_obj_set_style_text_color(back_btn, lv_color_hex(0x0000ff), 0);
    if (back_ena) {
        lv_obj_add_flag(back_btn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(back_btn, page_control_handler, LV_EVENT_ALL, NULL);
    }

    /* More button */
    more_btn = create_text_box(ctrl, NULL, &lv_font_montserrat_24, \
                               more_ena ? "..." : " ");
    lv_obj_set_style_text_color(more_btn, lv_color_hex(0x0000ff), 0);
    if (more_ena)
        lv_obj_add_flag(more_btn, LV_OBJ_FLAG_CLICKABLE);

    ret = set_view_control_ctx(v_ctx, ctrl);
    if (ret) {
        LOG_ERROR("Set view control context failed, ret %d", ret);
        goto err;
    }

    // FIXME: temporary visual marker
    lv_obj_set_style_bg_color(ctrl, lv_color_hex(0xFCBA03), 0);
    return ctrl;

err:
    remove_obj_and_child(get_meta(ctrl)->id, &get_meta(par)->child);
    return NULL;
}

lv_obj_t *create_menu(menu_view_t *v_ctx, lv_obj_t *par, const char *name)
{
    lv_obj_t *view;
    int32_t ret;

    if (!par)
        return NULL;

    /* Create main container using grid layout */
    view = create_grid_layout_object(par, name);
    if (!view)
        return NULL;

    ret = add_grid_row_col(view, LV_GRID_FR(98), LV_GRID_FR(35), \
                           LV_GRID_FR(65), v_ctx->cfg.split_view);
    if (ret) {
        LOG_ERROR("Layout [%s] add grid descriptor failed (%d)", \
                  name, ret);
        goto err;
    }

    apply_grid_layout_config(view);
    set_grid_layout_align(view, \
                          LV_GRID_ALIGN_SPACE_BETWEEN, \
                          LV_GRID_ALIGN_SPACE_BETWEEN);

    /* Base style setup */
    lv_obj_set_style_bg_color(view, lv_color_hex(bg_color(10)), 0);
    lv_obj_set_style_radius(view, 16, 0);

    ret = set_column_padding(view, 8);
    if (ret)
        LOG_WARN("Layout [%s] set column padding failed (%d)", \
                 name, ret);

    ret = set_padding(view, 8, 8, 8, 8);
    if (ret)
        LOG_WARN("Layout [%s] set padding failed (%d)", \
                 name, ret);

    ret = set_view_window_ctx(v_ctx, view);
    if (ret)
        goto err;

    ret = create_window_container(view, CONTAINER_LEFT);
    if (ret) {
        LOG_WARN("[%s] create left container failed, ret %d", \
                 get_name(view), ret);
        goto err;
    }

    if (v_ctx->cfg.split_view) {
        /* Register rotation callback if split view */
        get_meta(view)->data.post_children_rotate_cb = view_change_cb;

        ret = create_window_container(view, CONTAINER_RIGHT);
        if (ret) {
            LOG_WARN("[%s] create right container failed, ret %d", \
                    get_name(view), ret);
            goto err;
        }
    }

    // FIXME: temporary visual marker
    lv_obj_set_style_bg_color(view, lv_color_hex(bg_color(100)), 0);
    return view;

err:
    remove_obj_and_child(get_meta(view)->id, &get_meta(par)->child);
    return NULL;
}

/*
 * Sub menu will be created on the right side of menu bar in split view mode.
 * In single view mode, the sub menu will be created on top of the menu bar,
 * sharing the same parent container (l_container).
 */
menu_view_t *create_menu_view(lv_obj_t *par, const char *name, bool root)
{
    lv_obj_t *container, *control, *view;
    menu_view_t *v_ctx;
    char name_buf[64];
    int32_t ret;
    int32_t scr_rot;

    if (!par)
        return NULL;

    v_ctx = create_view_ctx(root, root ? true : false);
    if (!v_ctx)
        return NULL;

    container = root ? par : create_view_container(v_ctx, par, name);
    if (!container)
        goto err_ctn;

    if (!root) {
        snprintf(name_buf, sizeof(name_buf), "%s.CONTROL", name);
        control = create_view_control(v_ctx, container, name_buf, true, true);
        if (!control)
            goto err_ctrl;

    }

    snprintf(name_buf, sizeof(name_buf), "%s.HOLDER", name);
    view = create_menu(v_ctx, container, name_buf);
    if (!view)
        goto err_menu;

    return v_ctx;

err_menu:
    remove_obj_and_child(get_meta(control)->id, &get_meta(par)->child);
err_ctrl:
    remove_obj_and_child(get_meta(container)->id, &get_meta(par)->child);
err_ctn:
    free(v_ctx);
    return NULL;
}
