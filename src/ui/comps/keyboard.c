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
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include <lvgl.h>
#include <list.h>
#include <ui/ui_core.h>
#include <ui/comps.h>

/*********************
 *      DEFINES
 *********************/
#define KEYBOARD_BG_COLOR               0xADBACC
#define KEYBOARD_KEY_COLOR              0xFFFFFF
#define KEYBOARD_KEY_TEXT_COLOR         0x000000
#define KEYBOARD_LINE                   4
#define KEYBOARD_LINE_PAD_TOP           2       // %
#define KEYBOARD_LINE_PAD_BOT           2       // %
#define KEYBOARD_LINE_HEIGHT            ((100 - (KEYBOARD_LINE * \
                                         (KEYBOARD_LINE_PAD_TOP + \
                                         KEYBOARD_LINE_PAD_BOT))) / \
                                         KEYBOARD_LINE)     // %
#define KEY_PAD_LEFT                    1       // %
#define KEY_PAD_RIGHT                   1       // %
#define KEY_FIRST_LINE                  10      // Number of the first line keys
#define KEY_CHAR_WIDTH                  ((100 - (KEY_FIRST_LINE * \
                                         (KEY_PAD_LEFT + KEY_PAD_RIGHT))) / \
                                         KEY_FIRST_LINE)    // %
#define KEY_SPACE_WIDTH                 ((5 * KEY_CHAR_WIDTH) + \
                                         (4 * (KEY_PAD_LEFT + \
                                         KEY_PAD_RIGHT)))
#define KEY_FN_WIDTH                    ((KEY_CHAR_WIDTH * 17) / 10) // %
#define KEY_ENTER_WIDTH                 ((KEY_CHAR_WIDTH * 200) / 100 + \
                                         (1 * (KEY_PAD_LEFT + \
                                         KEY_PAD_RIGHT)))
#define KEY_MODE_WIDTH                  ((KEY_CHAR_WIDTH * 127) / 100) // %
#define KEY_ARROW_WIDTH                 ((KEY_CHAR_WIDTH * 90) / 100) // %

/**********************
 *      TYPEDEFS
 **********************/
typedef struct kb_ctx {
    int32_t l_pad_top;
    int32_t l_pad_bot;
    int32_t k_pad_left;
    int32_t k_pad_right;
    int32_t key_com_h;
    int32_t key_com_w;
    int32_t key_space_w;
    int32_t key_mode_w;
    int32_t key_enter_w;
    int32_t key_arrow_w;
    int32_t key_fn_w;
} kb_size_ctx;

typedef enum {
    // Key types
    T_KEY_TYPE,
    T_CHAR,
    T_NUM,
    T_SYM,
    T_SHIFT,
    T_DELETE,
    T_MODE,
    T_SPACE,
    T_ENTER,
    T_ARROW,
    // Key layout flags
    T_KEY_LAYOUT_FLAG,
    T_HOLDER,
    T_NEWLINE,
    T_END
} k_type;

typedef struct {
    const char* label;
    k_type type;
} key_def;

typedef struct {
    const char* name;
    const key_def *key;
    int32_t size;
} keyboard_def;

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static int32_t set_keyboard_mode(const key_def *key);

/**********************
 *  STATIC VARIABLES
 **********************/
static const keyboard_def *act_map;

