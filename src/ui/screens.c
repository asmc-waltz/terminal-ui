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
#include "ui/pages.h"
#include "ui/screen.h"
#include "main.h"

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

    refresh_obj_tree_layout(screen->user_data);
}

static void rotate_key_handler(lv_event_t *event)
{
    lv_obj_t *lobj = get_obj_by_name(COM_SCR_NAME, \
                            &((gobj_t *)(lv_screen_active()->user_data))->child);

    set_scr_rotation(get_random_3());

    ctx_t *ctx = get_ctx();
    lv_obj_t *screen = ctx->scr.now.obj;

    // rotate_anim_start(screen,
    //               120, /* phase 1 */
    //               280, /* phase 2 */
    //               450, /* phase 3 */
    //               350, /* phase 4 */
    //               refresh_screen_rotate_layout);
    refresh_obj_tree_layout(screen->user_data);
}

static void create_keyboard_handler(lv_event_t *event)
{
    ctx_t *ctx = get_ctx();
    static bool kb_en = false;
    lv_obj_t *kb;

    if (!ctx) {
        LOG_ERROR("Runtime context not found");
        return;
    }

    if (!kb_en) {
        kb = create_keyboard(ctx);
        if (kb)
            kb_en = true;
    } else {
        remove_keyboard(ctx);
        kb_en = false;

    }

    refresh_obj_tree_layout((ctx->scr.now.obj)->user_data);
}

#endif

lv_obj_t *create_common_screen(ctx_t *ctx, lv_obj_t *par, const char *name)
{
    int32_t obj_w, obj_h;
    lv_obj_t *screen;
    lv_obj_t *bg = lv_scr_act();

    lv_obj_set_style_bg_color(bg, lv_color_black(), 0);

    screen = create_base(par, name);
    if (!screen) {
        LOG_ERROR("Unable to create common screen");
        return NULL;
    }
    ctx->scr.now.obj = screen;

    set_gobj_size(screen, get_scr_width(), get_scr_height());
    set_gobj_pos(screen, 0, 0);

    // TODO: create background
    lv_obj_set_style_bg_color(screen, lv_color_hex(SCREEN_BG_COLOR), 0);


    lv_obj_t *top_space = create_top_bar(ctx);
    lv_obj_t *sym_box = add_top_bar_symbol(top_space, TOP_BAR_NAME".wifi",
                                                ICON_WIFI_SOLID);
    set_gobj_align_scale_x(sym_box, top_space, LV_ALIGN_LEFT_MID, TOP_BAR_SYM_ALN, 0);

    // TODO: debug setting screen
    create_setting_page(ctx);


#if defined(TEST)
    lv_obj_t *btn = create_btn(lv_layer_top(), "common.rotate_btn");
    lv_obj_add_event_cb(btn, rotate_key_handler, LV_EVENT_CLICKED, btn->user_data);
    set_gobj_size(btn, 54, 54);
    set_gobj_align(btn, lv_layer_top(), LV_ALIGN_CENTER, -50, 0);

    lv_obj_t *icon = create_sym(btn, NULL, TOP_BAR_SYM_FONTS, ICON_ROTATE_SOLID);
    lv_obj_set_style_text_color(icon, lv_color_hex(0xFFFFFF), 0);


    btn = create_btn(lv_layer_top(), "create keyboard");
    set_gobj_size(btn, 54, 54);
    set_gobj_align(btn, lv_layer_top(), LV_ALIGN_CENTER, 50, 0);
    lv_obj_add_event_cb(btn, create_keyboard_handler, LV_EVENT_CLICKED, btn->user_data);

    icon = create_sym(btn, NULL, TOP_BAR_SYM_FONTS, ICON_KEYBOARD);
    lv_obj_set_style_text_color(icon, lv_color_hex(0xFFFFFF), 0);
#endif

    return screen;
}
