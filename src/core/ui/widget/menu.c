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
#include "ui/widget/menu.h"

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
static inline void back_btn_pressed(lv_obj_t *lobj)
{
    lv_obj_set_style_text_color(lobj, lv_color_hex(0xFF6633), 0);
}

static inline int32_t back_btn_released(lv_obj_t *lobj)
{
    view_ctn_t *v_ctx;
    int32_t ret = 0;

    if (!lobj)
        return -EINVAL;

    v_ctx = get_view_ctx(lv_obj_get_parent(lobj));
    if (!v_ctx)
        return -EIO;

    lv_obj_set_style_text_color(lobj, lv_color_hex(0x000000), 0);

    if (!v_ctx->view)
        LOG_DEBUG("[unknown] Window is cleaning (null context)");
    else
        LOG_DEBUG("[%s] Window is cleaning", get_name(v_ctx->container));

    if (v_ctx->container) {
        ret = remove_obj_and_child(get_meta(v_ctx->container)->id, \
                                   &get_meta(lv_obj_get_parent( \
                                             v_ctx->container))->child);
        if (ret < 0) {
            LOG_WARN("Container not found, cannot clean child object");
            return -EIO;
        }
    } else {
        LOG_WARN("No container found in view context");
        return -EIO;
    }

    if (!v_ctx->view)
        LOG_DEBUG("[unknown] Window cleaned (null context)");
    else
        LOG_DEBUG("[%s] Window cleaned", get_name(v_ctx->view));

    return 0;
}

static inline void back_btn_clicked(lv_obj_t *lobj)
{
    LOG_DEBUG("Back button [%s] clicked", get_name(lobj));
}

static void back_btn_handler(lv_event_t *e)
{
    lv_event_code_t code;
    lv_obj_t *lobj;
    int32_t ret;

    code = lv_event_get_code(e);
    lobj = lv_event_get_target(e);

    switch (code) {
    case LV_EVENT_PRESSED:
        back_btn_pressed(lobj);
        break;

    case LV_EVENT_RELEASED:
        ret = back_btn_released(lobj);
        if (ret) {
            LOG_ERROR("Back button release handler failed, ret %d", ret);
        }
        break;

    case LV_EVENT_CLICKED:
        back_btn_clicked(lobj);
        break;

    default:
        break;
    }

    LOG_TRACE("Back button handle event [%d], return [%d]", code, ret);
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

static int32_t load_window_by_option(lv_obj_t *opt)
{
    lv_obj_t *view;
    view_ctn_t *view_ctx;
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

    /*
     * Highlight the newly selected option and restore the
     * color of the previously selected one.
     */
    if (view_ctx->l_ctn.selected_opt != opt) {
        lv_obj_set_style_bg_color(opt, lv_color_hex(0xFF6633), 0);

        if (view_ctx->l_ctn.selected_opt)
            lv_obj_set_style_bg_color(view_ctx->l_ctn.selected_opt, \
                                      lv_color_hex(bg_color(1)), 0);
    }

    /*
     * Remove the current active right-side pane before loading a
     * new one. In some cases the child window might have already
     * destroyed its pane, so validate before removal.
     */
    if (view_ctx->l_ctn.selected_opt != opt) {
        if (lv_obj_is_valid(view_ctx->opened_ctn->overlay_menu)) {
            LOG_DEBUG("|--- Removing previous menu pane [%s] --->", \
                      get_name(view_ctx->opened_ctn->overlay_menu));

            remove_obj_and_child(get_meta(view_ctx->opened_ctn->overlay_menu)->id, \
                                 &get_meta(view)->child);
            view_ctx->opened_ctn->overlay_menu = NULL;

            LOG_DEBUG("<--- Previous menu pane removed |");
        } else {
            LOG_DEBUG("|---> Previous menu pane already removed <---|");
            view_ctx->opened_ctn->overlay_menu = NULL;
        }
    } else if (view_ctx->l_ctn.selected_opt == opt) {
        LOG_DEBUG("| !!! Selected option already exists !!! |");
    }

    /* Create or activate a new window for the selected option */
    ret = set_and_load_window(view, opt_ctx->create_window_cb);
    if (ret)
        return ret;

    view_ctx->l_ctn.selected_opt = opt;

    LOG_TRACE("[%s] Window loaded successfully", get_name(view));

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
        LOG_DEBUG("Option [%s] clicked", get_name(lobj));
        ret = load_window_by_option(lobj);
        if (ret) {
            LOG_ERROR("Load option [%s] failed (%d)", get_name(lobj), ret);
        }
        break;

    default:
        break;
    }
}