static const key_def key_abc[] = {
    {"line_01", T_HOLDER}, \
    {"q", T_CHAR}, {"w", T_CHAR}, {"e", T_CHAR}, {"r", T_CHAR}, {"t", T_CHAR}, \
    {"y", T_CHAR}, {"u", T_CHAR}, {"i", T_CHAR}, {"o", T_CHAR}, {"p", T_CHAR}, \
    {"\n", T_NEWLINE}, \

    {"line_02", T_HOLDER}, \
    {"a", T_CHAR}, {"s", T_CHAR}, {"d", T_CHAR}, {"f", T_CHAR}, {"g", T_CHAR}, \
    {"h", T_CHAR}, {"j", T_CHAR}, {"k", T_CHAR}, {"l", T_CHAR}, \
    {"\n", T_NEWLINE}, \

    {"line_03", T_HOLDER}, \
    {"Shift", T_SHIFT}, \
    {"z", T_CHAR}, {"x", T_CHAR}, {"c", T_CHAR}, {"v", T_CHAR}, {"b", T_CHAR}, \
    {"n", T_CHAR}, {"m", T_CHAR}, \
    {"Del", T_DELETE}, \
    {"\n", T_NEWLINE},

    {"line_04", T_HOLDER}, \
    {"123", T_MODE}, {"<", T_ARROW}, {">", T_ARROW}, {" ", T_SPACE}, \
    {"Enter", T_ENTER},
    {"End", T_END}
};

static const key_def key_ABC[] = {
    {"line_01", T_HOLDER}, \
    {"Q", T_CHAR}, {"W", T_CHAR}, {"E", T_CHAR}, {"R", T_CHAR}, {"T", T_CHAR}, \
    {"Y", T_CHAR}, {"U", T_CHAR}, {"I", T_CHAR}, {"O", T_CHAR}, {"P", T_CHAR}, \
    {"\n", T_NEWLINE}, \

    {"line_02", T_HOLDER}, \
    {"A", T_CHAR}, {"S", T_CHAR}, {"D", T_CHAR}, {"F", T_CHAR}, {"G", T_CHAR}, \
    {"H", T_CHAR}, {"J", T_CHAR}, {"K", T_CHAR}, {"L", T_CHAR}, \
    {"\n", T_NEWLINE}, \

    {"line_03", T_HOLDER}, \
    {"Shift", T_SHIFT}, \
    {"Z", T_CHAR}, {"X", T_CHAR}, {"C", T_CHAR}, {"V", T_CHAR}, {"B", T_CHAR}, \
    {"N", T_CHAR}, {"M", T_CHAR}, \
    {"Del", T_DELETE}, \
    {"\n", T_NEWLINE},

    {"line_04", T_HOLDER}, \
    {"123", T_MODE}, {"<", T_ARROW}, {">", T_ARROW}, {" ", T_SPACE}, \
    {"Enter", T_ENTER},
    {"End", T_END}
};

static const key_def key_number[] = {
    {"line_01", T_HOLDER}, \
    {"1", T_NUM}, {"2", T_NUM}, {"3", T_NUM}, {"4", T_NUM}, {"5", T_NUM}, \
    {"6", T_NUM}, {"7", T_NUM}, {"8", T_NUM}, {"9", T_NUM}, {"0", T_NUM}, \
    {"\n", T_NEWLINE}, \

    {"line_02", T_HOLDER}, \
    {"-", T_SYM}, {"/", T_SYM}, {":", T_SYM}, {";", T_SYM}, {"(", T_SYM}, \
    {")", T_SYM}, {"`", T_SYM}, {"&", T_SYM}, {"@", T_SYM}, \
    {"\n", T_NEWLINE}, \

    {"line_03", T_HOLDER}, \
    {"#+=", T_SHIFT}, \
    {".", T_SYM}, {",", T_SYM}, {"?", T_SYM}, {"!", T_SYM}, {"\"", T_SYM}, \
    {"'", T_SYM}, {"*", T_SYM}, \
    {"Del", T_DELETE}, \
    {"\n", T_NEWLINE},

    {"line_04", T_HOLDER}, \
    {"ABC", T_MODE}, {"<", T_ARROW}, {">", T_ARROW}, {" ", T_SPACE}, \
    {"Enter", T_ENTER},
    {"End", T_END}
};

