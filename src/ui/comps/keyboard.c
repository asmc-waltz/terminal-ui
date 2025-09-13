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

#include <lvgl.h>
#include <list.h>
#include <ui/ui_core.h>
#include <ui/comps.h>

/*********************
 *      DEFINES
 *********************/
#define KEYBOARD_BG_COLOR               0xE6E6FF
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
#define KEY_SPACE_WIDTH                 ((5 * KEY_CHAR_WIDTH) + (4 * (KEY_PAD_LEFT + \
                                         KEY_PAD_RIGHT)))
#define KEY_MODE_WIDTH                  ((KEY_CHAR_WIDTH * 14) / 10)  // % (Magic)
#define KEY_ENTER_WIDTH                 ((2 * KEY_MODE_WIDTH) + (1 * (KEY_PAD_LEFT + \
                                         KEY_PAD_RIGHT)))

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
} kb_size_ctx;

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

static void kb_btn_cb(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);  // Get the button object
    lv_obj_t *par = lv_obj_get_parent(btn);
    g_obj *gobj = NULL;

    gobj = btn->user_data;
    LOG_TRACE("ID %d: button clicked", gobj->id);
    lv_obj_t * btn_label = lv_obj_get_child(btn, 0);
    lv_label_set_text(btn_label, "X");

}

static lv_obj_t *create_key(lv_obj_t *par, const k_def *key)
{
    lv_obj_t *btn, *lbl;

    btn = gf_create_btn(par, key->label);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xffffff), 0);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(btn, kb_btn_cb, LV_EVENT_CLICKED, btn->user_data);

    lbl = gf_create_text(btn, NULL, 10, 10, key->label);
    lv_obj_set_style_text_color(lbl, lv_color_hex(0x000000), 0);
    lv_obj_set_style_text_font(lbl, KEYBOARD_CHAR_FONTS, 0);

    return btn;
}

void set_key_size(lv_obj_t *key, int8_t type, kb_size_ctx *size)
{
    switch (type) {
    case T_CHAR:
    case T_NUM:
    case T_SYM:
        gf_gobj_set_size(key, size->key_com_w, size->key_com_h);
        break;
    case T_SPACE:
        gf_gobj_set_size(key, size->key_space_w, size->key_com_h);
        break;
    case T_ENTER:
        gf_gobj_set_size(key, size->key_enter_w, size->key_com_h);
        break;
    case T_SHIFT:
    case T_DELETE:
    case T_MODE:
        gf_gobj_set_size(key, size->key_mode_w, size->key_com_h);
        break;
    default:
        break;
    }
}

int32_t calc_kb_size_data(lv_obj_t *par, kb_size_ctx *size)
{
    int32_t key_com_h, key_com_w, key_mode_w, key_space_w, key_enter_w;
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
    
    LOG_INFO("Parent W %d - H %d", par_w, par_h);
    LOG_INFO("Key: \tpad: top[%d] bot[%d] - left[%d] right[%d] -\n" \
             "OBJ Size: w[%d] h[%d]",
             l_pad_top, l_pad_bot, k_pad_left, k_pad_right, \
             key_com_w, key_com_h);
     
    size->l_pad_top = l_pad_top;
    size->l_pad_bot = l_pad_bot;
    size->k_pad_left = k_pad_left;
    size->k_pad_right = k_pad_right;
    size->key_com_h = key_com_h;
    size->key_com_w = key_com_w;
    size->key_space_w = key_space_w;
    size->key_mode_w = key_mode_w;
    size->key_enter_w = key_enter_w;

    return 0;
}

