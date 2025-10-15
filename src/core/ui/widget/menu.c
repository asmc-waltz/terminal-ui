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
typedef struct {
    lv_obj_t *menu_ctn;
    lv_obj_t *act_menu_item;
    lv_obj_t *page_ctn;
    lv_obj_t *act_page;
    lv_obj_t *(*create_page_cb)(lv_obj_t *, const char *);
    bool page_visible;
} menu_ctx_t;

typedef struct {
    lv_obj_t *menu;
    lv_obj_t *(*create_page_cb)(lv_obj_t *, const char *);
} item_ctx_t;

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

    /* Remove current active page if switching to another item */
    if (menu_ctx->act_menu_item != lobj && menu_ctx->act_page) {
        remove_obj_and_child(get_meta(menu_ctx->act_page)->id, \
                             &get_meta(menu)->child);
        menu_ctx->act_page = NULL;
    }

    /* Create or activate the new menu page */
    ret = set_active_menu_page(menu, item_ctx->create_page_cb);
    if (ret)
        return ret;

    menu_ctx->act_menu_item = lobj;

    /* Recalculate layout after page activation */
    return refresh_object_tree_layout(menu_ctx->act_page);
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
        /* Highlight when pressed */
        lv_obj_set_style_bg_color(lobj, lv_color_hex(0xFF6633), 0);
        break;

    case LV_EVENT_RELEASED:
        /* Restore normal color and load the associated page */
        lv_obj_set_style_bg_color(lobj, lv_color_hex(bg_color(1)), 0);

        ret = load_menu_item_page(lobj);
        if (ret) {
            LOG_ERROR("Menu item [%s] load page failed (%d)", \
                      get_name(lobj), ret);
        }
        break;

    case LV_EVENT_CLICKED:
        LV_LOG_USER("Menu item [%s] clicked", get_name(lobj));
        break;

    default:
        break;
    }
}

static int32_t load_active_menu_page(lv_obj_t *lobj)
{
    menu_ctx_t *menu_ctx;
    lv_obj_t *parent;
    lv_obj_t *page;
    char name_buf[64];

    menu_ctx = lobj ? (menu_ctx_t *)get_internal_data(lobj) : NULL;
    if (!menu_ctx)
        return -EINVAL;

    if (!menu_ctx->create_page_cb)
        return -EIO;

    /* Select valid parent container */
    parent = menu_ctx->page_ctn ? menu_ctx->page_ctn : menu_ctx->menu_ctn;
    if (!parent)
        return -EIO;

    /* Build page name */
    snprintf(name_buf, sizeof(name_buf), "%s.PAGE", get_name(parent));

    /* Create page via callback */
    page = menu_ctx->create_page_cb(parent, name_buf);
    if (!page)
        return -EIO;

    menu_ctx->act_page = page;

    return 0;
}
static int32_t create_page_ctn(lv_obj_t *menu)
{
    lv_obj_t *page_ctn;
    menu_ctx_t *menu_ctx;
    int32_t scr_rot;
    char name_buf[64];

    menu_ctx = menu ? (menu_ctx_t *)get_internal_data(menu) : NULL;
    if (!menu_ctx)
        return -EINVAL;

    /* Build unique name for page container */
    snprintf(name_buf, sizeof(name_buf), "%s.PAGE_CTN", get_name(menu));

    /* Create default page container (right-side column) */
    page_ctn = create_box(menu, name_buf);
    if (!page_ctn)
        return -EIO;

    /* Adjust grid alignment depending on current screen rotation */
    scr_rot = get_scr_rotation();
    if (scr_rot == ROTATION_0) {
        set_grid_cell_align(page_ctn,
                            LV_GRID_ALIGN_STRETCH, 1, 1,
                            LV_GRID_ALIGN_STRETCH, 0, 1);
        /* Default object rotation is ROTATION_0 */
    } else {
        set_grid_cell_align(page_ctn,
                            LV_GRID_ALIGN_STRETCH, 0, 1,
                            LV_GRID_ALIGN_STRETCH, 0, 1);
        /*
         * If created while the screen is rotated to 180,
         * fix rotation to correct the next transition.
         */
        get_meta(page_ctn)->data.rotation = ROTATION_180;
    }

    /*
     * The page container holds detailed content or configuration
     * of the currently active menu item (interactive region).
     */
    menu_ctx->page_ctn = page_ctn;

    return 0;
}