static const key_def key_symbol[] = {
    {"line 01", T_HOLDER}, \
    {"1", T_NUM}, {"2", T_NUM}, {"3", T_NUM}, {"4", T_NUM}, {"5", T_NUM}, \
    {"6", T_NUM}, {"7", T_NUM}, {"8", T_NUM}, {"9", T_NUM}, {"0", T_NUM}, \
    {"\n", T_NEWLINE}, \

    {"line 02", T_HOLDER}, \
    {"[", T_SYM}, {"]", T_SYM}, {"\{", T_SYM}, {"\}", T_SYM}, {"#", T_SYM},\
    {"%", T_SYM}, {"^", T_SYM}, {"+", T_SYM}, {"=", T_SYM},  \
    {"\n", T_NEWLINE}, \

    {"line 03", T_HOLDER}, \
    {"#+=", T_SHIFT}, \
    {"_", T_SYM}, {"\\", T_SYM}, {"|", T_SYM}, {"~", T_SYM}, {"<", T_SYM}, \
    {">", T_SYM}, {"$", T_SYM}, \
    {"Del", T_DELETE}, \
    {"\n", T_NEWLINE},

    {"line 04", T_HOLDER}, \
    {"ABC", T_MODE}, {"<", T_ARROW}, {">", T_ARROW}, {" ", T_SPACE}, \
    {"Enter", T_ENTER},
    {"End", T_END}
};

static const keyboard_def kb_maps[] = {
    {"abc", key_abc, sizeof(key_abc) / sizeof(key_abc[0])},
    {"ABC", key_ABC, sizeof(key_ABC) / sizeof(key_ABC[0])},
    {"123", key_number, sizeof(key_number) / sizeof(key_number[0])},
    {"@*#", key_symbol, sizeof(key_symbol) / sizeof(key_symbol[0])},
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void dump_key_map(const keyboard_def *kb)
{
    int8_t cnt;
    for (cnt = 0; cnt < kb->size; cnt++) {
        LOG_INFO("Keyboard %s: index[%d] character[%s] type[%d]", \
                 kb->name, cnt, kb->key[cnt].label, kb->key[cnt].type);
    }
}

static void dump_all_maps(void)
{
    int8_t map_cnt, i;

    map_cnt = sizeof(kb_maps) / sizeof(keyboard_def);
    LOG_INFO("[%d] keyboard are available", map_cnt);
    for (i = 0; i < map_cnt; i++) {
        dump_key_map(&kb_maps[i]);
    }
}

static void kb_key_cb(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);
    const key_def *key_data;

    key_data = (const key_def *)get_gobj_data(btn);
    if (!key_data) {
        LOG_ERROR("Unable to get key internal data");
        return;
    }

    LOG_TRACE("KB: key ID[%d] is pressed, text data: (%s)", \
              get_gobj(btn)->id, key_data->label);


    switch (key_data->type) {
    case T_CHAR:
    case T_NUM:
    case T_SYM:
        break;
    case T_SPACE:
        break;
    case T_ENTER:
        break;
    case T_DELETE:
        break;
    case T_MODE:
    case T_SHIFT:
        set_keyboard_mode(key_data);
        break;
    case T_ARROW:
        break;
    default:
        break;
    }
}

