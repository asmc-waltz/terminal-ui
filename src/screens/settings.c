/**
 * @file settings.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <screens.h>
#include <gmisc.h>
#include <fonts.h>

/*********************
 *      DEFINES
 *********************/

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
static lv_obj_t * sf_create_main_setting_icon(lv_obj_t *par, uint32_t bg_color, \
                                              uint32_t symbol)
{
    LV_ASSERT_NULL(par);
    lv_style_t *p_style = NULL;

    p_style = gf_get_lv_style(STY_BG_ICON_37);
    lv_obj_t *icon_bg= gf_create_icon_bg(par, p_style, bg_color);

    p_style = gf_get_lv_style(STY_SYM_20);
    lv_obj_t *setting_symbol = gf_create_symbol(icon_bg, p_style, symbol);

    return icon_bg;
}

static lv_obj_t * sf_create_main_setting_container(lv_obj_t *par, uint32_t id)
{
    lv_obj_t *ctr = gf_create_obj(par, id);
    lv_style_t *p_style = gf_get_lv_style(STY_SETTING_MAIN_CTR);

    lv_obj_add_style(ctr, p_style, 0);
    lv_obj_align_to(ctr, par, LV_ALIGN_BOTTOM_LEFT, +5, -5);

    return ctr;
}

static lv_obj_t * sf_create_sub_setting_container(lv_obj_t *par, uint32_t id)
{
    lv_obj_t *ctr = gf_create_obj(par, id);
    lv_style_t *p_style = gf_get_lv_style(STY_SETTING_SUB_CTR);

    lv_obj_add_style(ctr, p_style, 0);
    lv_obj_align_to(ctr, par, LV_ALIGN_BOTTOM_RIGHT, -5, -5);

    return ctr;
}


static void sf_search_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    // lv_obj_t * kb = lv_event_get_user_data(e);

    if(code == LV_EVENT_FOCUSED) {
        if(lv_indev_get_type(lv_indev_active()) != LV_INDEV_TYPE_KEYPAD) {
            lv_keyboard_set_textarea(glob_kb, ta);
            // lv_obj_remove_flag(kb, LV_OBJ_FLAG_HIDDEN);
            gf_show_keyboard();
        }
    }
    else if(code == LV_EVENT_DEFOCUSED) {
        gf_hide_keyboard();
        // lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_state(ta, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, ta);   /*To forget the last clicked object to make it focusable again*/
    }
}

static lv_obj_t * sf_create_search_box(lv_obj_t *par, uint32_t id)
{
    lv_obj_t *search_ctr = gf_create_obj(par, id);
    lv_style_t *p_style = NULL;

    p_style = gf_get_lv_style(STY_SEARCH_BOX);
    lv_obj_add_style(search_ctr, p_style, 0);

    sf_create_main_setting_icon(search_ctr, 0xffffff, ICON_MAGNIFYING_GLASS_SOLID);

    lv_obj_t *search_ta = lv_textarea_create(search_ctr);
    gf_register_obj(search_ctr, search_ta, ID_SETTING_SEACH);

    lv_obj_set_size(search_ta, LV_PCT(100), LV_SIZE_CONTENT);
    lv_textarea_set_placeholder_text(search_ta, "Search...");
    lv_obj_set_style_text_font(search_ta, &lv_font_montserrat_20, 0);
    lv_obj_set_style_outline_width(search_ta, 0, 0);
    lv_obj_set_style_border_width(search_ta, 0, 0);
    lv_obj_set_style_shadow_width(search_ta, 0, 0);
    lv_obj_set_flex_grow(search_ta, 1);

    lv_obj_add_event_cb(search_ta, sf_search_cb, LV_EVENT_ALL, glob_kb);

    return search_ctr;
}


