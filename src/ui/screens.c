/**
 * @file screens.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
// #define LOG_LEVEL LOG_LEVEL_TRACE
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
#include "list.h"
#include "ui/ui_core.h"
#include "ui/fonts.h"
#include "ui/comps.h"
#include "ui/windows.h"
#include "ui/screen.h"
#include "ui/grid.h"
#include "ui/flex.h"
#include "main.h"
#include "comm/cmd_payload.h"

/*********************
 *      DEFINES
 *********************/
#define SCREEN_BG_COLOR                 0x292D33

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
static lv_obj_t *keyboard_box = NULL;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
#define TEST 1
#if defined(TEST)
#include <time.h>

int32_t get_random_3(void)
{
    int32_t val;

    val = rand() % 4;
    return val;
}

typedef struct {
    lv_obj_t *cont;
    uint32_t t_phase1;
    uint32_t t_phase2;
    uint32_t t_phase3;
    uint32_t t_phase4;
    void (*layout_cb)(lv_obj_t *obj);
} rotate_anim_ctx_t;

static void zoom_exec(void *obj, int32_t v)
{
    lv_obj_t *o = (lv_obj_t *)obj;

    lv_obj_set_style_transform_pivot_x(o, lv_obj_get_width(o) / 2, 0);
    lv_obj_set_style_transform_pivot_y(o, lv_obj_get_height(o) / 2, 0);

    lv_obj_set_style_transform_zoom((lv_obj_t *)obj, v, 0);
}

static void fade_exec(void *obj, int32_t opa)
{
    lv_obj_set_style_opa((lv_obj_t *)obj, opa, 0);
}

static void phase3_start(lv_anim_t *a);

static void phase2_done(lv_anim_t *a_end)
{
    rotate_anim_ctx_t *ctx2 = a_end->user_data;

    if (ctx2->layout_cb)
        ctx2->layout_cb(ctx2->cont);

    lv_anim_t a3;
    lv_anim_init(&a3);
    lv_anim_set_var(&a3, ctx2->cont);
    lv_anim_set_exec_cb(&a3, zoom_exec);
    lv_anim_set_values(&a3, LV_SCALE_NONE - 30, LV_SCALE_NONE - 10);
    lv_anim_set_time(&a3, ctx2->t_phase3);
    lv_anim_set_path_cb(&a3, lv_anim_path_ease_out);
    lv_anim_start(&a3);

    lv_anim_t a4;
    lv_anim_init(&a4);
    lv_anim_set_var(&a4, ctx2->cont);
    lv_anim_set_exec_cb(&a4, zoom_exec);
    lv_anim_set_values(&a4, LV_SCALE_NONE - 10, LV_SCALE_NONE);
    lv_anim_set_time(&a4, ctx2->t_phase4);
    lv_anim_set_path_cb(&a4, lv_anim_path_ease_out);
    lv_anim_start(&a4);
}

static void phase2_start(lv_anim_t *a)
{
    rotate_anim_ctx_t *ctx = a->user_data;

    lv_anim_t a2;
    lv_anim_init(&a2);
    lv_anim_set_var(&a2, ctx->cont);
    lv_anim_set_exec_cb(&a2, zoom_exec);
    lv_anim_set_values(&a2, LV_SCALE_NONE - 10, LV_SCALE_NONE - 30);
    lv_anim_set_time(&a2, ctx->t_phase2);
    lv_anim_set_path_cb(&a2, lv_anim_path_ease_in);
    lv_anim_set_ready_cb(&a2, phase2_done);
    lv_anim_set_user_data(&a2, ctx);
    lv_anim_start(&a2);
}

