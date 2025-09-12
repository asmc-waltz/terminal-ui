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

/**********************
 *      TYPEDEFS
 **********************/
typedef struct key_line {
    int32_t w;
    int32_t h;
    int32_t w_space;
    int32_t h_space;
    int32_t x_ofs;
    int32_t y_ofs;
} key_line;

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
static void keyboard_btn_handler(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);  // Get the button object
    lv_obj_t *par = lv_obj_get_parent(btn);
    g_obj *gobj = NULL;

    gobj = btn->user_data;
    LOG_TRACE("ID %d: button clicked", gobj->id);
    lv_obj_t * btn_label = lv_obj_get_child(btn, 0);
    lv_label_set_text(btn_label, "X");

}
lv_obj_t * sf_create_keyboard_line(lv_obj_t *par, key_line *line, const char * const *keys, int32_t line_size)
{
    lv_obj_t *aln_btn = NULL;
    lv_obj_t *next_btn = NULL;
    lv_obj_t *lbl = NULL;
    int8_t start_newline = 1;
    

    aln_btn = par;

    for (int8_t i=0; i < line_size; i++) {
        if (strcmp(keys[i], "\n1") == 0) {
            line->x_ofs = (((g_obj *)par->user_data)->pos.w*7)/100;
            line->y_ofs = (((g_obj *)par->user_data)->pos.h*28)/100;
            start_newline = 1;
            aln_btn = par;
            continue;
        } else if (strcmp(keys[i], "\n2") == 0) {
            line->x_ofs = (((g_obj *)par->user_data)->pos.w*17)/100;
            line->y_ofs = (((g_obj *)par->user_data)->pos.h*52)/100;
            start_newline = 1;
            aln_btn = par;
            continue;
        }
        next_btn = gf_create_btn(par, keys[i]);

        gf_gobj_set_size(next_btn, line->w, line->h);
        lv_obj_set_style_bg_color(next_btn, lv_color_hex(0xffffff), 0);
        lv_obj_clear_flag(next_btn, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_event_cb(next_btn, keyboard_btn_handler, LV_EVENT_CLICKED, next_btn->user_data);
        if (start_newline) {
            gf_gobj_align_to(next_btn, aln_btn, LV_ALIGN_TOP_LEFT, line->x_ofs, line->y_ofs);
            start_newline = 0;
        } else
            gf_gobj_align_to(next_btn, aln_btn, LV_ALIGN_OUT_RIGHT_MID, line->w_space, 0);

        lbl = gf_create_text(next_btn, keys[i], 10, 15, keys[i]);
        lv_obj_set_style_text_color(lbl, lv_color_hex(0x000000), 0);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_26, 0);
        aln_btn = next_btn;
    }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t *create_keyboard(lv_obj_t *par, const char *name, \
                          int32_t keyboard_w, int32_t keyboard_h)
{
    lv_obj_t *lobj = NULL;
    g_obj *gobj;

    lobj = gf_create_box(par, name);
    if (!lobj) {
        return NULL;
    }

    gf_gobj_set_size(lobj, keyboard_w, keyboard_h);
    lv_obj_set_style_bg_color(lobj, lv_color_hex(0xe6e6FF), 0);
    gf_gobj_align_to(lobj, par, LV_ALIGN_BOTTOM_MID, 0, -10);

    gobj = lobj->user_data;

    key_line line_first = {
        .w = (keyboard_w*8)/100,
        .h = (keyboard_h*20)/100,
        .w_space = (keyboard_w*2)/100,
        .h_space = (keyboard_h*4)/100,
        .x_ofs = (keyboard_w*2)/100,
        .y_ofs = (keyboard_h*4)/100,
    };

    const char * const line_1[] = {"Q\n", "W\n", "E\n", "R\n", "T\n", "Y\n", "U\n", "I\n", "O\n", "P\n", "\n1", \
                                   "A\n", "S\n", "D\n", "F\n", "G\n", "H\n", "J\n", "K\n", "L\n", "\n2", \
                                    "Z\n", "X\n", "C\n", "V\n", "B\n", "N\n", "M\n"};

    sf_create_keyboard_line(lobj, &line_first, line_1, 28);

    return lobj;
}