static lv_obj_t * sf_create_main_setting_child_ctr(lv_obj_t *par, uint32_t id)
{
    lv_obj_t *child_ctr = lv_obj_create(par);
    lv_style_t *p_style = NULL;

    p_style = gf_get_lv_style(STY_SETTING_MAIN_CHILD_CTR);
    lv_obj_add_style(child_ctr, p_style, 0);

    lv_obj_t *sub_list = lv_list_create(child_ctr);
    lv_obj_set_size(sub_list, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_align(sub_list, LV_ALIGN_CENTER, 0, 0);

    gf_register_obj(par, sub_list, id);

    return sub_list;
}

static lv_obj_t * sf_create_setting_title_ctr(lv_obj_t *par, uint32_t id)
{
    lv_obj_t *child_ctr = gf_create_obj(par, id);
    lv_style_t *p_style = gf_get_lv_style(STY_SETTING_MAIN_CHILD_CTR);

    lv_obj_add_style(child_ctr, p_style, 0);

    return child_ctr;
}

static lv_obj_t * sf_create_setting_btn(lv_obj_t *par, uint32_t id)
{
    lv_obj_t * btn = lv_list_add_button(par, NULL, NULL);
    lv_style_t *p_style = NULL;

    gf_register_obj(par, btn, id);
    p_style = gf_get_lv_style(STY_SETTING_BTN);
    lv_obj_add_style(btn, p_style, 0);

    return btn;
}

static lv_obj_t * sf_create_setting_btn_name(lv_obj_t *par, char *text)
{
    lv_obj_t *lbl = lv_label_create(par);

    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_18, 0);
    lv_label_set_text_fmt(lbl, "  %s", text);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_flex_grow(lbl, 1);

    return lbl;
}

static lv_obj_t * sf_create_setting_btn_status(lv_obj_t *par, char *text, uint32_t id)
{
    lv_obj_t *lbl = lv_label_create(par);

    gf_register_obj(par, lbl, id);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_18, 0);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text_fmt(lbl, "%s", text);

    return lbl;
}

static lv_obj_t * sf_create_setting_btn_switch(lv_obj_t *par, uint32_t id)
{
    lv_obj_t *sw = lv_switch_create(par);

    gf_register_obj(par, sw, id);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x03BF1F), LV_PART_INDICATOR | LV_STATE_CHECKED);

    return sw;
}

static lv_obj_t * sf_create_main_setting_title(lv_obj_t *par, char *text, uint32_t id)
{
    lv_obj_t *lbl = lv_label_create(par);

    gf_register_obj(par, lbl, id);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_38, 0);
    lv_label_set_text_fmt(lbl, "%s", text);

    return lbl;
}

static lv_obj_t * sf_create_sub_setting_title(lv_obj_t *par, char *text, uint32_t id)
{
    lv_obj_t *lbl = lv_label_create(par);

    gf_register_obj(par, lbl, id);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_28, 0);
    lv_label_set_text_fmt(lbl, "%s", text);
    lv_obj_set_style_align(lbl, LV_ALIGN_CENTER, 0);

    return lbl;
}

static void sf_update_sub_setting_title(char *text)
{
    lv_obj_t *lv_obj = gf_get_obj(ID_SETTING_SUB_TITLE, NULL);
    lv_label_set_text_fmt(lv_obj, "%s", text);
}

static void setting_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target_obj(e);
    uint32_t id = ID_NONE;

    if (code == LV_EVENT_CLICKED) {
        id = ((g_obj *)(obj->user_data))->id;
        LV_LOG_USER("ID %d: %s is clicked", id, lv_list_get_button_text(NULL, obj));
        sf_update_sub_setting_title(lv_list_get_button_text(NULL, obj));

        if (id == ID_SETTING_SEACH) {
            gf_show_keyboard();
        }
    }
}

