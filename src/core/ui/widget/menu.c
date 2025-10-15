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
int32_t load_menu_item_page(lv_obj_t *lobj)
{
    lv_obj_t *menu;
    menu_ctx_t *menu_ctx;
    item_ctx_t *item_ctx;
    int32_t ret = 0;

    item_ctx = lobj ? (item_ctx_t *)get_internal_data(lobj) : NULL;
    if (!item_ctx)
        return -EINVAL;

    menu = item_ctx->menu;
    if (!menu)
        return -EIO;


    menu_ctx = get_internal_data(item_ctx->menu);
    if (!menu_ctx)
        return -EIO;

    if (menu_ctx->act_menu_item != lobj && menu_ctx->act_page) {
        remove_obj_and_child(get_meta(menu_ctx->act_page)->id, \
                                     &get_meta(menu)->child);

        menu_ctx->act_page = NULL;
    }

    ret = set_active_menu_page(menu, item_ctx->create_page_cb);
    if (ret)
        return ret; 

    menu_ctx->act_menu_item = lobj;
    
    return refresh_object_tree_layout(menu_ctx->act_page);
}

static void menu_item_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    switch (code) {
    case LV_EVENT_PRESSED:
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xFF6633), 0);
        break;
    case LV_EVENT_RELEASED:
        lv_obj_set_style_bg_color(obj, lv_color_hex(bg_color(1)), 0);
        load_menu_item_page(obj);
        break;
    case LV_EVENT_CLICKED:
        LV_LOG_USER("Box clicked!");
        break;
    default:
        break;
    }
}

int32_t create_page_ctn(lv_obj_t *menu)
{
    lv_obj_t *page_ctn;
    int32_t scr_rot, ret;
    char name_buf[100];
    menu_ctx_t *menu_ctx;

    menu_ctx = menu ? get_internal_data(menu) : NULL;
    if (!menu_ctx)
        return -EINVAL;

    sprintf(name_buf, "%s.%s", get_name(menu), "PAGE_CTN");
    /* Create the default page_ctn space as the right side colum */
    page_ctn = create_box(menu, name_buf);
    if (!page_ctn)
        return -EINVAL;

    scr_rot = get_scr_rotation();
    if (scr_rot == ROTATION_0)
        set_grid_cell_align(page_ctn,
                            LV_GRID_ALIGN_STRETCH, 1, 1,
                            LV_GRID_ALIGN_STRETCH, 0, 1);
        // The object rotation is set as ROTATION_0 by default
    else {
        set_grid_cell_align(page_ctn,
                            LV_GRID_ALIGN_STRETCH, 0, 1,
                            LV_GRID_ALIGN_STRETCH, 0, 1);
        /*
         * If the object is created while screen is rotated to 180.
         * We must set this value to correct the next rotation turn.
         */
        get_meta(page_ctn)->data.rotation = ROTATION_180;
    }

    /*
     * Page container will contain the detail information or configuration depend
     * on the activated item in menu bar. As know as the container of user interative
     * configuration.
     */

    menu_ctx->page_ctn = page_ctn;

    return 0;
}

int32_t show_and_hide_detail_cb(lv_obj_t *menu)
{
    int32_t ret = 0;
    menu_ctx_t *menu_ctx = get_internal_data(menu);

    int32_t scr_rot = get_scr_rotation();

    if (scr_rot == ROTATION_90 || scr_rot == ROTATION_270) {
        if (menu_ctx->page_visible) {
            ret = remove_grid_layout_last_row_dsc(menu);
            if (ret) {
                LOG_ERROR("Remove detail layout failed, ret %d", ret);
            } else {
                menu_ctx->page_visible = false;
                menu_ctx->page_ctn = NULL;
                menu_ctx->act_page = NULL;
            }
        }
    } else if (scr_rot == ROTATION_0 || scr_rot == ROTATION_180) {
        if (!menu_ctx->page_visible) {
            ret = add_grid_layout_col_dsc(menu, LV_GRID_FR(65));
            if (ret) {
                LOG_ERROR("Add detail layout failed, ret %d", ret);
            } else {
                menu_ctx->page_visible = true;
            }

            /*
             * Active page could be created by user while the screen in vertical
             * state and parent will be the menu bar container instead of page
             */
            if (menu_ctx->act_page) {
                remove_obj_and_child(get_meta(menu_ctx->act_page)->id, \
                                     &get_meta(menu)->child);
                menu_ctx->act_page = NULL;
            }
        }
    }

    apply_grid_layout_config(menu);

    if (menu_ctx->page_visible && !menu_ctx->page_ctn) {
        if (create_page_ctn(menu) || load_active_menu_page(menu)) {
            return -EIO;
        }
        refresh_object_tree_layout(menu_ctx->act_page);
    }

    return 0;
}



