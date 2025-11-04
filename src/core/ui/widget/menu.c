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
    item_ctx_t *item_ctx;
    menu_ctx_t *menu_ctx;

    lv_obj_set_style_text_color(lobj, lv_color_hex(0x000000), 0);

    item_ctx = lobj ? (item_ctx_t *)get_internal_data(lobj) : NULL;
    if (!item_ctx)
        return;

    menu_ctx = item_ctx->menu ? \
               (menu_ctx_t *)get_internal_data(item_ctx->menu) : NULL;
    if (!menu_ctx)
        return;

    if (menu_ctx->detail_pane) {
        remove_obj_and_child(get_meta(menu_ctx->detail_pane)->id, \
                             &get_meta(item_ctx->menu)->child);
        menu_ctx->detail_pane = NULL;
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
lv_obj_t *create_menu_view_control(lv_obj_t *menu, lv_obj_t *par, \
                                   const char *name, \
                                   bool back_btn_ena, \
                                   bool more_btn_ena)
{
    lv_obj_t *lobj;
    lv_obj_t *back_btn;
    lv_obj_t *more_btn;
    item_ctx_t *item_ctx;

    if (!par)
        return NULL;

    lobj = create_horizontal_flex_group(par, name);
    if (!lobj)
        return NULL;

    item_ctx = calloc(1, sizeof(*item_ctx));
    if (!item_ctx)
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
        set_internal_data(back_btn, item_ctx);
        item_ctx->menu = menu;
    }

    /* more button */
    more_btn = create_text_box(lobj, NULL, &lv_font_montserrat_24, \
                               more_btn_ena ? "..." : " ");
    lv_obj_set_style_text_color(more_btn, lv_color_hex(0x0000ff), 0);
    if (more_btn_ena)
        lv_obj_add_flag(more_btn, LV_OBJ_FLAG_CLICKABLE);

    return lobj;
}

static int32_t load_menu_item_page(lv_obj_t *lobj)
{
    lv_obj_t *menu;
    menu_ctx_t *menu_ctx;
    item_ctx_t *item_ctx;
    int32_t ret = 0;

    /* Validate argument and extract item context */
    item_ctx = lobj ? (item_ctx_t *)get_internal_data(lobj) : NULL;
    if (!item_ctx)
        return -EINVAL;

    menu = item_ctx->menu;
    if (!menu)
        return -EIO;

    menu_ctx = (menu_ctx_t *)get_internal_data(menu);
    if (!menu_ctx)
        return -EIO;

    if (menu_ctx->selected_opt != lobj) {
        /* Highlight before load the associated page */
        lv_obj_set_style_bg_color(lobj, lv_color_hex(0xFF6633), 0);

        /* Restore normal color and load the associated page */
        if (menu_ctx->selected_opt) {
            lv_obj_set_style_bg_color(menu_ctx->selected_opt, \
                                      lv_color_hex(bg_color(1)), 0);
        }
    }

    /* Remove current active page if switching to another item */
    if (menu_ctx->selected_opt != lobj && menu_ctx->detail_pane) {
        remove_obj_and_child(get_meta(menu_ctx->detail_pane)->id, \
                             &get_meta(menu)->child);
        menu_ctx->detail_pane = NULL;
    }

    /* Create or activate the new menu page */
    ret = set_active_menu_page(menu, item_ctx->create_detail_pane_cb);
    if (ret)
        return ret;

    menu_ctx->selected_opt = lobj;

    /* Recalculate layout after page activation */
    return refresh_object_tree_layout(menu_ctx->detail_pane);
}

static void menu_item_event_handler(lv_event_t *e)
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
        LOG_TRACE("Menu item [%s] clicked", get_name(lobj));
        ret = load_menu_item_page(lobj);
        if (ret) {
            LOG_ERROR("Menu item [%s] load page failed (%d)", \
                      get_name(lobj), ret);
        }
        break;

    default:
        break;
    }
}