static void sf_create_main_setting_menu(lv_obj_t *par)
{
    lv_obj_t * btn;
    lv_obj_t *main_ctr = sf_create_main_setting_container(par, ID_SETTING_MAIN_CTR);

    lv_obj_t *child_ctr = sf_create_setting_title_ctr(main_ctr, ID_COMM);
    sf_create_main_setting_title(child_ctr, "Settings", ID_SETTING_MAIN_TITLE);

    // Seach
    child_ctr = sf_create_search_box(main_ctr, ID_SETTING_SUB_CRT_SEARCH);

    // Wireless
    child_ctr = sf_create_main_setting_child_ctr(main_ctr, ID_SETTING_SUB_CRT_WIRELESS);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_AIRPLANE);
    sf_create_main_setting_icon(btn, 0xFF6B3A, ICON_PLANE_SOLID);
    sf_create_setting_btn_name(btn, "Airplane");
    gf_register_handler(btn, ID_SETTING_AIRPLANE, setting_handler, LV_EVENT_CLICKED);
    sf_create_setting_btn_switch(btn, ID_SETTING_AIRPLANE_SW);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_WIFI);
    sf_create_main_setting_icon(btn, 0x3895FF, ICON_WIFI_SOLID);
    sf_create_setting_btn_name(btn, "Wi-Fi");
    sf_create_setting_btn_status(btn, "go_5G", ID_SETTING_WIFI_STATUS);
    gf_register_handler(btn, ID_SETTING_WIFI, setting_handler, LV_EVENT_CLICKED);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_HOSTSPOT);
    sf_create_main_setting_icon(btn, 0x86807B, ICON_SHARE_NODES_SOLID);
    sf_create_setting_btn_name(btn, "Hostspot");
    gf_register_handler(btn, ID_SETTING_HOSTSPOT, setting_handler, LV_EVENT_CLICKED);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_BLUETOOTH);
    sf_create_main_setting_icon(btn, 0x3895FF, ICON_SHARE_NODES_SOLID);
    sf_create_setting_btn_name(btn, "Bluetooth");
    sf_create_setting_btn_status(btn, "Off", ID_SETTING_BLUETOOTH_STATUS);
    gf_register_handler(btn, ID_SETTING_BLUETOOTH, setting_handler, LV_EVENT_CLICKED);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_CELLULAR);
    sf_create_main_setting_icon(btn, 0x03BF1F, ICON_TOWER_CELL_SOLID);
    sf_create_setting_btn_name(btn, "Cellular");
    sf_create_setting_btn_status(btn, "On", ID_SETTING_CELLULAR_STATUS);
    gf_register_handler(btn, ID_SETTING_CELLULAR, setting_handler, LV_EVENT_CLICKED);

    // Cable - Network
    child_ctr = sf_create_main_setting_child_ctr(main_ctr, ID_SETTING_SUB_CRT_NETWORK);
    btn = sf_create_setting_btn(child_ctr, ID_SETTING_ETHERNET);
    sf_create_main_setting_icon(btn, 0xFBDE00, ICON_ETHERNET_SOLID);
    sf_create_setting_btn_name(btn, "Ethernet");
    gf_register_handler(btn, ID_SETTING_ETHERNET, setting_handler, LV_EVENT_CLICKED);
    sf_create_setting_btn_switch(btn, ID_SETTING_ETHERNET_SW);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_NETWORK);
    sf_create_main_setting_icon(btn, 0x9E78FF, ICON_NETWORK_WIRED_SOLID);
    sf_create_setting_btn_name(btn, "Network");
    gf_register_handler(btn, ID_SETTING_NETWORK, setting_handler, LV_EVENT_CLICKED);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_FILTER);
    sf_create_main_setting_icon(btn, 0xFF6B3A, ICON_FILTER_SOLID);
    sf_create_setting_btn_name(btn, "Filter");
    gf_register_handler(btn, ID_SETTING_FILTER, setting_handler, LV_EVENT_CLICKED);
    sf_create_setting_btn_switch(btn, ID_SETTING_FILTER_SW);

    // Phone - Message
    child_ctr = sf_create_main_setting_child_ctr(main_ctr, ID_SETTING_SUB_CRT_PHONE);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_PHONE);
    sf_create_main_setting_icon(btn, 0x34E424, ICON_PHONE_SOLID);
    sf_create_setting_btn_name(btn, "Phone");
    gf_register_handler(btn, ID_SETTING_PHONE, setting_handler, LV_EVENT_CLICKED);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_MESSAGE);
    sf_create_main_setting_icon(btn, 0x34E424, ICON_COMMENT_SOLID);
    sf_create_setting_btn_name(btn, "Message");
    gf_register_handler(btn, ID_SETTING_MESSAGE, setting_handler, LV_EVENT_CLICKED);

    // General setting
    child_ctr = sf_create_main_setting_child_ctr(main_ctr, ID_SETTING_SUB_CRT_GENERAL);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_ALERT_AND_HAPTIC);
    sf_create_main_setting_icon(btn, 0xFF6B3A, ICON_BELL_SOLID);
    sf_create_setting_btn_name(btn, "Alert & Haptic");
    gf_register_handler(btn, ID_SETTING_ALERT_AND_HAPTIC, setting_handler, LV_EVENT_CLICKED);
    sf_create_setting_btn_switch(btn, ID_SETTING_ALERT_AND_HAPTIC_SW);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_AUDIO);
    sf_create_main_setting_icon(btn, 0xFBDE00, ICON_VOLUME_HIGH_SOLID);
    sf_create_setting_btn_name(btn, "Audio");
    gf_register_handler(btn, ID_SETTING_AUDIO, setting_handler, LV_EVENT_CLICKED);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_MICROPHONE);
    sf_create_main_setting_icon(btn, 0xFBDE00, ICON_MICROPHONE_SOLID);
    sf_create_setting_btn_name(btn, "Microphone");
    gf_register_handler(btn, ID_SETTING_MICROPHONE, setting_handler, LV_EVENT_CLICKED);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_ROTATE);
    sf_create_main_setting_icon(btn, 0x7AB6FE, ICON_ROTATE_SOLID);
    sf_create_setting_btn_name(btn, "Rotate");
    gf_register_handler(btn, ID_SETTING_ROTATE, setting_handler, LV_EVENT_CLICKED);
    sf_create_setting_btn_switch(btn, ID_SETTING_ROTATE_SW);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_DATE_TIME);
    sf_create_main_setting_icon(btn, 0x008097, ICON_CALENDAR_DAYS_SOLID);
    sf_create_setting_btn_name(btn, "Date & Time");
    gf_register_handler(btn, ID_SETTING_DATE_TIME, setting_handler, LV_EVENT_CLICKED);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_GNSS);
    sf_create_main_setting_icon(btn, 0x9E78FF, ICON_SATELLITE_SOLID);
    sf_create_setting_btn_name(btn, "GNSS");
    gf_register_handler(btn, ID_SETTING_GNSS, setting_handler, LV_EVENT_CLICKED);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_BRIGHTNESS);
    sf_create_main_setting_icon(btn, 0xFBDE00, ICON_CIRCLE_HALF_STROKE_SOLID);
    sf_create_setting_btn_name(btn, "Brightness");
    gf_register_handler(btn, ID_SETTING_BRIGHTNESS, setting_handler, LV_EVENT_CLICKED);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_POWER);
    sf_create_main_setting_icon(btn, 0xBD0000, ICON_BOLT_SOLID);
    sf_create_setting_btn_name(btn, "Power");
    gf_register_handler(btn, ID_SETTING_POWER, setting_handler, LV_EVENT_CLICKED);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_ABOUT);
    sf_create_main_setting_icon(btn, 0x34E424, ICON_CIRCLE_INFO_SOLID);
    sf_create_setting_btn_name(btn, "About");
    gf_register_handler(btn, ID_SETTING_ABOUT, setting_handler, LV_EVENT_CLICKED);
}