int32_t show_and_hide_page_cb(lv_obj_t *menu)
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
        if (menu_ctx->page_visible) {
            ret = remove_grid_layout_last_row_dsc(menu);
            if (ret) {
                LOG_ERROR("Remove layout failed (%d)", ret);
                return ret;
            }

            menu_ctx->page_visible = false;
            menu_ctx->page_ctn = NULL;
            menu_ctx->act_page = NULL;
        }
    } else {
        /* Show page container in horizontal rotation */
        if (!menu_ctx->page_visible) {
            ret = add_grid_layout_col_dsc(menu, LV_GRID_FR(65));
            if (ret) {
                LOG_ERROR("Add layout failed (%d)", ret);
                return ret;
            }

            menu_ctx->page_visible = true;

            /*
             * Active page may exist if user created it while in vertical mode.
             * Remove it to avoid incorrect parent linkage.
             */
            if (menu_ctx->act_page) {
                remove_obj_and_child(get_meta(menu_ctx->act_page)->id, \
                                     &get_meta(menu)->child);
                menu_ctx->act_page = NULL;
            }
        }
    }

    /* Apply updated grid layout configuration */
    apply_grid_layout_config(menu);

    /* Create or reload page container when visible but not yet initialized */
    if (menu_ctx->page_visible && !menu_ctx->page_ctn) {
        ret = create_page_ctn(menu);
        if (ret)
            return ret;

        ret = load_active_menu_page(menu);
        if (ret)
            return ret;

        refresh_object_tree_layout(menu_ctx->act_page);
    }

    return 0;
}

static int32_t create_menu_views(lv_obj_t *menu)
{
    menu_ctx_t *menu_ctx;
    lv_obj_t *menu_ctn;
    char name_buf[100];
    int32_t ret;

    menu_ctx = menu ? get_internal_data(menu) : NULL;
    if (!menu_ctx)
        return -EINVAL;

    sprintf(name_buf, "%s.MENU_CTN", get_name(menu));

    /* Create left column for menu bar */
    menu_ctn = create_box(menu, name_buf);
    if (!menu_ctn)
        return -EINVAL;

    set_grid_cell_align(menu_ctn, \
                        LV_GRID_ALIGN_STRETCH, 0, 1, \
                        LV_GRID_ALIGN_STRETCH, 0, 1);

    /*
     * The menu container holds the menu bar items defined by user.
     * Each item can trigger a corresponding page via callback.
     */
    menu_ctx->menu_ctn = menu_ctn;

    /* Create right-side page container */
    ret = create_page_ctn(menu);
    if (ret)
        return ret;

    return 0;
}