static int32_t load_active_menu_page(lv_obj_t *menu)
{
    menu_ctx_t *menu_ctx;
    lv_obj_t *parent;
    lv_obj_t *page;
    char name_buf[64];

    menu_ctx = menu ? (menu_ctx_t *)get_internal_data(menu) : NULL;
    if (!menu_ctx)
        return -EINVAL;

    if (!menu_ctx->create_detail_pane_cb)
        return -EIO;

    /* Select valid parent container */
    if (menu_ctx->split_view) {
        parent = menu_ctx->r_container ? menu_ctx->r_container : \
            menu_ctx->l_container;
        if (!lv_obj_is_valid(parent))
            return -EIO;
    } else {
        parent = menu_ctx->l_container;
        if (!lv_obj_is_valid(parent))
            return -EIO;
    }

    /* Build page name */
    snprintf(name_buf, sizeof(name_buf), "%s.PAGE", get_name(parent));

    /* Create page via callback */
    page = menu_ctx->create_detail_pane_cb(menu, parent, name_buf);
    if (!page)
        return -EIO;

    menu_ctx->detail_pane = page;

    return 0;
}
static int32_t create_page_ctn(lv_obj_t *menu)
{
    lv_obj_t *r_container;
    menu_ctx_t *menu_ctx;
    int32_t scr_rot;
    char name_buf[64];

    menu_ctx = menu ? (menu_ctx_t *)get_internal_data(menu) : NULL;
    if (!menu_ctx)
        return -EINVAL;

    /* Build unique name for page container */
    snprintf(name_buf, sizeof(name_buf), "%s.R_CONTAINTER", get_name(menu));

    /* Create default page container (right-side column) */
    r_container = create_box(menu, name_buf);
    if (!r_container)
        return -EIO;

    /* Adjust grid alignment depending on current screen rotation */
    scr_rot = get_scr_rotation();
    if (scr_rot == ROTATION_0) {
        set_grid_cell_align(r_container,
                            LV_GRID_ALIGN_STRETCH, 1, 1,
                            LV_GRID_ALIGN_STRETCH, 0, 1);
        /* Default object rotation is ROTATION_0 */
    } else {
        set_grid_cell_align(r_container,
                            LV_GRID_ALIGN_STRETCH, 0, 1,
                            LV_GRID_ALIGN_STRETCH, 0, 1);
        /*
         * If created while the screen is rotated to 180,
         * fix rotation to correct the next transition.
         */
        get_meta(r_container)->data.rotation = ROTATION_180;
    }

    /*
     * The page container holds detailed content or configuration
     * of the currently active menu item (interactive region).
     */
    menu_ctx->r_container = r_container;

    lv_obj_set_style_bg_color(r_container, lv_color_hex(0xFF0000), 0);

    return 0;
}

int32_t show_and_hide_page_ctn_cb(lv_obj_t *menu)
{
    menu_ctx_t *menu_ctx;
    int32_t scr_rot;
    int32_t ret = 0;
    bool is_vertical;

    menu_ctx = menu ? (menu_ctx_t *)get_internal_data(menu) : NULL;
    if (!menu_ctx)
        return -EINVAL;

    scr_rot = get_scr_rotation();
    is_vertical = (scr_rot == ROTATION_90 || scr_rot == ROTATION_270);

    if (is_vertical) {
        /* Hide page container in vertical rotation */
        if (menu_ctx->detail_visible) {
            ret = remove_grid_layout_last_row_dsc(menu);
            if (ret) {
                LOG_ERROR("Remove layout failed (%d)", ret);
                return ret;
            }

            menu_ctx->detail_visible = false;
            menu_ctx->r_container = NULL;
            menu_ctx->detail_pane = NULL;
        }
    } else {
        /* Show page container in horizontal rotation */
        if (!menu_ctx->detail_visible) {
            ret = add_grid_layout_col_dsc(menu, LV_GRID_FR(65));
            if (ret) {
                LOG_ERROR("Add layout failed (%d)", ret);
                return ret;
            }

            menu_ctx->detail_visible = true;

            /*
             * Active page may exist if user created it while in vertical mode.
             * Remove it to avoid incorrect parent linkage.
             */
            if (menu_ctx->detail_pane) {
                remove_obj_and_child(get_meta(menu_ctx->detail_pane)->id, \
                                     &get_meta(menu)->child);
                menu_ctx->detail_pane = NULL;
            }
        }
    }

    /* Apply updated grid layout configuration */
    apply_grid_layout_config(menu);

    /* Create or reload page container when visible but not yet initialized */
    if (menu_ctx->detail_visible && !menu_ctx->r_container) {
        ret = create_page_ctn(menu);
        if (ret)
            return ret;

        ret = load_active_menu_page(menu);
        if (ret)
            return ret;

        refresh_object_tree_layout(menu_ctx->detail_pane);
    }

    return 0;
}