static void sf_create_sub_setting_menu(lv_obj_t *par)
{
    lv_obj_t * btn;
    lv_obj_t *main_ctr = sf_create_sub_setting_container(par, ID_SETTING_SUB_CTR);
    lv_obj_t *child_ctr = sf_create_setting_title_ctr(main_ctr, ID_COMM);

    sf_create_sub_setting_title(child_ctr, "Airplane", ID_SETTING_SUB_TITLE);
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void gf_create_setting_menu(lv_obj_t *par)
{
    sf_create_main_setting_menu(par);
    sf_create_sub_setting_menu(par);
    gf_hide_setting();
}

void gf_hide_setting()
{
    lv_obj_t *lv_obj = gf_get_obj(ID_SETTING_MAIN_CTR, NULL);
    lv_obj_add_flag(lv_obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj = gf_get_obj(ID_SETTING_SUB_CTR, NULL);
    lv_obj_add_flag(lv_obj, LV_OBJ_FLAG_HIDDEN);
}

void gf_show_setting()
{
    lv_obj_t *lv_obj = gf_get_obj(ID_SETTING_MAIN_CTR, NULL);
    lv_obj_remove_flag(lv_obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj = gf_get_obj(ID_SETTING_SUB_CTR, NULL);
    lv_obj_remove_flag(lv_obj, LV_OBJ_FLAG_HIDDEN);
}
