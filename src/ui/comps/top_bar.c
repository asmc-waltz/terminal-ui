/**
 * @file top_bar.c
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

#include <lvgl.h>
#include "list.h"
#include "ui/ui_core.h"
#include "ui/comps.h"
#include "ui/fonts.h"
#include "main.h"

/*********************
 *      DEFINES
 *********************/
#define TOP_BAR_BG_COLOR           0x636D7A

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
int32_t replace_top_bar_symbol(lv_obj_t *par, const char *name, \
                               const char *index)
{
    lv_obj_t *box, *icon;
    gobj_t *gobj_par;
    int32_t ret;

    gobj_par = par->user_data;
    box = get_obj_by_name(name, &gobj_par->child);
    if (!box) {
        LOG_ERROR("Symbol box %s not found", name);
        return -1;
    }

    ret = remove_children(box->user_data);
    if (ret <= 0) {
        LOG_ERROR("Unable to remove children of %s, ret %d", name, ret);
        return -1;
    }

    icon = create_sym(box, NULL, TOP_BAR_SYM_FONTS, index);
    if (!icon) {
        LOG_ERROR("Unable to add symbol %s", name);
        remove_obj_and_child_by_name(name, &gobj_par->child);
        return -1;
    }
    lv_obj_set_style_text_color(icon, lv_color_hex(0xFFFFFF), 0);

    return 0;
}

lv_obj_t *add_top_bar_symbol(lv_obj_t *par, const char *name, const char *index)
{
    lv_obj_t *box, *icon;
    gobj_t *gobj_par;

    box = create_box(par, name);
    if (!box) {
        return NULL;
    }

    lv_obj_set_style_bg_opa(box, LV_OPA_0, 0);
    lv_obj_clear_flag(box, LV_OBJ_FLAG_SCROLLABLE);
    gobj_par = par->user_data;
    set_gobj_size(box, gobj_par->size.h, gobj_par->size.h);

    icon = create_sym(box, NULL, TOP_BAR_SYM_FONTS, index);
    if (!icon) {
        LOG_ERROR("Unable to add symbol %s", name);
        remove_obj_and_child_by_name(name, &gobj_par->child);
        return NULL;
    }
    lv_obj_set_style_text_color(icon, lv_color_hex(0xFFFFFF), 0);

    return box;
}

/*
 * Example usage of add_top_bar_symbol() and replace_top_bar_symbol()
 *
 * This shows how to create top bar symbols, align them relative
 * to each other, and later replace a symbol by name.
 *
 * Add first symbol: signal icon
 *      lv_obj_t *sym = add_top_bar_symbol(top_bar, TOP_BAR_NAME".signal",
 *                                         ICON_SIGNAL_SOLID);
 *      set_gobj_align(sym, top_bar, LV_ALIGN_TOP_LEFT,
 *                       TOP_BAR_SYM_ALN, 0);

 * Add second symbol: WiFi icon, aligned to the right of signal icon
 *      lv_obj_t *sub = add_top_bar_symbol(top_bar, TOP_BAR_NAME".wifi",
 *                                         ICON_WIFI_SOLID);
 *      set_gobj_align(sub, sym, LV_ALIGN_OUT_RIGHT_MID,
 *                       TOP_BAR_SYM_ALN, 0);

 * Add third symbol: alert (bell) icon, centered on top bar
 *      sym = add_top_bar_symbol(top_bar, TOP_BAR_NAME".alert",
 *                               ICON_BELL_SOLID);
 *      set_gobj_align(sym, top_bar, LV_ALIGN_TOP_MID, 0, 0);

 * Replace alert symbol with "bell slash" version
 *      replace_top_bar_symbol(top_bar, TOP_BAR_NAME".alert",
 *                             ICON_BELL_SLASH_SOLID);
 */

lv_obj_t *create_top_bar(ctx_t *ctx)
{
    int32_t obj_w, obj_h, pad_w;
    lv_obj_t *par, *top_bar;

    if (!ctx || !ctx->scr.now.obj)
        return NULL;

    par = ctx->scr.now.obj;

    /* Create container box for top bar */
    top_bar = create_box(par, TOP_BAR_NAME);
    if (!top_bar)
        return NULL;

    ctx->scr.now.top.obj = top_bar;
    ctx->scr.now.top.upper_space = TOP_BAR_PAD_TOP;
    ctx->scr.now.top.under_space = TOP_BAR_PAD_BOT;

    /* Calculate top bar size as percentage of parent size */
    obj_w = (obj_width(par) * TOP_BAR_WIDTH) / 100;
    obj_h = (obj_height(par) * TOP_BAR_HEIGHT) / 100;
    set_gobj_size(top_bar, obj_w, obj_h);

    /* Set background color and disable scroll */
    lv_obj_set_style_bg_color(top_bar, lv_color_hex(TOP_BAR_BG_COLOR), 0);
    lv_obj_clear_flag(top_bar, LV_OBJ_FLAG_SCROLLABLE);

    /* Enable horizontal scaling with padding */
    enable_scale_w(top_bar);
    // pad_w = (obj_width(par) * (TOP_BAR_PAD_LEFT + TOP_BAR_PAD_RIGHT)) / 100;
    pad_w = (TOP_BAR_PAD_LEFT + TOP_BAR_PAD_RIGHT);
    set_obj_scale_pad_w(top_bar, pad_w);

    /* Align top bar to top middle of parent with vertical offset */
    set_gobj_align_scale_xy(top_bar, par, LV_ALIGN_TOP_MID, \
                     0, \
                     ctx->scr.now.top.upper_space);

    return top_bar;
}