void rotate_anim_start(lv_obj_t *cont, \
                       uint32_t t1, uint32_t t2, \
                       uint32_t t3, uint32_t t4, \
                       void (*layout_cb)(lv_obj_t *obj))
{
    static rotate_anim_ctx_t ctx;
    ctx.cont = cont;
    ctx.t_phase1 = t1;
    ctx.t_phase2 = t2;
    ctx.t_phase3 = t3;
    ctx.t_phase4 = t4;
    ctx.layout_cb = layout_cb;

    lv_obj_set_style_transform_zoom(cont, LV_SCALE_NONE, 0);
    lv_obj_set_style_opa(cont, LV_OPA_COVER, 0);

    lv_anim_t a1;
    lv_anim_init(&a1);
    lv_anim_set_var(&a1, cont);
    lv_anim_set_exec_cb(&a1, zoom_exec);
    lv_anim_set_values(&a1, LV_SCALE_NONE, LV_SCALE_NONE - 10);
    lv_anim_set_time(&a1, t1);
    lv_anim_set_path_cb(&a1, lv_anim_path_ease_in);
    lv_anim_set_ready_cb(&a1, phase2_start);
    lv_anim_set_user_data(&a1, &ctx);
    lv_anim_start(&a1);
}

static void refresh_screen_rotate_layout(lv_obj_t *root)
{
    ctx_t *ctx = get_ctx();
    lv_obj_t *screen = ctx->scr.now.obj;

    refresh_object_tree_layout(screen);
}

static void rotate_key_handler(lv_event_t *event)
{
    int32_t ret;
    ctx_t *ctx = get_ctx();
    static bool detail_window_added = true;
    static bool new_window = false;

    lv_obj_t *kb = get_obj_by_name(COMPS_KEYBOARD, \
                                   &get_meta(lv_screen_active())->child);

    set_scr_rotation(get_random_3());

    if (kb) {
        remove_keyboard(ctx);
    }

    // rotate_anim_start(screen,
    //               120, /* phase 1 */
    //               280, /* phase 2 */
    //               450, /* phase 3 */
    //               350, /* phase 4 */
    //               refresh_screen_rotate_layout);

    refresh_object_tree_layout(ctx->scr.now.obj);

    if (kb) {
        kb = create_keyboard(keyboard_box);
        if (!kb)
            LOG_ERROR("Create keyboard failed");
    }
}

static void create_keyboard_handler(lv_event_t *event)
{
    static bool added;
    lv_obj_t *kb, *top_layout;
    ctx_t *ctx = get_ctx();
    int32_t ret, scr_rot;
    bool is_vert;

    top_layout = get_obj_by_name(LAYOUT_SETTING, \
                    &get_meta(lv_screen_active())->child);
    scr_rot = get_scr_rotation();
    is_vert = (scr_rot == ROTATION_0 || scr_rot == ROTATION_180);

    if (!added) {
        ret = is_vert ? add_grid_layout_row_dsc(top_layout, LV_GRID_FR(30))
                      : add_grid_layout_col_dsc(top_layout, LV_GRID_FR(30));
    } else {
        ret = is_vert ? remove_grid_layout_last_row_dsc(top_layout)
                      : remove_grid_layout_last_column_dsc(top_layout);
    }

    if (ret)
        LOG_ERROR("%s descriptor failed, ret %d", added ? "Remove" : "Add", ret);
    else
        added = !added;

    apply_grid_layout_config(top_layout);

    kb = get_obj_by_name(COMPS_KEYBOARD, \
            &get_meta(lv_screen_active())->child);

    if (!kb) {
        kb = create_keyboard(keyboard_box);
        if (!kb)
            LOG_ERROR("Create keyboard failed");
    } else {
        remove_keyboard(ctx);
    }
}

#endif

/*
 * create_common_screen - Initialize the main setting screen container.
 *
 * This function builds the base layout of the screen including:
 *   - A grid-based container layout (root)
 *   - A top bar section
 *   - A setting container (content area)
 *
 * The screen layout:
 *   ---------------------------
 *   |        Top bar          |  (fixed height)
 *   ---------------------------
 *   |   Setting container     |  (expandable area)
 *   ---------------------------
 *
 * The layout is intentionally simple and flexible, allowing dynamic
 * rebuild of the child layout when rotation or display configuration
 * changes.
 *
 * Return:
 *   Pointer to the created base LVGL object, or NULL if creation failed.
 */