static int32_t create_menu_views(lv_obj_t *menu)
{
    int32_t ret;
    lv_obj_t *menu_ctn, *page_ctn;
    menu_ctx_t *menu_ctx;
    char name_buf[100];


    menu_ctx = menu ? get_internal_data(menu) : NULL;
    if (!menu_ctx)
        return -EINVAL;

    sprintf(name_buf, "%s.%s", get_name(menu), "MENU_CTN");
    /* Create the default menu bar as the left side colum */
    menu_ctn = create_box(menu, name_buf);
    if (!menu_ctn)
        return -EINVAL;

    menu_ctx->menu_ctn = menu_ctn;
    set_grid_cell_align(menu_ctn, \
                        LV_GRID_ALIGN_STRETCH, 0, 1, \
                        LV_GRID_ALIGN_STRETCH, 0, 1);
    /*
     * Menu container will contain the menu bar which will be define item by user
     * User must define the caller of subpage for each item in menu bar. It will
     * be automatically call when menu bar item is clicked.
     */

    return create_page_ctn(menu);
}

static int32_t initial_menu_views(lv_obj_t *menu)
{
    int32_t ret;
    lv_obj_t *menu_bar;
    menu_ctx_t *menu_ctx;

    menu_ctx = menu ? get_internal_data(menu) : NULL;
    if (!menu_ctx)
        return -EINVAL;

    ret = create_menu_views(menu);
    if (ret) {
        LOG_WARN("Layout [%s] create holder failed, ret %d", \
                 get_name(menu), ret);
        return ret;
    }

    return 0;
}


/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/*
 * Menu item: an entry to open a specific setting window.
 * Each menu item consists of a symbol (icon) and a title label.
 */
lv_obj_t *create_menu_item(lv_obj_t *menu, lv_obj_t *menu_bar, \
                           const char *sym_index, const char *title,\
                           lv_obj_t *(* create_page_cb)(lv_obj_t *, const char *))
{
    lv_obj_t *item, *sym, *label;
    lv_obj_t *first_child;
    item_ctx_t *item_ctx;
    char name_buf[100];

    if (!menu || !menu_bar)
        return NULL;

    sprintf(name_buf, "%s.%s", get_name(menu_bar), title);

    /* Create container (menu item) */
    item = create_flex_layout_object(menu_bar, name_buf);
    if (!item)
        return NULL;

    item_ctx = (item_ctx_t *)calloc(1, sizeof(item_ctx_t));
    if (!item_ctx)
        return NULL;

    set_flex_layout_flow(item, LV_FLEX_FLOW_ROW);

    set_size(item, LV_PCT(100), 50);
    set_padding(item, 0, 0, 20, 20);
    lv_obj_set_style_bg_color(item, lv_color_hex(bg_color(1)), 0);

    lv_obj_add_flag(item, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(item, menu_item_event_handler, LV_EVENT_ALL, NULL);

    /* Style */
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
        LOG_ERROR("Menu label [%s] create symbol failed", name_buf);

    set_internal_data(item, item_ctx);
    item_ctx->create_page_cb = create_page_cb;
    item_ctx->menu = menu;

    return item;
}
 
/*
 * Setting group: a child container inside the menu bar.
 * This container uses a flex layout with vertical flow,
 * and its size depends on the number of child elements.
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

    /* Visual style */
    set_size(group, LV_PCT(100), LV_SIZE_CONTENT);
    /* Padding and spacing */
    set_padding(group, 20, 20, 20, 20);
    /* Layout configuration */
    set_flex_layout_flow(group, LV_FLEX_FLOW_COLUMN);


    /*------------------------------
     * Flex alignment
     *-----------------------------*/
    set_flex_layout_align(group, \
                          LV_FLEX_ALIGN_START, \
                          LV_FLEX_ALIGN_CENTER, \
                          LV_FLEX_ALIGN_CENTER);

    return group;
}

/*
 * Menu bar: a left-side panel containing system settings.
 * This bar is organized in a vertical flex layout, and itself
 * contains one or more grouped flex containers.
 */