static lv_obj_t *create_key(lv_obj_t *par, const key_def *key)
{
    lv_obj_t *btn, *lbl;

    if (key->type <= T_KEY_TYPE || key->type >= T_KEY_LAYOUT_FLAG) {
        LOG_ERROR("KB: unable to create key, invalid type %d", key->type);
        return NULL;
    }

    btn = gf_create_btn(par, key->label);
    if (!btn)
        return NULL;
    lv_obj_set_style_bg_color(btn, lv_color_hex(KEYBOARD_KEY_COLOR), 0);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(btn, 0, 0);
    lv_obj_set_style_pad_gap(btn, 0, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_add_event_cb(btn, kb_key_cb, LV_EVENT_CLICKED, btn->user_data);

    lbl = gf_create_text(btn, NULL, 10, 10, key->label);
    if (!lbl)
        return NULL;
    lv_obj_set_style_text_color(lbl, lv_color_hex(KEYBOARD_KEY_TEXT_COLOR), 0);
    lv_obj_set_style_text_font(lbl, KEYBOARD_CHAR_FONTS, 0);

    return btn;
}

void set_key_size(lv_obj_t *key, int8_t type, kb_size_ctx *size)
{
    int32_t key_w = 0;

    switch (type) {
    case T_CHAR:
    case T_NUM:
    case T_SYM:
        key_w = size->key_com_w;
        break;
    case T_SPACE:
        key_w = size->key_space_w;
        break;
    case T_ENTER:
        key_w = size->key_enter_w;
        break;
    case T_SHIFT:
    case T_DELETE:
        key_w = size->key_fn_w;
        break;
    case T_MODE:
        key_w = size->key_mode_w;
        break;
    case T_ARROW:
        key_w = size->key_arrow_w;
        break;
    default:
        break;
    }

    gf_gobj_set_size(key, key_w, size->key_com_h);
}

/*
 * The keyboard contains multiple keys inside. Each key will be generated
 * with a specific size and alignment based on the parent size and line
 * padding. In addition, some special keys may have different sizes, and
 * all of them will be calculated in this function.
 *
 * The output data will be shared for both key layout and resize, whenever
 * the parent size is changed.
 */
int32_t calc_kb_size_data(lv_obj_t *par, kb_size_ctx *size)
{
    int32_t key_com_h, key_com_w, key_mode_w, key_space_w, key_enter_w;
    int32_t key_arrow_w, key_fn_w;
    int32_t l_pad_top, l_pad_bot, k_pad_left, k_pad_right;
    int32_t par_h, par_w;

    if (!par || !size)
        return -EINVAL;

    // TODO: Parent scale height and width ?
    // e.g. par_h = 250;
    //      par_w = 580;
    par_h = obj_height(par);
    par_w = obj_width(par);


    l_pad_top = calc_pixels(par_h, KEYBOARD_LINE_PAD_TOP);
    l_pad_bot = calc_pixels(par_h, KEYBOARD_LINE_PAD_BOT);

    k_pad_left = calc_pixels(par_w, KEY_PAD_LEFT);
    k_pad_right = calc_pixels(par_w, KEY_PAD_RIGHT);

    key_com_h = calc_pixels(par_h, KEYBOARD_LINE_HEIGHT);
    key_com_w = calc_pixels(par_w, KEY_CHAR_WIDTH);
    key_space_w = calc_pixels(par_w, KEY_SPACE_WIDTH);
    key_mode_w = calc_pixels(par_w, KEY_MODE_WIDTH);
    key_enter_w = calc_pixels(par_w, KEY_ENTER_WIDTH);
    key_arrow_w = calc_pixels(par_w, KEY_ARROW_WIDTH);
    key_fn_w = calc_pixels(par_w, KEY_FN_WIDTH);
    
    LOG_TRACE("KB: Parent: \tw[%d] - h[%d]", par_w, par_h);
    LOG_TRACE("KB: Key: \tPadding: top[%d] bot[%d] - left[%d] right[%d]", \
              l_pad_top, l_pad_bot, k_pad_left, k_pad_right);
    LOG_TRACE("KB: Key: \tSize: w[%d] h[%d]", key_com_w, key_com_h);
     
    size->l_pad_top = l_pad_top;
    size->l_pad_bot = l_pad_bot;
    size->k_pad_left = k_pad_left;
    size->k_pad_right = k_pad_right;
    size->key_com_h = key_com_h;
    size->key_com_w = key_com_w;
    size->key_space_w = key_space_w;
    size->key_mode_w = key_mode_w;
    size->key_enter_w = key_enter_w;
    size->key_arrow_w = key_arrow_w;
    size->key_fn_w = key_fn_w;

    return 0;
}

void set_line_box_size(lv_obj_t *par, lv_obj_t *line_box, kb_size_ctx *size, \
                       int32_t line_w)
{
    int32_t obj_h = 0;

    obj_h = size->l_pad_top + size->key_com_h + size->l_pad_bot;
    gf_gobj_set_size(line_box, line_w, obj_h);
}

lv_obj_t *create_line_box(lv_obj_t *par, kb_size_ctx *size, \
                          const key_def *box_info)
{
    lv_obj_t *line_box;

    line_box = gf_create_box(par, box_info->label);
    if (!line_box)
        return NULL;

    lv_obj_set_style_bg_opa(line_box, LV_OPA_0, 0);
    // lv_obj_set_style_bg_color(line_box, lv_color_hex(0xBDBDBD), 0);

    return line_box;
}

int32_t create_keys_layout(lv_obj_t *par, const keyboard_def *map)
{
    lv_obj_t *btn, *btn_aln;
    lv_obj_t *line_box;
    int8_t line_cnt = 0, i;
    int32_t line_h, line_w = 0;
    kb_size_ctx size;
    bool new_line = true;

    if (calc_kb_size_data(par, &size)) {
        LOG_ERROR("Unable to calculate keyboard child size");
        return -EINVAL;
    }

    line_h = size.l_pad_top + size.key_com_h + size.l_pad_bot;

    for (i = 0; i < map->size; i++) {
        LOG_TRACE("KB name [%s]: index[%d] character[%s] type[%d]", \
                   map->name, i, map->key[i].label, map->key[i].type);

        if (map->key[i].type == T_NEWLINE || map->key[i].type == T_END) {
            int32_t line_x_ofs = (obj_width(par) - line_w) / 2;
            int32_t line_y_ofs = (size.l_pad_top + (line_h * line_cnt));

            set_line_box_size(par, line_box, &size, line_w);
            line_w = 0;
            // Align the current line box before create the next one
            gf_gobj_align_to(line_box, par, LV_ALIGN_TOP_LEFT, \
                             line_x_ofs, line_y_ofs);
            LOG_TRACE("KB line box [%d]: alignment x %d - y %d", line_cnt, \
                      line_x_ofs, line_y_ofs);

            if (map->key[i].type == T_NEWLINE) {
                line_cnt++;
                new_line = true;
            }

            continue;
        } else if (map->key[i].type == T_HOLDER) {
            line_box = create_line_box(par, &size, &map->key[i]);
            if (!line_box)
                return -EINVAL;

            continue;
        }

        btn = create_key(line_box, &map->key[i]);
        if (!btn)
            return -EINVAL;

        if (new_line) {
            new_line = false;
            gf_gobj_align_to(btn, line_box, LV_ALIGN_TOP_LEFT, \
                             size.k_pad_left, size.l_pad_top);
        } else {
            gf_gobj_align_to(btn, btn_aln, LV_ALIGN_OUT_RIGHT_TOP, \
                             (size.k_pad_left + size.k_pad_right), 0);
        }

        // The previous button is used to align the next one
        btn_aln = btn;
        set_key_size(btn, map->key[i].type, &size);
        set_gobj_data(btn, &map->key[i]);
        line_w += size.k_pad_left + get_gobj(btn)->pos.w + size.k_pad_right;
    }

    return 0;
}

int32_t update_keys_layout(lv_obj_t *par, const keyboard_def *map)
{
    lv_obj_t *btn, *btn_aln;
    lv_obj_t *line_box = NULL;
    int8_t line_cnt = 0, i;
    int32_t line_h, line_w = 0;
    kb_size_ctx size;
    bool new_line = true;

    if (calc_kb_size_data(par, &size)) {
        LOG_ERROR("Unable to calculate keyboard child size");
        return -EINVAL;
    }

    line_h = size.l_pad_top + size.key_com_h + size.l_pad_bot;

    for (i = 0; i < map->size; i++) {
        LOG_TRACE("KB name [%s]: index[%d] character[%s] type[%d]", \
                   map->name, i, map->key[i].label, map->key[i].type);

        if (map->key[i].type == T_NEWLINE || map->key[i].type == T_END) {
            int32_t line_x_ofs = (obj_width(par) - line_w) / 2;
            int32_t line_y_ofs = (size.l_pad_top + (line_h * line_cnt));

            set_line_box_size(par, line_box, &size, line_w);
            line_w = 0;
            // Align the current line box before create the next one
            gf_gobj_align_to(line_box, par, LV_ALIGN_TOP_LEFT, \
                             line_x_ofs, line_y_ofs);

            LOG_TRACE("KB line box [%d]: alignment x %d - y %d", line_cnt, \
                      line_x_ofs, line_y_ofs);

            if (map->key[i].type == T_NEWLINE) {
                line_cnt++;
                new_line = true;
            }

            continue;
        } else if (map->key[i].type == T_HOLDER) {
            line_box = gf_get_obj_by_name(map->key[i].label, \
                                          &get_gobj(par)->child);
            // TODO:
            /************** SOMETHING WRONG AT THE END OF THIS ***************/
            // if (!line_box)
            //     LOG_ERROR("line box [%s] not found", map->key[i].label);
            //     return -EINVAL;
            /************** SOMETHING WRONG AT THE ABOVE OF THIS *************/
            get_gobj(line_box)->pos.rot = ROTATION_0;

            continue;
        }

        btn = gf_get_obj_by_name(map->key[i].label, \
                                 &get_gobj(line_box)->child);
        if (!btn) {
            LOG_ERROR("Key [%s] not found", map->key[i].label);
            continue;
        }

        if (new_line) {
            new_line = false;
            gf_gobj_align_to(btn, line_box, LV_ALIGN_TOP_LEFT, \
                             size.k_pad_left, size.l_pad_top);
        } else {
            gf_gobj_align_to(btn, btn_aln, LV_ALIGN_OUT_RIGHT_TOP, \
                             (size.k_pad_left + size.k_pad_right), 0);
        }

        // The previous button is used to align the next one
        btn_aln = btn;
        set_key_size(btn, map->key[i].type, &size);
        // Reset key configurations to the horizontal map.
        get_gobj(btn)->pos.rot = ROTATION_0;
        line_w += size.k_pad_left + get_gobj(btn)->pos.w + size.k_pad_right;
    }

    return 0;
}

static const keyboard_def *find_map_next(const key_def *key)
{
    const char *active_map;
    const keyboard_def *next_map = NULL;

    if (!key)
        return NULL;

    active_map = act_map->name;

    if (strcmp(active_map, "abc") == 0) {
        if (key->type == T_SHIFT) {
            next_map = &kb_maps[1];
        } else if (key->type == T_MODE) {
            next_map = &kb_maps[2];
        }
    } else if (strcmp(active_map, "ABC") == 0) {
        if (key->type == T_SHIFT) {
            next_map = &kb_maps[0];
        } else if (key->type == T_MODE) {
            next_map = &kb_maps[2];
        }
    } else if (strcmp(active_map, "123") == 0) {
        if (key->type == T_SHIFT) {
            next_map = &kb_maps[3];
        } else if (key->type == T_MODE) {
            next_map = &kb_maps[0];
        }
    } else if (strcmp(active_map, "@*#") == 0) {
        if (key->type == T_SHIFT) {
            next_map = &kb_maps[2];
        } else if (key->type == T_MODE) {
            next_map = &kb_maps[0];
        }
    } else {
        LOG_ERROR("The activated map name is invalid");
        next_map = &kb_maps[0];
    }

    if (next_map) {
        LOG_TRACE("Current map %s -> %s", active_map, next_map->name);
    } else {
        LOG_ERROR("New map is not found. Please check key maps");
    }

    return next_map;
}

static int32_t change_keyboard_mode(lv_obj_t *par, const keyboard_def *map, \
                      const keyboard_def *next_map)
{
    lv_obj_t *btn;
    lv_obj_t *line_box = NULL;
    int8_t i;

    if (!map || !next_map)
        return -EINVAL;

    for (i = 0; i < map->size; i++) {
        LOG_TRACE("KB name [%s]: index[%d] character[%s] type[%d]", \
                   next_map->name, i, next_map->key[i].label, \
                   next_map->key[i].type);

        if (map->key[i].type == T_NEWLINE || map->key[i].type == T_END) {
            continue;
        } else if (map->key[i].type == T_HOLDER) {
            line_box = gf_get_obj_by_name(map->key[i].label, \
                                          &get_gobj(par)->child);
            // TODO:
            /************** SOMETHING WRONG AT THE END OF THIS ***************/
            // if (!line_box)
            //     LOG_ERROR("line box [%s] not found", map->key[i].label);
            //     return -EINVAL;
            /************** SOMETHING WRONG AT THE ABOVE OF THIS *************/
            continue;
        }

        btn = gf_get_obj_by_name(map->key[i].label, \
                                 &get_gobj(line_box)->child);
        if (!btn) {
            LOG_ERROR("Key [%s] not found", map->key[i].label);
            continue;
        }

        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "%s", next_map->key[i].label);
        set_gobj_data(btn, &next_map->key[i]);
    }

    act_map = next_map;

    return 0;
}