static int32_t load_window(lv_obj_t *view, bool split)
{
    view_ctn_t *view_ctx;
    lv_obj_t *parent;
    lv_obj_t *window;
    char name_buf[64];
    lv_obj_t *(*create_window_cb)(lv_obj_t *, const char *);

    if (!view)
        return -EINVAL;

    view_ctx = get_view_ctx(view);
    if (!view_ctx)
        return -EIO;

    create_window_cb = view_ctx->opened_ctn->create_window_cb;
    parent = view_ctx->opened_ctn->container;

    if (!create_window_cb || !lv_obj_is_valid(parent))
        return -EIO;

    snprintf(name_buf, sizeof(name_buf), "%s.WINDOW", get_name(parent));

    LOG_DEBUG("| +++ Creating window [%s] --->", name_buf);
    /* Create window via callback */
    window = create_window_cb(parent, name_buf);
    if (!window)
        return -EIO;

    view_ctx->opened_ctn->overlay_menu = window;
    LOG_DEBUG("<--- Created window [%s] |", \
              get_name(view_ctx->opened_ctn->overlay_menu));

    return refresh_object_tree_layout(window);
}

/*
 * Create a container (left or right) for a given view.
 * The left container holds the menu or control pane,
 * while the right container displays detailed content.
 */
static int32_t create_window_container(lv_obj_t *view, enum container_side side)
{
    view_ctn_t *view_ctx;
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
        view_ctx->l_ctn.container = container;

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

        view_ctx->r_ctn.container = container;

        // FIXME: temporary visual marker
        lv_obj_set_style_bg_color(container, lv_color_hex(0xFCCE03), 0);
    }

    return 0;
}

/*
 * Handle rotation change: vertical split view
 */
static int32_t handle_vertical_split_view(view_ctn_t *ctx)
{
    lv_obj_t *view = get_view(ctx);
    int32_t ret = 0;

    LOG_TRACE("Vertical: Split view");

    if (!ctx->r_ctn.visible) {
        LOG_TRACE("|| vertical --> vertical ||: nothing change");
        return 0;
    }

    LOG_TRACE("-- horizontal --> vertical ||: change");

    ret = remove_grid_layout_last_row_dsc(view);
    if (ret) {
        LOG_ERROR("Remove layout failed (%d)", ret);
        return ret;
    }

    ctx->r_ctn.visible = false;
    ctx->r_ctn.container = NULL;
    ctx->r_ctn.overlay_menu = NULL;

    ctx->l_ctn.create_window_cb = ctx->opened_ctn->create_window_cb;
    ctx->opened_ctn = &ctx->l_ctn;

    LOG_DEBUG("Rotation -> Remove side window");
    return 0;
}

/*
 * Handle rotation change: horizontal split view
 */
static int32_t handle_horizontal_split_view(view_ctn_t *ctx)
{
    lv_obj_t *view = get_view(ctx);
    int32_t ret = 0;

    LOG_TRACE("Horizontal: Split view");

    if (ctx->r_ctn.visible) {
        LOG_TRACE("-- horizontal --> horizontal --: nothing change");
        return 0;
    }

    LOG_TRACE("|| vertical --> horizontal --: change");

    ret = add_grid_layout_col_dsc(view, LV_GRID_FR(65));
    if (ret) {
        LOG_ERROR("Add layout failed (%d)", ret);
        return ret;
    }

    /* Remove overlay menu if created in vertical mode */
    if (ctx->opened_ctn->overlay_menu) {
        LOG_DEBUG("Remove overlay [%s]",
                  get_name(ctx->opened_ctn->overlay_menu));
        remove_obj_and_child(
            get_meta(ctx->opened_ctn->overlay_menu)->id,
            &get_meta(view)->child);
        ctx->opened_ctn->overlay_menu = NULL;
    }

    ctx->r_ctn.visible = true;
    ctx->r_ctn.create_window_cb = ctx->opened_ctn->create_window_cb;
    ctx->opened_ctn = &ctx->r_ctn;

    LOG_DEBUG("Rotation -> Add side window");
    return 0;
}

/*
 * Apply updated layout and reload if needed
 */
static int32_t apply_layout_and_reload(view_ctn_t *ctx)
{
    lv_obj_t *view = get_view(ctx);
    int32_t ret;

    apply_grid_layout_config(view);

    /* Create or reload right container when visible but missing */
    if (ctx->r_ctn.visible && !ctx->r_ctn.container) {
        ret = create_window_container(view, CONTAINER_RIGHT);
        if (ret)
            return ret;

        ret = load_window(view, true);
        if (ret)
            return ret;
    }

    return 0;
}

/*
 * Main rotation callback
 */
