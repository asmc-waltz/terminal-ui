/**
 * @file keyboard.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL defined locally will override the global setting in this file"
#endif
#include <log.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <lvgl.h>
#include <list.h>
#include <ui/ui_core.h>

/*********************
 *      DEFINES
 *********************/
#define KEY_PADDING 10

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    KEY_TYPE_CHAR,
    KEY_TYPE_SHIFT,
    KEY_TYPE_DELETE,
    KEY_TYPE_MODE,
    KEY_TYPE_SPACE,
    KEY_TYPE_ENTER,
    KEY_TYPE_NEWLINE
} key_type;

typedef struct key_def {
    const char *label;
    key_type type;
} key_def;

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
static void keyboard_key_handler(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);  // Get the button object
    lv_obj_t *par = lv_obj_get_parent(btn);
    g_obj *gobj = NULL;

    gobj = btn->user_data;
    LOG_TRACE("ID %d: button clicked", gobj->id);
    lv_obj_t * btn_label = lv_obj_get_child(btn, 0);
    lv_label_set_text(btn_label, "X");

}

static lv_obj_t *keyboard_create_container(lv_obj_t *par, const char *name, \
                                           int32_t keyboard_w, int32_t keyboard_h)
{
    lv_obj_t *cont = gf_create_box(par, name);
    if (!cont)
        return NULL;

    gf_gobj_set_size(cont, keyboard_w, keyboard_h);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0xe6e6FF), 0);
    gf_gobj_align_to(cont, par, LV_ALIGN_BOTTOM_MID, 0, -10);

    return cont;
}

static int32_t keyboard_calculate_key_width(int32_t keyboard_w)
{
    return (keyboard_w - (KEY_PADDING * 11)) / 10;
}

static int32_t keyboard_get_key_width(const key_def *key, int32_t cur_line, \
                                      int32_t base_width, int32_t keyboard_w)
{
    if (cur_line == 3) { /* Hàng 3 có shift và delete */
        if (key->type == KEY_TYPE_SHIFT || key->type == KEY_TYPE_DELETE)
            return base_width * 1.5;
    } else if (cur_line == 4) { /* Hàng cuối */
        if (key->type == KEY_TYPE_MODE || key->type == KEY_TYPE_ENTER)
            return base_width * 2;
        else if (key->type == KEY_TYPE_SPACE)
            return keyboard_w - (base_width * 4) - (KEY_PADDING * 4);
    }
    return base_width;
}

static lv_obj_t *keyboard_create_key(lv_obj_t *parent, const key_def *key, \
                                     int32_t w, int32_t h)
{
    lv_obj_t *btn = gf_create_btn(parent, key->label);
    if (!btn)
        return NULL;

    gf_gobj_set_size(btn, w, h);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xffffff), 0);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(btn, keyboard_key_handler, LV_EVENT_CLICKED, btn->user_data);

    lv_obj_t *lbl = gf_create_text(btn, key->label, 10, 15, key->label);
    lv_obj_set_style_text_color(lbl, lv_color_hex(0x000000), 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_26, 0);

    return btn;
}

static void keyboard_render(lv_obj_t *container, const key_def *map, int32_t map_size, \
                             int32_t keyboard_w, int32_t keyboard_h)
{
    int32_t line_count = 4;
    int32_t line_height = keyboard_h / line_count;
    int32_t base_width = keyboard_calculate_key_width(keyboard_w);

    lv_obj_t *aln_btn = container;
    lv_obj_t *next_btn = NULL;
    int32_t cur_line = 1;
    int32_t y_ofs = 0;
    int32_t x_pos = 0;

    for (int32_t i = 0; i < map_size; i++) {
        if (map[i].type == KEY_TYPE_NEWLINE) {
            cur_line++;
            y_ofs += line_height;
            aln_btn = container;
            x_pos = 0;
            continue;
        }

        int32_t w = keyboard_get_key_width(&map[i], cur_line, base_width, keyboard_w);

        next_btn = keyboard_create_key(container, &map[i], w, line_height - KEY_PADDING);
        if (!next_btn)
            continue;

        if (x_pos == 0)
            gf_gobj_align_to(next_btn, aln_btn, LV_ALIGN_TOP_LEFT, KEY_PADDING, y_ofs);
        else
            gf_gobj_align_to(next_btn, aln_btn, LV_ALIGN_OUT_RIGHT_MID, KEY_PADDING, 0);

        aln_btn = next_btn;
        x_pos++;
    }
}
/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t *gf_create_keyboard(lv_obj_t *par, const char *name, \
                             int32_t keyboard_w, int32_t keyboard_h)
{
    static const key_def key_map[] = {
        {"Q", KEY_TYPE_CHAR}, {"W", KEY_TYPE_CHAR}, {"E", KEY_TYPE_CHAR},
        {"R", KEY_TYPE_CHAR}, {"T", KEY_TYPE_CHAR}, {"Y", KEY_TYPE_CHAR},
        {"U", KEY_TYPE_CHAR}, {"I", KEY_TYPE_CHAR}, {"O", KEY_TYPE_CHAR},
        {"P", KEY_TYPE_CHAR}, {"\n", KEY_TYPE_NEWLINE},

        {"A", KEY_TYPE_CHAR}, {"S", KEY_TYPE_CHAR}, {"D", KEY_TYPE_CHAR},
        {"F", KEY_TYPE_CHAR}, {"G", KEY_TYPE_CHAR}, {"H", KEY_TYPE_CHAR},
        {"J", KEY_TYPE_CHAR}, {"K", KEY_TYPE_CHAR}, {"L", KEY_TYPE_CHAR},
        {"\n", KEY_TYPE_NEWLINE},

        {"Shift", KEY_TYPE_SHIFT}, {"Z", KEY_TYPE_CHAR}, {"X", KEY_TYPE_CHAR},
        {"C", KEY_TYPE_CHAR}, {"V", KEY_TYPE_CHAR}, {"B", KEY_TYPE_CHAR},
        {"N", KEY_TYPE_CHAR}, {"M", KEY_TYPE_CHAR}, {"Delete", KEY_TYPE_DELETE},
        {"\n", KEY_TYPE_NEWLINE},

        {"123", KEY_TYPE_MODE}, {"Space", KEY_TYPE_SPACE}, {"Enter", KEY_TYPE_ENTER}
    };

    int32_t key_map_size = sizeof(key_map) / sizeof(key_map[0]);

    lv_obj_t *container = keyboard_create_container(par, name, keyboard_w, keyboard_h);
    if (!container)
        return NULL;

    keyboard_render(container, key_map, key_map_size, keyboard_w, keyboard_h);

    return container;
}