static int32_t initial_menu_views(lv_obj_t *menu)
{
    menu_ctx_t *menu_ctx;
    int32_t ret;

    menu_ctx = menu ? get_internal_data(menu) : NULL;
    if (!menu_ctx)
        return -EINVAL;

    ret = create_menu_views(menu);
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

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t *create_menu_item(lv_obj_t *menu, lv_obj_t *menu_bar, \
                           const char *sym_index, const char *title, \
                           lv_obj_t *(*create_page_cb)(lv_obj_t *, const char *))
{
    lv_obj_t *item, *sym, *label, *first_child;
    item_ctx_t *item_ctx;
    char name_buf[100];

    if (!menu || !menu_bar)
        return NULL;

    sprintf(name_buf, "%s.%s", get_name(menu_bar), title);

    /* Create the container (menu item) */
    item = create_flex_layout_object(menu_bar, name_buf);
    if (!item)
        return NULL;

    item_ctx = calloc(1, sizeof(*item_ctx));
    if (!item_ctx)
        return NULL;

    set_flex_layout_flow(item, LV_FLEX_FLOW_ROW);
    set_size(item, LV_PCT(100), 50);
    set_padding(item, 0, 0, 20, 20);
    lv_obj_set_style_bg_color(item, lv_color_hex(bg_color(1)), 0);

    lv_obj_add_flag(item, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(item, menu_item_event_handler, LV_EVENT_ALL, NULL);

    /* Add border for non-first child */
    first_child = lv_obj_get_child(menu_bar, 0);
    if (first_child != item) {
        set_border_side(item, LV_BORDER_SIDE_TOP);
        lv_obj_set_style_border_width(item, 2, 0);
        lv_obj_set_style_border_color(item, lv_color_black(), 0);
    }

    /* Flex alignment */
    set_flex_layout_align(item, \
                          LV_FLEX_ALIGN_SPACE_BETWEEN, \
                          LV_FLEX_ALIGN_CENTER, \
                          LV_FLEX_ALIGN_CENTER);

    /* Create children: symbol + title */
    sym = create_symbol_box(item, NULL, &terminal_icons_32, sym_index);
    if (!sym)
        LOG_ERROR("Menu item [%s] create symbol failed", name_buf);

    label = create_text_box(item, NULL, &lv_font_montserrat_24, title);
    if (!label)
        LOG_ERROR("Menu item [%s] create label failed", name_buf);

    set_internal_data(item, item_ctx);
    item_ctx->menu = menu;
    item_ctx->create_page_cb = create_page_cb;

    return item;
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

    group = create_flex_layout_object(par, name_buf);
    if (!group)
        return NULL;

    /* Layout & style configuration */
    set_size(group, LV_PCT(100), LV_SIZE_CONTENT);
    set_padding(group, 20, 20, 20, 20);
    set_flex_layout_flow(group, LV_FLEX_FLOW_COLUMN);
    set_flex_layout_align(group, \
                          LV_FLEX_ALIGN_START, \
                          LV_FLEX_ALIGN_CENTER, \
                          LV_FLEX_ALIGN_CENTER);

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

    menu_ctx = menu ? get_internal_data(menu) : NULL;
    if (!menu_ctx)
        return NULL;

    sprintf(name_buf, "%s.%s", get_name(menu_ctx->menu_ctn), "MENU_BAR");

    menu_bar = create_flex_layout_object(menu_ctx->menu_ctn, name_buf);
    if (!menu_bar)
        return NULL;

    /* Style and layout configuration */
    lv_obj_set_style_bg_color(menu_bar, lv_color_hex(bg_color(120)), 0);
    set_size(menu_bar, LV_PCT(100), LV_PCT(100));
    set_align(menu_bar, menu_ctx->menu_ctn, LV_ALIGN_CENTER, 0, 0);
    set_padding(menu_bar, 4, 4, 4, 4);
    set_row_padding(menu_bar, 20);

    /* Scrollbar cleanup */
    lv_obj_set_style_width(menu_bar, 1, LV_PART_SCROLLBAR);
    lv_obj_set_style_pad_all(menu_bar, 0, LV_PART_SCROLLBAR);
    lv_obj_set_style_margin_all(menu_bar, 0, LV_PART_SCROLLBAR);

    /* Flex configuration */
    set_flex_layout_flow(menu_bar, LV_FLEX_FLOW_COLUMN);
    set_flex_layout_align(menu_bar, \
                          LV_FLEX_ALIGN_START, \
                          LV_FLEX_ALIGN_CENTER, \
                          LV_FLEX_ALIGN_CENTER);

    return menu_bar;
}

/*
 * Create a menu page under the given parent.
 * Each page uses a flex column layout with a control bar on top.
 */
lv_obj_t *create_menu_page(lv_obj_t *par, const char *name)
{
    lv_obj_t *page;
    lv_obj_t *control;
    char name_buf[64];
    int32_t ret;

    if (!par)
        return NULL;

    page = create_flex_layout_object(par, name);
    if (!page)
        return NULL;

    lv_obj_set_style_bg_color(page, lv_color_hex(bg_color(120)), 0);
    set_size(page, LV_PCT(100), LV_PCT(100));
    set_align(page, par, LV_ALIGN_CENTER, 0, 0);

    ret = set_padding(page, 4, 4, 4, 4);
    if (ret)
        LOG_WARN("Page [%s] set padding failed (%d)", get_name(page), ret);

    set_row_padding(page, 20);
    set_flex_layout_flow(page, LV_FLEX_FLOW_COLUMN);
    set_flex_layout_align(page, \
                          LV_FLEX_ALIGN_START, \
                          LV_FLEX_ALIGN_CENTER, \
                          LV_FLEX_ALIGN_CENTER);

    lv_obj_set_style_width(page, 1, LV_PART_SCROLLBAR);
    lv_obj_set_style_pad_all(page, 0, LV_PART_SCROLLBAR);
    lv_obj_set_style_margin_all(page, 0, LV_PART_SCROLLBAR);

    snprintf(name_buf, sizeof(name_buf), "%s.CONTROL", name);
    control = create_page_control(page, name_buf, true, false);
    if (!control)
        return NULL;

    return page;
}

int32_t set_active_menu_page(lv_obj_t *lobj, \
                             lv_obj_t *(*create_page_cb)(lv_obj_t *, \
                                                         const char *))
{
    menu_ctx_t *menu_ctx;

    menu_ctx = lobj ? get_internal_data(lobj) : NULL;
    if (!menu_ctx)
        return -EINVAL;

    /* Skip if same page callback is already active */
    if (menu_ctx->act_page && menu_ctx->create_page_cb == create_page_cb) {
        LOG_TRACE("Menu page [%s] is already active -> ignore", \
                  get_name(menu_ctx->act_page));
        return 0;
    }

    menu_ctx->create_page_cb = create_page_cb;

    return load_active_menu_page(lobj);
}

lv_obj_t *create_menu(lv_obj_t *par, const char *name)
{
    menu_ctx_t *menu_ctx;
    lv_obj_t *lobj;
    int32_t ret;

    if (!par)
        return NULL;

    menu_ctx = (menu_ctx_t *)calloc(1, sizeof(menu_ctx_t));
    if (!menu_ctx)
        return NULL;

    menu_ctx->page_visible = true;

    /* Create main container using grid layout */
    lobj = create_grid_layout_object(par, name);
    if (!lobj)
        return NULL;

    ret = add_grid_layout_row_dsc(lobj, LV_GRID_FR(98));
    if (ret)
        LOG_ERROR("Layout [%s] row descriptor failed", get_name(lobj));

    ret = add_grid_layout_col_dsc(lobj, LV_GRID_FR(35));
    if (ret)
        LOG_ERROR("Layout [%s] column descriptor failed", get_name(lobj));

    ret = add_grid_layout_col_dsc(lobj, LV_GRID_FR(65));
    if (ret)
        LOG_ERROR("Layout [%s] column descriptor failed", get_name(lobj));

    apply_grid_layout_config(lobj);
    set_grid_layout_align(lobj, \
                          LV_GRID_ALIGN_SPACE_BETWEEN, \
                          LV_GRID_ALIGN_SPACE_BETWEEN);

    /* Base style setup */
    lv_obj_set_style_bg_color(lobj, lv_color_hex(bg_color(80)), 0);
    lv_obj_set_style_radius(lobj, 16, 0);
    set_column_padding(lobj, 8);

    ret = set_padding(lobj, 20, 20, 20, 20);
    if (ret)
        LOG_WARN("Layout [%s] set padding failed, ret %d", \
                 get_name(lobj), ret);

    /* Context and layout rotation callback */
    set_internal_data(lobj, menu_ctx);
    get_meta(lobj)->data.post_children_rotate_cb = show_and_hide_page_cb;

    /* Initialize sub-views */
    ret = initial_menu_views(lobj);
    if (ret)
        LOG_WARN("Menu [%s] view initialization failed, ret %d", \
                 get_name(lobj), ret);

    return lobj;
}
