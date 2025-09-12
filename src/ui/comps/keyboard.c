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
#include <ui/comps.h>

/*********************
 *      DEFINES
 *********************/
#define KEYBOARD_BG_COLOR               0xE6E6FF

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

typedef enum {
    T_CHAR,
    T_NUM,
    T_SYM,
    T_SHIFT,
    T_DELETE,
    T_MODE,
    T_SPACE,
    T_ENTER,
    T_NEWLINE
} k_type;

typedef struct {
    const char* label;
    k_type type;
} k_def;

typedef struct {
    const char* name;
    const k_def *map;
    int32_t size;
} kb_def;
/**********************
 *  STATIC VARIABLES
 **********************/
static const k_def map_ABC[] = {
    {"Q", T_CHAR}, {"W", T_CHAR}, {"E", T_CHAR}, {"R", T_CHAR}, {"T", T_CHAR}, \
    {"Y", T_CHAR}, {"U", T_CHAR}, {"I", T_CHAR}, {"O", T_CHAR}, {"P", T_CHAR}, \
    {"\n", T_NEWLINE}, \

    {"A", T_CHAR}, {"S", T_CHAR}, {"D", T_CHAR}, {"F", T_CHAR}, {"G", T_CHAR}, \
    {"H", T_CHAR}, {"J", T_CHAR}, {"K", T_CHAR}, {"L", T_CHAR}, \
    {"\n", T_NEWLINE}, \

    {"Shift", T_SHIFT}, {"Z", T_CHAR}, {"X", T_CHAR}, {"C", T_CHAR}, \
    {"V", T_CHAR}, {"B", T_CHAR}, {"N", T_CHAR}, {"M", T_CHAR}, \
    {"Delete", T_DELETE}, {"\n", T_NEWLINE},

    {"123", T_MODE}, {"_____", T_SPACE}, {"Enter", T_ENTER}
};

static const k_def map_abc[] = {
    {"q", T_CHAR}, {"w", T_CHAR}, {"e", T_CHAR}, {"r", T_CHAR}, {"t", T_CHAR}, \
    {"y", T_CHAR}, {"u", T_CHAR}, {"i", T_CHAR}, {"o", T_CHAR}, {"p", T_CHAR}, \
    {"\n", T_NEWLINE}, \

    {"a", T_CHAR}, {"s", T_CHAR}, {"d", T_CHAR}, {"f", T_CHAR}, {"g", T_CHAR}, \
    {"h", T_CHAR}, {"j", T_CHAR}, {"k", T_CHAR}, {"l", T_CHAR}, \
    {"\n", T_NEWLINE}, \

    {"Shift", T_SHIFT}, {"z", T_CHAR}, {"x", T_CHAR}, {"c", T_CHAR}, \
    {"v", T_CHAR}, {"b", T_CHAR}, {"n", T_CHAR}, {"m", T_CHAR}, \
    {"Delete", T_DELETE}, {"\n", T_NEWLINE},

    {"123", T_MODE}, {"_____", T_SPACE}, {"Enter", T_ENTER}
};

static const k_def map_number[] = {
    {"1", T_NUM}, {"2", T_NUM}, {"3", T_NUM}, {"4", T_NUM}, {"5", T_NUM}, \
    {"6", T_NUM}, {"7", T_NUM}, {"8", T_NUM}, {"9", T_NUM}, {"0", T_NUM}, \
    {"\n", T_NEWLINE}, \

    {"-", T_SYM}, {"/", T_SYM}, {":", T_SYM}, {";", T_SYM}, {"(", T_SYM}, \
    {")", T_SYM}, {"&", T_SYM}, {"@", T_SYM}, {"\"", T_SYM}, \
    {"\n", T_NEWLINE}, \

    {"+-=", T_SHIFT}, {".", T_SYM}, {",", T_SYM}, {"?", T_SYM}, \
    {"!", T_SYM}, {"'", T_SYM},
    {"Delete", T_DELETE}, {"\n", T_NEWLINE},

    {"ABC", T_MODE}, {"_____", T_SPACE}, {"Enter", T_ENTER}
};

static const k_def map_symbol[] = {
    {"[", T_SYM}, {"]", T_SYM}, {"{", T_SYM}, {"}", T_SYM}, {"#", T_SYM}, \
    {"%", T_SYM}, {"^", T_SYM}, {"*", T_SYM}, {"+", T_SYM}, {"=", T_SYM}, \
    {"\n", T_NEWLINE}, \

    {"_", T_SYM}, {"\\", T_SYM}, {"|", T_SYM}, {"~", T_SYM}, {"<", T_SYM}, \
    {">", T_SYM}, {"$", T_SYM}, \
    {"\n", T_NEWLINE}, \

    {"123", T_SHIFT}, {".", T_SYM}, {",", T_SYM}, {"?", T_SYM}, \
    {"!", T_SYM}, {"'", T_SYM},
    {"Delete", T_DELETE}, {"\n", T_NEWLINE},

    {"ABC", T_MODE}, {"_____", T_SPACE}, {"Enter", T_ENTER}
};


static const kb_def kb_maps[] = {
    {"ABC", map_ABC, sizeof(map_ABC) / sizeof(map_ABC[0])},
    {"abc", map_abc, sizeof(map_abc) / sizeof(map_abc[0])},
    {"123", map_number, sizeof(map_number) / sizeof(map_number[0])},
    {"@*#", map_symbol, sizeof(map_symbol) / sizeof(map_symbol[0])},
};
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
static void dump_key_map(const kb_def *kb)
{
    int8_t cnt;
    for (cnt = 0; cnt < kb->size; cnt++) {
        LOG_INFO("Keyboard %s: index[%d] character[%s] type[%d]", \
                 kb->name, cnt, kb->map[cnt].label, kb->map[cnt].type);
    }
}

static void dump_all_maps(void)
{
    int8_t map_cnt, i;

    map_cnt = sizeof(kb_maps) / sizeof(kb_def);
    LOG_INFO("[%d] keyboard are available", map_cnt);
    for (i = 0; i < map_cnt; i++) {
        dump_key_map(&kb_maps[i]);
    }
}

lv_obj_t *create_keyboard_containter(lv_obj_t *par)
{
    lv_obj_t *cont;
    int32_t obj_w, obj_h;

    if (!par)
        return NULL;

    /* Create container box for the keyboard and all button */
    cont = gf_create_box(par, KEYBOAR_NAME);
    if (!cont)
        return NULL;

    /* Calculate setting container size as percentage of parent size */
    obj_w = calc_pixels(obj_width(par), KEYBOARD_WIDTH);
    obj_h = calc_pixels(obj_height(par), HOR_KEYBOARD_HEIGHT);

    gf_gobj_set_size(cont, obj_w, obj_h);
    lv_obj_set_style_bg_color(cont, lv_color_hex(KEYBOARD_BG_COLOR), 0);
    gf_gobj_align_to(cont, par, LV_ALIGN_BOTTOM_MID, 0,\
                     -calc_pixels(obj_height(par), KEYBOARD_BOT_PAD));

    gf_obj_scale_enable_w(cont);
    gf_obj_scale_set_pad_w(cont, calc_pixels(obj_width(par), \
                           (KEYBOARD_PAD_RIGHT + KEYBOARD_PAD_LEFT)));
    return cont;
}

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

    dump_all_maps();

    return lobj;
}