lv_obj_t *create_menu_bar(lv_obj_t *menu)
{
    lv_obj_t *menu_bar;
    menu_ctx_t *menu_ctx;
    char name_buf[100];

    menu_ctx = menu ? get_internal_data(menu) : NULL;
    if (!menu_ctx)
        return NULL;

    sprintf(name_buf, "%s.%s", get_name(menu_ctx->menu_ctn), "MENU_BAR");

    menu_bar = create_flex_layout_object(menu_ctx->menu_ctn, name_buf);
    if (!menu_bar)
        return NULL;

    /* Visual style */
    lv_obj_set_style_bg_color(menu_bar, lv_color_hex(bg_color(120)), 0);
    set_size(menu_bar, LV_PCT(100), LV_PCT(100));
    set_align(menu_bar, menu_ctx->menu_ctn, LV_ALIGN_CENTER, 0, 0);
    /* Padding and spacing */
    set_padding(menu_bar, 4, 4, 4, 4);
    set_row_padding(menu_bar, 20);
    /* Scrollbar cleanup */
    lv_obj_set_style_width(menu_bar, 1, LV_PART_SCROLLBAR);
    lv_obj_set_style_pad_all(menu_bar, 0, LV_PART_SCROLLBAR);
    lv_obj_set_style_margin_all(menu_bar, 0, LV_PART_SCROLLBAR);

    /* Layout configuration */
    set_flex_layout_flow(menu_bar, LV_FLEX_FLOW_COLUMN);

    /* Flex alignment */
    set_flex_layout_align(menu_bar, \
                          LV_FLEX_ALIGN_START, \
                          LV_FLEX_ALIGN_CENTER, \
                          LV_FLEX_ALIGN_CENTER);

    return menu_bar;
}

int32_t load_active_menu_page(lv_obj_t *lobj)
{
    menu_ctx_t *menu_ctx;
    lv_obj_t *page;

    menu_ctx = lobj ? get_internal_data(lobj) : NULL;
    if (!menu_ctx)
        return -EINVAL;

    if (!menu_ctx->create_page_cb)
        return -EIO;

    if (menu_ctx->page_ctn) {
        page = menu_ctx->create_page_cb(menu_ctx->page_ctn, "TODO");
        if (!page)
            return -EIO;
    } else {
        LOG_WARN("Reuse the menu bar space");
        page = menu_ctx->create_page_cb(menu_ctx->menu_ctn, "TODO");
        if (!page)
            return -EIO;
    }

    menu_ctx->act_page = page;

    return 0;
}

int32_t set_active_menu_page(lv_obj_t *lobj, \
                             lv_obj_t *(*create_page_cb)(lv_obj_t *, \
                                                         const char *))
{
    menu_ctx_t *menu_ctx;

    menu_ctx = lobj ? get_internal_data(lobj) : NULL;
    if (!menu_ctx)
        return -EINVAL;

    if (menu_ctx->act_page) {
        LOG_WARN("Menu page [%s] is activating -> ignore request", \
                 get_name(menu_ctx->act_page));
        return 0;
    }

    menu_ctx->create_page_cb = create_page_cb;

    return load_active_menu_page(lobj);
}

lv_obj_t *create_menu(lv_obj_t *par, const char *name)
{
    lv_obj_t *lobj;
    int32_t ret;
    menu_ctx_t *menu_ctx;

    if (!par)
        return NULL;

    menu_ctx = (menu_ctx_t *)calloc(1, sizeof(menu_ctx_t));
    if (!menu_ctx)
        return NULL;

    menu_ctx->page_visible = true;

    /*-----------------------------------------
     * Create menu container using grid layout
     *----------------------------------------*/
    lobj = create_grid_layout_object(par, name);
    if (!lobj)
        return NULL;

    ret = add_grid_layout_row_dsc(lobj, LV_GRID_FR(98));
    if (ret)
        LOG_ERROR("Layout [%s] Row descriptor failed", get_name(lobj));

    ret = add_grid_layout_col_dsc(lobj, LV_GRID_FR(35));
    if (ret)
        LOG_ERROR("Layout [%s] Column descriptor failed", get_name(lobj));

    ret = add_grid_layout_col_dsc(lobj, LV_GRID_FR(65));
    if (ret)
        LOG_ERROR("Layout [%s] Column descriptor failed", get_name(lobj));

    apply_grid_layout_config(lobj);

    set_grid_layout_align(lobj, \
                          LV_GRID_ALIGN_SPACE_BETWEEN, \
                          LV_GRID_ALIGN_SPACE_BETWEEN);

    /*-----------------------------------------
     * Base style setup
     *----------------------------------------*/
    lv_obj_set_style_bg_color(lobj, lv_color_hex(bg_color(80)), 0);
    lv_obj_set_style_radius(lobj, 16, 0);
    set_column_padding(lobj, 8);
    ret = set_padding(lobj, 20, 20, 20, 20);
    if (ret)
        LOG_WARN("Layout [%s] set padding failed, ret %d", get_name(lobj), ret);

    set_internal_data(lobj, menu_ctx);
    get_meta(lobj)->data.post_children_rotate_cb = show_and_hide_detail_cb;

    /*-----------------------------------------
     * Views setup
     *----------------------------------------*/
    ret = initial_menu_views(lobj);

    return lobj;
}