static int32_t create_views(lv_obj_t *menu)
{
    menu_ctx_t *menu_ctx;
    lv_obj_t *l_container;
    char name_buf[100];
    int32_t ret;

    menu_ctx = menu ? get_internal_data(menu) : NULL;
    if (!menu_ctx)
        return -EINVAL;

    sprintf(name_buf, "%s.L_CONTAINER", get_name(menu));

    /* Create left column for menu bar */
    l_container = create_box(menu, name_buf);
    if (!l_container)
        return -EINVAL;

    set_grid_cell_align(l_container, \
                        LV_GRID_ALIGN_STRETCH, 0, 1, \
                        LV_GRID_ALIGN_STRETCH, 0, 1);

    /*
     * The menu container holds the menu bar items defined by user.
     * Each item can trigger a corresponding page via callback.
     */
    menu_ctx->l_container = l_container;

    if (menu_ctx->split_view) {
        /* Create right-side page container */
        ret = create_page_ctn(menu);
        if (ret)
            return ret;
    }

    return 0;
}

static int32_t initial_views(lv_obj_t *menu)
{
    menu_ctx_t *menu_ctx;
    int32_t ret;

    menu_ctx = menu ? get_internal_data(menu) : NULL;
    if (!menu_ctx)
        return -EINVAL;

    ret = create_views(menu);
    if (ret) {
        LOG_WARN("Layout [%s] create holder failed, ret %d", \
                 get_name(menu), ret);
        return ret;
    }

    /*
     * The menu layout and page container have been created.
     * User can now define menu items and their corresponding pages.
     */
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
    lv_obj_t *item, *first_child;
    item_ctx_t *item_ctx;
    char name_buf[100];
    int32_t ret;

    if (!par || !name)
        return NULL;

    snprintf(name_buf, sizeof(name_buf), "%s.%s", get_name(par), name);

    item = create_horizontal_flex_group(par, name_buf);
    if (!item)
        return NULL;

    set_size(item, LV_PCT(100), 50);
    ret = set_padding(item, 0, 0, 20, 20);
    if (ret)
        LOG_WARN("Page [%s] set padding failed (%d)", get_name(item), ret);

    lv_obj_set_style_bg_color(item, lv_color_hex(bg_color(1)), 0);
    lv_obj_add_event_cb(item, menu_item_event_handler, LV_EVENT_ALL, NULL);

    /* Add border for non-first child */
    first_child = lv_obj_get_child(par, 0);
    if (first_child && first_child != item) {
        set_border_side(item, LV_BORDER_SIDE_TOP);
        lv_obj_set_style_border_width(item, 2, 0);
        lv_obj_set_style_border_color(item, lv_color_black(), 0);
    }

    item_ctx = calloc(1, sizeof(*item_ctx));
    if (!item_ctx)
        goto err_create;

    set_internal_data(item, item_ctx);
    return item;

err_create:
    remove_obj_and_child(get_meta(item)->id, &get_meta(par)->child);
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

int32_t set_item_menu_page(lv_obj_t *lobj, lv_obj_t *menu, \
                           lv_obj_t *(* create_detail_pane_cb)(lv_obj_t *, \
                                                        lv_obj_t *, \
                                                        const char *))
{
    item_ctx_t *item_ctx;
    int32_t ret = 0;

    /* Validate argument and extract item context */
    item_ctx = lobj ? (item_ctx_t *)get_internal_data(lobj) : NULL;
    if (!item_ctx)
        return -EINVAL;

    item_ctx->menu = menu;
    item_ctx->create_detail_pane_cb = create_detail_pane_cb;

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
lv_obj_t *create_menu_bar(lv_obj_t *menu)
{
    menu_ctx_t *menu_ctx;
    lv_obj_t *menu_bar;
    char name_buf[100];
    int32_t ret;

    menu_ctx = menu ? get_internal_data(menu) : NULL;
    if (!menu_ctx)
        return NULL;

    sprintf(name_buf, "%s.%s", get_name(menu_ctx->l_container), "MENU_BAR");

    menu_bar = create_vscroll_flex_group(menu_ctx->l_container, \
                                                   name_buf);
    if (!menu_bar)
        return NULL;

    /* Style and layout configuration */
    set_size(menu_bar, LV_PCT(100), LV_PCT(100));
    set_align(menu_bar, menu_ctx->l_container, LV_ALIGN_CENTER, 0, 0);

    ret = set_padding(menu_bar, 0, 0, 0, 0);
    if (ret)
        LOG_WARN("Page [%s] set padding failed (%d)", get_name(menu_bar), ret);

    lv_obj_set_style_bg_color(menu_bar, lv_color_hex(bg_color(10)), 0);

    return menu_bar;
}

/*
 * Create a menu page under the given parent.
 * Each page uses a flex column layout with a control bar on top.
 */
lv_obj_t *create_menu_page(lv_obj_t *menu, lv_obj_t *par, const char *name)
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
    control = create_menu_view_control(menu, page, name_buf, true, false);
    if (!control)
        return NULL;

    return page;
}

int32_t set_active_menu_page(lv_obj_t *menu, \
                             lv_obj_t *(*create_detail_pane_cb)(lv_obj_t *, \
                                                         lv_obj_t *, \
                                                         const char *))
{
    menu_ctx_t *menu_ctx;

    menu_ctx = menu ? get_internal_data(menu) : NULL;
    if (!menu_ctx)
        return -EINVAL;

    /* Skip if same page callback is already active */
    if (menu_ctx->detail_pane && \
        menu_ctx->create_detail_pane_cb == create_detail_pane_cb) {
        LOG_TRACE("Menu page [%s] is already active -> ignore", \
                  get_name(menu_ctx->detail_pane));
        return 0;
    }

    menu_ctx->create_detail_pane_cb = create_detail_pane_cb;

    return load_active_menu_page(menu);
}

lv_obj_t *create_menu_view(lv_obj_t *par, const char *name, bool split_view)
{
    menu_ctx_t *menu_ctx;
    lv_obj_t *lobj;
    int32_t ret;

    if (!par)
        return NULL;

    menu_ctx = (menu_ctx_t *)calloc(1, sizeof(menu_ctx_t));
    if (!menu_ctx)
        return NULL;

    menu_ctx->split_view = split_view;
    if (split_view)
        menu_ctx->detail_visible = true;
    else
        menu_ctx->detail_visible = false;

    /* Create main container using grid layout */
    lobj = create_grid_layout_object(par, name);
    if (!lobj)
        return NULL;

    ret = add_grid_row_col(lobj, LV_GRID_FR(98), LV_GRID_FR(35), \
                           LV_GRID_FR(65), split_view);
    if (ret)
        goto out_dsc;

    apply_grid_layout_config(lobj);
    set_grid_layout_align(lobj, \
                          LV_GRID_ALIGN_SPACE_BETWEEN, \
                          LV_GRID_ALIGN_SPACE_BETWEEN);

    /* Base style setup */
    lv_obj_set_style_bg_color(lobj, lv_color_hex(bg_color(10)), 0);
    lv_obj_set_style_radius(lobj, 16, 0);
    set_column_padding(lobj, 8);

    ret = set_padding(lobj, 8, 8, 8, 8);
    if (ret)
        LOG_WARN("Layout [%s] set padding failed, ret %d", \
                 get_name(lobj), ret);

    /* Context and layout rotation callback */
    set_internal_data(lobj, menu_ctx);

    if (split_view) {
        get_meta(lobj)->data.post_children_rotate_cb = \
                                                    show_and_hide_page_ctn_cb;
    }

    /* Initialize sub-views */
    ret = initial_views(lobj);
    if (ret)
        LOG_WARN("Menu [%s] view initialization failed, ret %d", \
                 get_name(lobj), ret);

    return lobj;

out_dsc:
    LOG_ERROR("Layout [%s] add row/column descriptor failed, ret %d", \
              get_name(lobj), ret);
    return NULL;
}

/*
 * Sub menu will be created on the right side of menu bar in split view mode.
 * In single view mode, the sub menu will be created on top of the menu bar,
 * sharing the same parent container (l_container).
 */
lv_obj_t *create_sub_menu_view(lv_obj_t *menu, lv_obj_t *par, \
                               const char *name, \
                               lv_obj_t *(*sub_menu_creator)(lv_obj_t *, \
                                                             const char *, \
                                                             bool))
{
    lv_obj_t *view, *control, *sub_menu;
    menu_view_t *menu_ctx;
    char name_buf[64];
    int32_t ret;
    int32_t scr_rot;

    if (!par)
        return NULL;

    view = create_grid_layout_object(par, name);
    if (!view)
        return NULL;

    set_align(view, par, LV_ALIGN_CENTER, 0, 0);
    set_size(view, LV_PCT(100), LV_PCT(100));

    scr_rot = get_scr_rotation();
    if (scr_rot == ROTATION_180 || scr_rot == ROTATION_270) {
        if (add_grid_layout_row_dsc(view, LV_GRID_FR(98)) ||
            add_grid_layout_row_dsc(view, 50) ||
            add_grid_layout_col_dsc(view, LV_GRID_FR(98)))
            goto err_view;
    } else {
        if (add_grid_layout_row_dsc(view, 50) ||
            add_grid_layout_row_dsc(view, LV_GRID_FR(98)) ||
            add_grid_layout_col_dsc(view, LV_GRID_FR(98)))
            goto err_view;
    }

    apply_grid_layout_config(view);
    set_grid_layout_align(view, LV_GRID_ALIGN_SPACE_BETWEEN, \
                          LV_GRID_ALIGN_SPACE_BETWEEN);

    snprintf(name_buf, sizeof(name_buf), "%s.CONTROL", name);
    control = create_menu_view_control(menu, view, name_buf, true, false);
    if (!control)
        goto err_view;

    lv_obj_set_style_bg_color(control, lv_color_hex(bg_color(70)), 0);
    set_grid_cell_align(control, LV_GRID_ALIGN_STRETCH, 0, 1, \
                        LV_GRID_ALIGN_STRETCH, 0, 1);

    snprintf(name_buf, sizeof(name_buf), "%s.HOLDER", name);
    sub_menu = sub_menu_creator(view, name_buf, false);
    if (!sub_menu)
        goto err_view;

    lv_obj_set_style_bg_color(sub_menu, lv_color_hex(bg_color(100)), 0);
    set_grid_cell_align(sub_menu, LV_GRID_ALIGN_STRETCH, 0, 1, \
                        LV_GRID_ALIGN_STRETCH, 1, 1);

    ret = set_padding(sub_menu, 0, 0, 0, 0);
    if (ret)
        LOG_WARN("view [%s] set padding failed (%d)", get_name(sub_menu), ret);

    lv_obj_set_style_bg_color(sub_menu, lv_color_hex(bg_color(10)), 0);

    menu_ctx = calloc(1, sizeof(*menu_ctx));
    if (!menu_ctx)
        goto err_view;

    menu_ctx->menu = menu;
    menu_ctx->sub_menu = sub_menu;
    set_internal_data(view, menu_ctx);

    return view;

err_view:
    remove_obj_and_child(get_meta(view)->id, &get_meta(par)->child);
    return NULL;
}