static int32_t set_keyboard_mode(const key_def *key)
{
    lv_obj_t *kb;
    const keyboard_def *map;
    int32_t ret;

    kb = gf_get_obj_by_name(KEYBOAR_NAME, \
                                   &get_gobj(lv_screen_active())->child);
    if (!kb) {
        LOG_ERROR("Keyboard [%s] not found", "screens.common");
        return -EINVAL;
    }

    map = find_map_next(key);
    if (!map)
        return -EINVAL;


    ret = change_keyboard_mode(kb, &kb_maps[0], map);
    if (ret) {
        LOG_ERROR("Unable to switch keyboard map, ret %d", ret);
    }

    return ret;
}

int32_t pre_rotation_redraw_kb_layout(lv_obj_t *kb)
{
    lv_obj_t *par;
    int32_t scr_rot;
    int32_t obj_w = 0, obj_h = 0;

    par = lv_obj_get_parent(kb);
    if (!par)
        return -EINVAL;

    // Keyboard size is based on rotation is ROTATION_0
    scr_rot = g_get_scr_rot_dir();
    if (scr_rot == ROTATION_0 || scr_rot == ROTATION_180) {
        obj_w = calc_pixels(obj_width(par), KEYBOARD_WIDTH);
        obj_h = calc_pixels(obj_height(par), HOR_KEYBOARD_HEIGHT);
    } else if (scr_rot == ROTATION_90 || scr_rot == ROTATION_270) {
        obj_w = calc_pixels(obj_height(par), KEYBOARD_WIDTH);
        obj_h = calc_pixels(obj_width(par), VER_KEYBOARD_HEIGHT);
    }

    // Reset all keyboard configurations to the horizontal layout.
    gf_gobj_set_size(kb, obj_w, obj_h);
    get_gobj(kb)->pos.rot = ROTATION_0;
    gf_gobj_align_to(kb, par, LV_ALIGN_BOTTOM_MID, 0,\
                     -calc_pixels(obj_height(par), KEYBOARD_BOT_PAD));

    // TODO: map?
    const keyboard_def *map = &kb_maps[0];
    update_keys_layout(kb, map);

    return 0;
}

    // Resize parent
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

    get_gobj(cont)->scale.pre_rot_redraw_cb = pre_rotation_redraw_kb_layout;

    return cont;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t *create_keyboard(lv_obj_t *par)
{
    lv_obj_t *kb;
    const keyboard_def *map = &kb_maps[0];
    int32_t ret;

    if (act_map) {
        LOG_WARN("Keyboard already active, please recall the previous one");

    }

    kb = create_keyboard_containter(par);
    if (!kb)
        return NULL;

    ret = create_keys_layout(kb, map);
    if (ret) {
        LOG_ERROR("Create keyboard failed %d, remove container ret %d", ret, \
                   gf_remove_obj_and_child_by_name(KEYBOAR_NAME, \
                                            &get_gobj(par)->child));
        return NULL;
    }

    act_map = map;

    if (g_get_scr_rot_dir() != ROTATION_0) {
        refresh_obj_tree_layout(kb->user_data);
    }

    return kb;
}
