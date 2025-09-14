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
#define KEY_MODE_WIDTH                  ((KEY_CHAR_WIDTH * 17) / 10)  // % (Magic)
#define KEY_ENTER_WIDTH                 ((2 * KEY_CHAR_WIDTH) + (1 * (KEY_PAD_LEFT + \
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
    T_NEWLINE,
    T_END
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
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

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

    {"Shift", T_SHIFT}, \
    {"Z", T_CHAR}, {"X", T_CHAR}, {"C", T_CHAR}, {"V", T_CHAR}, {"B", T_CHAR}, \
    {"N", T_CHAR}, {"M", T_CHAR}, \
    {"Del", T_DELETE}, \
    {"\n", T_NEWLINE},

    {"123", T_MODE}, {"KB", T_MODE}, {" ", T_SPACE}, {"Enter", T_ENTER}, 
    {"End", T_END}
};

static const k_def map_abc[] = {
    {"q", T_CHAR}, {"w", T_CHAR}, {"e", T_CHAR}, {"r", T_CHAR}, {"t", T_CHAR}, \
    {"y", T_CHAR}, {"u", T_CHAR}, {"i", T_CHAR}, {"o", T_CHAR}, {"p", T_CHAR}, \
    {"\n", T_NEWLINE}, \

    {"a", T_CHAR}, {"s", T_CHAR}, {"d", T_CHAR}, {"f", T_CHAR}, {"g", T_CHAR}, \
    {"h", T_CHAR}, {"j", T_CHAR}, {"k", T_CHAR}, {"l", T_CHAR}, \
    {"\n", T_NEWLINE}, \

    {"Shift", T_SHIFT}, \
    {"z", T_CHAR}, {"x", T_CHAR}, {"c", T_CHAR}, {"v", T_CHAR}, {"b", T_CHAR}, \
    {"n", T_CHAR}, {"m", T_CHAR}, \
    {"Del", T_DELETE}, \
    {"\n", T_NEWLINE},

    {"123", T_MODE}, {"KB", T_MODE}, {" ", T_SPACE}, {"Enter", T_ENTER}, 
    {"End", T_END}
};

static const k_def map_number[] = {
    {"1", T_NUM}, {"2", T_NUM}, {"3", T_NUM}, {"4", T_NUM}, {"5", T_NUM}, \
    {"6", T_NUM}, {"7", T_NUM}, {"8", T_NUM}, {"9", T_NUM}, {"0", T_NUM}, \
    {"\n", T_NEWLINE}, \

    {"-", T_SYM}, {"/", T_SYM}, {":", T_SYM}, {";", T_SYM}, {"(", T_SYM}, \
    {")", T_SYM}, {"...", T_SYM}, {"&", T_SYM}, {"@", T_SYM}, \
    {"\n", T_NEWLINE}, \

    {"#+=", T_SHIFT}, \
    {".", T_SYM}, {",", T_SYM}, {"?", T_SYM}, {"!", T_SYM}, {"\"", T_SYM}, \
    {"'", T_SYM}, {"*", T_SYM}, \
    {"Del", T_DELETE}, \
    {"\n", T_NEWLINE},

    {"ABC", T_MODE}, {"KB", T_MODE}, {" ", T_SPACE}, {"Enter", T_ENTER}, 
    {"End", T_END}
};

static const k_def map_symbol[] = {
    {"1", T_NUM}, {"2", T_NUM}, {"3", T_NUM}, {"4", T_NUM}, {"5", T_NUM}, \
    {"6", T_NUM}, {"7", T_NUM}, {"8", T_NUM}, {"9", T_NUM}, {"0", T_NUM}, \
    {"\n", T_NEWLINE}, \

    {"[", T_SYM}, {"]", T_SYM}, {"\{", T_SYM}, {"\}", T_SYM}, {"#", T_SYM},\
    {"%", T_SYM}, {"^", T_SYM}, {"+", T_SYM}, {"=", T_SYM},  \
    {"\n", T_NEWLINE}, \

    {"123", T_SHIFT}, \
    {"_", T_SYM}, {"\\", T_SYM}, {"|", T_SYM}, {"~", T_SYM}, {"<", T_SYM}, \
    {">", T_SYM}, {"$", T_SYM}, \
    {"Del", T_DELETE}, \
    {"\n", T_NEWLINE},

    {"ABC", T_MODE}, {"KB", T_MODE}, {" ", T_SPACE}, {"Enter", T_ENTER}, 
    {"End", T_END}
};