int32_t create_key_layout(lv_obj_t *par, const kb_def *kb)
{
    lv_obj_t *btn, *btn_aln;
    int8_t line_cnt = 0, new_line = 0, i;
    int32_t ret;
    kb_size_ctx size;

    /* TESTING START ***************************************/
    ret = calc_kb_size_data(par, &size);
    if (ret) {
        LOG_ERROR("Unable to calculate the keyboard child size");
        return -EINVAL;
    }

    int32_t line_size = size.l_pad_top + size.key_com_h + size.l_pad_bot;

    for (i = 0; i < kb->size; i++) {
        LOG_INFO("Keyboard %s: index[%d] character[%s] type[%d]", \
                 kb->name, i, kb->map[i].label, kb->map[i].type);

        //------------
        if (kb->map[i].type == T_NEWLINE) {
            LOG_INFO("LINE KEY NUMBER DETECTED %d", i);
            line_cnt++;
            new_line = 1;
            continue;
        }
        //------------
        btn = create_key(par, &kb->map[i]);
        if (i == 0) {
            // The top padding must x2 Due to no upper line for the first line
            gf_gobj_align_to(btn, par, LV_ALIGN_TOP_LEFT, size.k_pad_left, \
                             size.l_pad_top * 2);
        } else if (new_line == 1) {
            new_line = 0;
            gf_gobj_align_to(btn, par, LV_ALIGN_TOP_LEFT, \
                             size.k_pad_left, \
                             (size.l_pad_top*2 + (line_size * line_cnt)));
        } else {

            gf_gobj_align_to(btn, btn_aln, LV_ALIGN_OUT_RIGHT_TOP, \
                             (size.k_pad_left + size.k_pad_right), 0);
        }

        btn_aln = btn;
        //------------
        set_key_size(btn, kb->map[i].type, &size);
    }

    /* TESTING END ***************************************/
}

int32_t update_keys_layout(lv_obj_t *par, const kb_def *layout)
{
    lv_obj_t *btn, *btn_aln;
    int8_t line_cnt = 0, new_line = 0, i;
    int32_t ret;
    int32_t line_size;
    kb_size_ctx size;

    ret = calc_kb_size_data(par, &size);
    if (ret) {
        LOG_ERROR("Unable to calculate keyboard child size");
        return -EINVAL;
    }

    line_size = size.l_pad_top + size.key_com_h + size.l_pad_bot;

    for (i = 0; i < layout->size; i++) {
        LOG_TRACE("Keyboard %s: index[%d] character[%s] type[%d]", \
                 layout->name, i, layout->map[i].label, layout->map[i].type);

        /*
         * The T_NEWLINE is used to create a new line instead of a key button.
         * It cannot be found from the parent, only used for alignment purpose.
         */
        if (layout->map[i].type == T_NEWLINE) {
            line_cnt++;
            new_line = 1;
            continue;
        }

        btn = gf_get_obj_by_name(layout->map[i].label, & get_gobj(par)->child);
        if (!btn) {
            LOG_ERROR("Key [%s] not found", layout->map[i].label);
            continue;
        }

        if (i == 0) {
            /*
             * The top padding must be doubled because there is no upper line
             * for the first line
             */
            gf_gobj_align_to(btn, par, LV_ALIGN_TOP_LEFT, size.k_pad_left, \
                             size.l_pad_top * 2);
        } else if (new_line == 1) {
            new_line = 0;
            gf_gobj_align_to(btn, par, LV_ALIGN_TOP_LEFT, size.k_pad_left, \
                             (size.l_pad_top*2 + (line_size * line_cnt)));
        } else {
            gf_gobj_align_to(btn, btn_aln, LV_ALIGN_OUT_RIGHT_TOP, \
                             (size.k_pad_left + size.k_pad_right), 0);
        }

        // The previous button is used to align the next one
        btn_aln = btn;
        set_key_size(btn, layout->map[i].type, &size);
        // Reset key configurations to the horizontal layout.
        get_gobj(btn)->pos.rot = ROTATION_0;
    }

    return 0;
}

int32_t redraw_keyboard_layout(lv_obj_t *kb)
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
    const kb_def *layout = &kb_maps[0];
    update_keys_layout(kb, layout);

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

    get_gobj(cont)->scale.rot_redraw_cb = redraw_keyboard_layout;
    return cont;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t *create_keyboard(lv_obj_t *par)
{
    lv_obj_t *kb_cont;

    kb_cont = create_keyboard_containter(par);

    /* TESTING START ***************************************/
    const kb_def *kb = &kb_maps[0];
    create_key_layout(kb_cont, kb);
    /* TESTING END ***************************************/

    return kb_cont;
}