static int32_t view_angle_change_cb(lv_obj_t *view)
{
    view_ctn_t *ctx;
    int32_t scr_rot, ret = 0;
    bool vertical;

    if (!view)
        return -EINVAL;

    ctx = get_view_ctx(view);
    if (!ctx)
        return -EIO;

    scr_rot = get_scr_rotation();
    vertical = (scr_rot == ROTATION_90 || scr_rot == ROTATION_270);

    LOG_TRACE("Rotation detected: %s", vertical ? "Vertical" : "Horizontal");

    if (vertical) {
        if (ctx->cfg.split_view)
            ret = handle_vertical_split_view(ctx);
        else
            LOG_TRACE("Vertical: Single view (TODO)");
    } else {
        if (ctx->cfg.split_view)
            ret = handle_horizontal_split_view(ctx);
        else
            LOG_TRACE("Horizontal: Single view (TODO)");
    }

    if (ret)
        return ret;

    ret = apply_layout_and_reload(ctx);
    if (ret)
        return ret;

    /* TODO: Handle single view rotation when child menu is active */
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

static int32_t validate_opened_container(view_ctn_t *ctx, \
                                         lv_obj_t *(*cb)(lv_obj_t *, \
                                                         const char *))
{
    win_ctn_t *opened = ctx->opened_ctn;

    if (!opened || !lv_obj_is_valid(opened->container))
        return -EIO;

    /* Case: overlay menu is valid and same callback already active */
    if (lv_obj_is_valid(opened->overlay_menu)) {
        if (opened->create_window_cb == cb) {
            LOG_TRACE("Menu [%s] already active -> skip", \
                      get_name(opened->overlay_menu));
            return 1; /* same callback, no need to reload */
        }

        LOG_TRACE("[%s] Creating new pane", get_name(get_view(ctx)));
    }

    return 0;
}

static int32_t create_split_window(view_ctn_t *ctx, \
                                   lv_obj_t *(*cb)(lv_obj_t *, const char *))
{
    int32_t ret;
    win_ctn_t *opened = ctx->opened_ctn;

    LOG_TRACE("Split view: Create window in split-view mode");

    opened->create_window_cb = cb;
    ret = load_window(get_view(ctx), true);
    if (ret)
        opened->create_window_cb = NULL;

    return ret;
}

static int32_t create_single_window(view_ctn_t *ctx, \
                                    lv_obj_t *(*cb)(lv_obj_t *, const char *))
{
    int32_t ret;
    win_ctn_t *opened = ctx->opened_ctn;

    LOG_TRACE("Single view: Create window in single-view mode");

    opened->create_window_cb = cb;
    ret = load_window(get_view(ctx), false);
    if (ret)
        opened->create_window_cb = NULL;

    return ret;
}

static view_ctn_t *create_view_ctx(bool ctrl, bool split)
{
    view_ctn_t *v_ctx;

    v_ctx = calloc(1, sizeof(*v_ctx));
    if (!v_ctx) {
        LOG_ERROR("Create menu view context failed");
        return NULL;
    }

    v_ctx->cfg.ctrl = ctrl;
    v_ctx->cfg.split_view = split;
    v_ctx->r_ctn.visible = split;

    /*
     * When split-view mode is active, the left window shows
     * the main menu and the right window displays details.
     * opened_ctn points to the active window for new submenus.
     */
    if (split) {
        v_ctx->opened_ctn = &v_ctx->r_ctn;
        LOG_DEBUG("Split view: create child window on the right container");
    } else {
        v_ctx->opened_ctn = &v_ctx->l_ctn;
        LOG_DEBUG("Single view: create child window on the left container");
    }

    return v_ctx;
}

/*
 * Create a view container (window) which may include a control bar
 * depending on the parent configuration. The menu view will be
 * created as a child of this container.
 */
static lv_obj_t *create_view_container(view_ctn_t *v_ctx, \
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
static int32_t set_view_control_ctx(view_ctn_t *v_ctx, lv_obj_t *ctrl)
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
 * Create a horizontal menu control bar containing optional back and
 * more buttons. The control is hidden by default and only shown when
 * screen rotation requires it.
 */
static lv_obj_t *create_view_control(view_ctn_t *v_ctx, \
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
        lv_obj_add_event_cb(back_btn, back_btn_handler, \
                            LV_EVENT_ALL, NULL);
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

/*
 * Set the main view window object for this view context.
 * Handles grid alignment if the view lives inside a container.
 */
static int32_t set_view_window_ctx(view_ctn_t *v_ctx, lv_obj_t *view)
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

static lv_obj_t *create_view(view_ctn_t *v_ctx, lv_obj_t *par, const char *name)
{
    lv_obj_t *view;
    int32_t ret;

    if (!par)
        return NULL;

    /* Create main container using grid layout */
    view = create_grid_layout_object(par, name);
    if (!view)
        return NULL;

    /*
     * In certain situations, the menu view may be created inside a
     * regular object rather than a layout container. In this case,
     * it must align itself properly with its parent.
     */
    if (!v_ctx->cfg.ctrl) {
        set_size(view, LV_PCT(100), LV_PCT(100));
        set_align(view, par, LV_ALIGN_CENTER, 0, 0);
    }

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
        get_meta(view)->data.post_children_rotate_cb = view_angle_change_cb;

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

int32_t create_basic_menu_option(lv_obj_t *item, \
                                 const lv_font_t *sym_font, \
                                 const char *sym_index, \
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

lv_obj_t *create_menu_option(lv_obj_t *par, \
                             const lv_font_t *sym_font, const char *sym_index, \
                             const lv_font_t *title_font, const char *title)
{
    lv_obj_t *item;
    int32_t ret;

    item = create_option_cell(par, title);
    if (!item)
        return NULL;

    ret = create_basic_menu_option(item, \
                                   sym_font, sym_index, title_font, title);
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
    view_ctn_t *view_ctx;
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
lv_obj_t *create_menu(lv_obj_t *view)
{
    view_ctn_t *view_ctx;
    lv_obj_t *bar;
    char name_buf[100];
    int32_t ret;

    if (!view)
        return NULL;

    view_ctx = get_view_ctx(view);
    if (!view_ctx)
        return NULL;

    sprintf(name_buf, "%s.%s", get_name(view_ctx->l_ctn.container), "L_BAR");
    bar = create_vscroll_flex_group(view_ctx->l_ctn.container, name_buf);
    if (!bar)
        return NULL;

    /* Style and layout configuration */
    set_size(bar, LV_PCT(100), LV_PCT(100));
    set_align(bar, view_ctx->l_ctn.container, LV_ALIGN_CENTER, 0, 0);

    ret = set_padding(bar, 0, 0, 0, 0);
    if (ret)
        LOG_WARN("Page [%s] set padding failed (%d)", get_name(bar), ret);

    lv_obj_set_style_bg_color(bar, lv_color_hex(bg_color(10)), 0);
    view_ctx->l_ctn.menu = bar;

    return bar;
}

int32_t set_and_load_window(lv_obj_t *view, \
                            lv_obj_t *(*create_window_cb)(lv_obj_t *, \
                                                          const char *))
{
    view_ctn_t *ctx;
    int32_t ret;

    if (!view)
        return -EINVAL;

    ctx = get_view_ctx(view);
    if (!ctx)
        return -EIO;

    /* Validate current opened container */
    ret = validate_opened_container(ctx, create_window_cb);
    if (ret != 0) {
        if (ret > 0)
            return 0; /* skip reload if same callback already active */
        LOG_ERROR("[%s] Invalid container", get_name(view));
        return ret;
    }

    /* Dispatch to correct mode */
    if (ctx->cfg.split_view)
        ret = create_split_window(ctx, create_window_cb);
    else
        ret = create_single_window(ctx, create_window_cb);

    if (ret) {
        LOG_ERROR("[%s] Create menu view failed, ret %d", \
                  get_name(view), ret);
        return ret;
    }

    return 0;
}

/*
 * Sub menu will be created on the right side of menu bar in split view mode.
 * In single view mode, the sub menu will be created on top of the menu bar,
 * sharing the same parent container (l_ctn).
 */
view_ctn_t *create_menu_view(lv_obj_t *par, const char *name, \
                              bool ctrl, bool split)
{
    lv_obj_t *container, *control, *view;
    view_ctn_t *v_ctx;
    char name_buf[64];
    int32_t ret;
    int32_t scr_rot;

    if (!par)
        return NULL;

    v_ctx = create_view_ctx(ctrl, split);
    if (!v_ctx)
        return NULL;

    container = ctrl ? create_view_container(v_ctx, par, name) : par;
    if (!container)
        goto err_ctn;

    if (ctrl) {
        snprintf(name_buf, sizeof(name_buf), "%s.CONTROL", name);
        control = create_view_control(v_ctx, container, name_buf, true, true);
        if (!control)
            goto err_ctrl;

    }

    snprintf(name_buf, sizeof(name_buf), "%s.HOLDER", name);
    view = create_view(v_ctx, container, name_buf);
    if (!view)
        goto err_view;

    return v_ctx;

err_view:
    remove_obj_and_child(get_meta(control)->id, &get_meta(par)->child);
err_ctrl:
    remove_obj_and_child(get_meta(container)->id, &get_meta(par)->child);
err_ctn:
    free(v_ctx);
    return NULL;
}