static const kb_def kb_maps[] = {
    {"ABC", map_ABC, sizeof(map_ABC) / sizeof(map_ABC[0])},
    {"abc", map_abc, sizeof(map_abc) / sizeof(map_abc[0])},
    {"123", map_number, sizeof(map_number) / sizeof(map_number[0])},
    {"@*#", map_symbol, sizeof(map_symbol) / sizeof(map_symbol[0])},
};

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
    lv_obj_set_style_pad_all(btn, 0, 0);
    lv_obj_set_style_pad_gap(btn, 0, 0);
    lv_obj_add_event_cb(btn, kb_btn_cb, LV_EVENT_CLICKED, btn->user_data);

    lbl = gf_create_text(btn, NULL, 10, 10, key->label);
    lv_obj_set_style_text_color(lbl, lv_color_hex(0x000000), 0);
    lv_obj_set_style_text_font(lbl, KEYBOARD_CHAR_FONTS, 0);
    gf_gobj_set_size(lbl, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

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
    case T_MODE:
        key_w = size->key_mode_w;
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

lv_obj_t *create_line_box(lv_obj_t *par, kb_size_ctx *size)
{
    int32_t obj_w = 0, obj_h = 0;
    lv_obj_t *line_box;

    line_box = gf_create_box(par, KEYBOAR_NAME".linebox");
    if (!line_box)
        return NULL;


    obj_w = obj_width(par);
    obj_h = size->l_pad_top + size->key_com_h + size->l_pad_bot;

    gf_gobj_set_size(line_box, LV_SIZE_CONTENT, obj_h);
    lv_obj_set_style_bg_opa(line_box, LV_OPA_0, 0);
    // lv_obj_set_style_bg_color(line_box, lv_color_hex(0xBDBDBD), 0);

    return line_box;

}

int32_t create_keys_layout(lv_obj_t *par, const kb_def *layout)
{
    lv_obj_t *btn, *btn_aln;
    int8_t line_cnt = 0, new_line = 0, i;
    int32_t ret;
    int32_t line_h, line_w;
    kb_size_ctx size;
    lv_obj_t *line_box;

    ret = calc_kb_size_data(par, &size);
    if (ret) {
        LOG_ERROR("Unable to calculate keyboard child size");
        return -EINVAL;
    }

    // First line box
    line_box = create_line_box(par, &size);
    if (!line_box)
        return -EINVAL;
    gf_gobj_align_to(line_box, par, LV_ALIGN_TOP_LEFT, 0, 0);

    line_h = size.l_pad_top + size.key_com_h + size.l_pad_bot;

    for (i = 0; i < layout->size; i++) {
        LOG_TRACE("Keyboard %s: index[%d] character[%s] type[%d]", \
                   layout->name, i, layout->map[i].label, layout->map[i].type);

        if (layout->map[i].type == T_NEWLINE) {

            //-----------------
            int32_t first_key_x_ofs = (obj_width(par) - line_w) / 2;
            gf_gobj_align_to(line_box, par, LV_ALIGN_TOP_LEFT, first_key_x_ofs, \
                             (size.l_pad_top + (line_h * line_cnt)));
            line_w = 0;
            //-----------------

            line_cnt++;
            new_line = 1;

            //-----------------
            // Create new line box
            line_box = create_line_box(par, &size);
            if (!line_box)
                return -EINVAL;
            // Preset for the new line - Last line got trouble
            gf_gobj_align_to(line_box, par, LV_ALIGN_TOP_LEFT, 0, \
                             (size.l_pad_top + (line_h * (line_cnt))));
            //-----------------
            continue;
        } else if (layout->map[i].type == T_END) {
            int32_t first_key_x_ofs = (obj_width(par) - line_w) / 2;
            gf_gobj_align_to(line_box, par, LV_ALIGN_TOP_LEFT, first_key_x_ofs, \
                             (size.l_pad_top + (line_h * line_cnt)));
            line_w = 0;
            continue;
        }

        // btn = create_key(par, &layout->map[i]);
        btn = create_key(line_box, &layout->map[i]);
        if (i == 0) {
            /*
             * The top padding must be doubled because there is no upper line
             * for the first line
             */
            // gf_gobj_align_to(btn, par, LV_ALIGN_TOP_LEFT, size.k_pad_left, \
            //                  size.l_pad_top * 2);
            gf_gobj_align_to(btn, line_box, LV_ALIGN_TOP_LEFT, size.k_pad_left, \
                             size.l_pad_top);
        } else if (new_line == 1) {
            new_line = 0;
            // gf_gobj_align_to(btn, par, LV_ALIGN_TOP_LEFT, size.k_pad_left, \
            //                  (size.l_pad_top*2 + (line_h * line_cnt)));
            gf_gobj_align_to(btn, line_box, LV_ALIGN_TOP_LEFT, size.k_pad_left, \
                             size.l_pad_top);
        } else {
            gf_gobj_align_to(btn, btn_aln, LV_ALIGN_OUT_RIGHT_TOP, \
                             (size.k_pad_left + size.k_pad_right), 0);
        }

        // The previous button is used to align the next one
        btn_aln = btn;
        set_key_size(btn, layout->map[i].type, &size);
        line_w += size.k_pad_left + get_gobj(btn)->pos.w + size.k_pad_right;
    }

    return 0;
}

int32_t update_keys_layout(lv_obj_t *par, const kb_def *layout)
{
    lv_obj_t *btn, *btn_aln;
    int8_t line_cnt = 0, new_line = 0, i;
    int32_t ret;
    int32_t line_h;
    kb_size_ctx size;

    ret = calc_kb_size_data(par, &size);
    if (ret) {
        LOG_ERROR("Unable to calculate keyboard child size");
        return -EINVAL;
    }

    line_h = size.l_pad_top + size.key_com_h + size.l_pad_bot;

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
                             (size.l_pad_top*2 + (line_h * line_cnt)));
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

    get_gobj(cont)->scale.pre_rot_redraw_cb = pre_rotation_redraw_kb_layout;

    return cont;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t *create_keyboard(lv_obj_t *par)
{
    lv_obj_t *kb;
    const kb_def *layout = &kb_maps[0];
    int32_t ret;

    kb = create_keyboard_containter(par);
    if (!kb)
        return NULL;

    ret = create_keys_layout(kb, layout);
    if (ret) {
        LOG_ERROR("Create keyboard failed %d, remove container ret %d", ret, \
                   gf_remove_obj_and_child_by_name(KEYBOAR_NAME, \
                                            &get_gobj(par)->child));
        return NULL;
    }

    if (g_get_scr_rot_dir() != ROTATION_0) {
        refresh_obj_tree_layout(kb->user_data);
    }

    return kb;
}