lv_obj_t *create_common_screen(ctx_t *ctx, lv_obj_t *par, const char *name)
{
    lv_obj_t *base, *top_bar, *setting_container;
    int32_t ret;

    if (!ctx || !par)
        return NULL;

    /*-----------------------------------------
     * Background initialization
     *----------------------------------------*/
    lv_obj_set_style_bg_color(par, lv_color_black(), 0);

    /*-----------------------------------------
     * Create root container using grid layout
     *----------------------------------------*/
    base = create_grid_layout_object(par, LAYOUT_SETTING);
    if (!base)
        return NULL;

    set_obj_type(base, OBJ_BASE);

    /* Grid rows: [top bar: 50px | content: flexible 60%] */
    if (add_grid_layout_row_dsc(base, 50) ||
        add_grid_layout_row_dsc(base, LV_GRID_FR(60)))
        LOG_ERROR("Add row descriptor failed");

    /* Single column layout occupying 98% of width */
    if (add_grid_layout_col_dsc(base, LV_GRID_FR(98)))
        LOG_ERROR("Add col descriptor failed");

    apply_grid_layout_config(base);
    set_grid_layout_align(base, LV_GRID_ALIGN_SPACE_BETWEEN,
                               LV_GRID_ALIGN_SPACE_BETWEEN);

    /*-----------------------------------------
     * Base style setup
     *----------------------------------------*/
    set_size(base, get_scr_width(), get_scr_height());
    set_pos(base, 0, 0);
    set_padding(base, 8, 8, 8, 8);
    set_row_padding(base, 8);
    lv_obj_set_style_radius(base, 20, 0);
    lv_obj_set_style_bg_color(base, lv_color_hex(bg_color(20)), 0);

    /*-----------------------------------------
     * Create top bar section
     *----------------------------------------*/
    top_bar = create_box(base, "top_bar");
    set_grid_cell_align(top_bar, LV_GRID_ALIGN_STRETCH, 0, 1,
                                   LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_style_radius(top_bar, 16, 0);
    lv_obj_set_style_bg_color(top_bar, lv_color_hex(bg_color(40)), 0);

    /*-----------------------------------------
     * Create setting container section
     *----------------------------------------*/
    setting_container = create_menu(base, WINDOW_SETTING);
    set_grid_cell_align(setting_container,
                        LV_GRID_ALIGN_STRETCH, 0, 1,
                        LV_GRID_ALIGN_STRETCH, 1, 1);

    set_active_menu_page(setting_container, "BRIGHTNESS", create_brightness_setting);
    /*-----------------------------------------
     * Save context for future reference
     *----------------------------------------*/
    ctx->scr.now.obj = base;

#if defined(TEST)
    /*-----------------------------------------
     * Debug/test utilities
     *----------------------------------------*/
    lv_obj_t *btn, *icon;

    /* Rotate test button */
    btn = create_btn(lv_layer_top(), "btn.rotate");
    lv_obj_add_event_cb(btn, rotate_key_handler, LV_EVENT_CLICKED, get_meta(btn));
    set_size(btn, 54, 54);
    set_align(btn, lv_layer_top(), LV_ALIGN_TOP_RIGHT, -150, 0);

    icon = create_sym(btn, NULL, TOP_BAR_SYM_FONTS, ICON_ROTATE_SOLID);
    lv_obj_set_style_text_color(icon, lv_color_hex(bg_color(60)), 0);

    /* Keyboard test button */
    btn = create_btn(lv_layer_top(), "btn.keyboard");
    lv_obj_add_event_cb(btn, create_keyboard_handler, LV_EVENT_CLICKED, get_meta(btn));
    set_size(btn, 54, 54);
    set_align(btn, lv_layer_top(), LV_ALIGN_TOP_RIGHT, -50, 0);

    icon = create_sym(btn, NULL, TOP_BAR_SYM_FONTS, ICON_KEYBOARD);
    lv_obj_set_style_text_color(icon, lv_color_hex(bg_color(60)), 0);
#endif

    return base;
}
