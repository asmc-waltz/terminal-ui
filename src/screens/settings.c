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
static lv_obj_t * list1;

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

static lv_obj_t * sf_create_main_setting_container(lv_obj_t *par)
{
    lv_obj_t *container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(container, 332, 530);
    lv_obj_align_to(container, lv_screen_active(), LV_ALIGN_BOTTOM_LEFT, +10, -10);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN); // Vertical stacking
    lv_obj_set_style_bg_color(container, lv_color_hex(0xE9EDF2), 0);
    lv_obj_set_style_pad_all(container, 15, 0);
    lv_obj_set_style_pad_gap(container, 0, 0);
    lv_obj_set_style_border_width(container, 1, 0);
    lv_obj_set_style_border_color(container, lv_color_hex(0x979797), LV_PART_MAIN);

    lv_obj_scroll_to_view(container, LV_ANIM_ON);
    lv_obj_set_style_anim_duration(container, 50, 0);
    return container;
}

static lv_obj_t * sf_create_main_setting_child_ctr(lv_obj_t *par)
{
    lv_obj_t *child_ctr = lv_obj_create(par);
    lv_obj_set_size(child_ctr, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(child_ctr, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_pad_all(child_ctr, 0, 0);
    lv_obj_set_style_pad_gap(child_ctr, 0, 0);
    lv_obj_set_style_border_width(child_ctr, 0, 0);

    lv_obj_t *sub_list = lv_list_create(child_ctr);
    lv_obj_set_size(sub_list, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_align(sub_list, LV_ALIGN_CENTER, 0, 0);


    return sub_list;
}

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target_obj(e);
    if(code == LV_EVENT_CLICKED) {
        LV_UNUSED(obj);
        LV_LOG_USER("Clicked: %s", lv_list_get_button_text(list1, obj));
    }
}

static lv_obj_t * sf_create_setting_btn(lv_obj_t *par, uint32_t btn_id)
{
    lv_obj_t * btn = lv_list_add_button(par, NULL, NULL);
    gf_register_obj(btn, btn_id);

    lv_obj_set_layout(btn, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_set_style_pad_all(btn, 5, 0);
    lv_obj_set_style_pad_gap(btn, 0, 0);
    lv_obj_set_size(btn, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_radius(btn, 8, 0);

    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    return btn;
}

static lv_obj_t * sf_create_setting_btn_name(lv_obj_t *par, char *text)
{
    lv_obj_t *lbl = lv_label_create(par);
    lv_label_set_text_fmt(lbl, "    %s", text);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_flex_grow(lbl, 1);

    return lbl;
}

static lv_obj_t * sf_create_setting_btn_status(lv_obj_t *par, char *def_stt, uint32_t stt_id)
{
    lv_obj_t *lbl = lv_label_create(par);
    gf_register_obj(lbl, stt_id);
    lv_label_set_text_fmt(lbl, "    %s", def_stt);

    return lbl;
}

static lv_obj_t * sf_create_setting_btn_switch(lv_obj_t *par, uint32_t sw_id)
{
    lv_obj_t *sw = lv_switch_create(par);
    gf_register_obj(sw, sw_id);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x03BF1F), LV_PART_INDICATOR | LV_STATE_CHECKED);

    return sw;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void gf_create_main_setting_menu(void)
{
    lv_obj_t * btn;
    lv_obj_t *main_ctr = sf_create_main_setting_container(lv_screen_active());
    lv_obj_set_style_pad_row(main_ctr, 15, 0);


    // Seach
    lv_obj_t *child_ctr = sf_create_main_setting_child_ctr(main_ctr);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_SEACH);
    sf_create_main_setting_icon(btn, 0xffffff, ICON_MAGNIFYING_GLASS_SOLID);
    sf_create_setting_btn_name(btn, "Search...");

    // Wireless
    child_ctr = sf_create_main_setting_child_ctr(main_ctr);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_AIRPLANE);
    sf_create_main_setting_icon(btn, 0xFF6B3A, ICON_PLANE_SOLID);
    sf_create_setting_btn_name(btn, "Airplane");
    sf_create_setting_btn_switch(btn, ID_SETTING_AIRPLANE_SW);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_WIFI);
    sf_create_main_setting_icon(btn, 0x3895FF, ICON_WIFI_SOLID);
    sf_create_setting_btn_name(btn, "Wi-Fi");
    sf_create_setting_btn_status(btn, "go_5G", ID_SETTING_WIFI_STATUS);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_HOSTSPOT);
    sf_create_main_setting_icon(btn, 0x86807B, ICON_SHARE_NODES_SOLID);
    sf_create_setting_btn_name(btn, "Hostspot");

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_BLUETOOTH);
    sf_create_main_setting_icon(btn, 0x3895FF, ICON_SHARE_NODES_SOLID);
    sf_create_setting_btn_name(btn, "Bluetooth");
    sf_create_setting_btn_status(btn, "Off", ID_SETTING_BLUETOOTH_STATUS);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_CELLULAR);
    sf_create_main_setting_icon(btn, 0x03BF1F, ICON_TOWER_CELL_SOLID);
    sf_create_setting_btn_name(btn, "Cellular");
    sf_create_setting_btn_status(btn, "On", ID_SETTING_CELLULAR_STATUS);


    // Cable - Network
    child_ctr = sf_create_main_setting_child_ctr(main_ctr);
    btn = sf_create_setting_btn(child_ctr, ID_SETTING_ETHERNET);
    sf_create_main_setting_icon(btn, 0xFBDE00, ICON_ETHERNET_SOLID);
    sf_create_setting_btn_name(btn, "Ethernet");
    sf_create_setting_btn_switch(btn, ID_SETTING_ETHERNET_SW);


    btn = sf_create_setting_btn(child_ctr, ID_SETTING_NETWORK);
    sf_create_main_setting_icon(btn, 0x9E78FF, ICON_NETWORK_WIRED_SOLID);
    sf_create_setting_btn_name(btn, "Network");


    btn = sf_create_setting_btn(child_ctr, ID_SETTING_FILTER);
    sf_create_main_setting_icon(btn, 0xFF6B3A, ICON_FILTER_SOLID);
    sf_create_setting_btn_name(btn, "Filter");
    sf_create_setting_btn_switch(btn, ID_SETTING_FILTER_SW);


    // Phone - Message
    child_ctr = sf_create_main_setting_child_ctr(main_ctr);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_PHONE);
    sf_create_main_setting_icon(btn, 0x34E424, ICON_PHONE_SOLID);
    sf_create_setting_btn_name(btn, "Phone");


    btn = sf_create_setting_btn(child_ctr, ID_SETTING_MESSAGE);
    sf_create_main_setting_icon(btn, 0x34E424, ICON_COMMENT_SOLID);
    sf_create_setting_btn_name(btn, "Message");


    // General setting
    child_ctr = sf_create_main_setting_child_ctr(main_ctr);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_ALERT_AND_HAPTIC);
    sf_create_main_setting_icon(btn, 0xFF6B3A, ICON_BELL_SOLID);
    sf_create_setting_btn_name(btn, "Alert & Haptic");
    sf_create_setting_btn_switch(btn, ID_SETTING_ALERT_AND_HAPTIC_SW);


    btn = sf_create_setting_btn(child_ctr, ID_SETTING_AUDIO);
    sf_create_main_setting_icon(btn, 0xFBDE00, ICON_VOLUME_HIGH_SOLID);
    sf_create_setting_btn_name(btn, "Audio");

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_MICROPHONE);
    sf_create_main_setting_icon(btn, 0xFBDE00, ICON_MICROPHONE_SOLID);
    sf_create_setting_btn_name(btn, "Microphone");

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_ROTATE);
    sf_create_main_setting_icon(btn, 0x7AB6FE, ICON_ROTATE_SOLID);
    sf_create_setting_btn_name(btn, "Rotate");
    sf_create_setting_btn_switch(btn, ID_SETTING_ROTATE_SW);

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_DATE_TIME);
    sf_create_main_setting_icon(btn, 0x008097, ICON_CALENDAR_DAYS_SOLID);
    sf_create_setting_btn_name(btn, "Date & Time");

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_GNSS);
    sf_create_main_setting_icon(btn, 0x9E78FF, ICON_SATELLITE_SOLID);
    sf_create_setting_btn_name(btn, "GNSS");

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_BRIGHTNESS);
    sf_create_main_setting_icon(btn, 0xFBDE00, ICON_CIRCLE_HALF_STROKE_SOLID);
    sf_create_setting_btn_name(btn, "Brightness");

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_POWER);
    sf_create_main_setting_icon(btn, 0xBD0000, ICON_BOLT_SOLID);
    sf_create_setting_btn_name(btn, "Power");

    btn = sf_create_setting_btn(child_ctr, ID_SETTING_ABOUT);
    sf_create_main_setting_icon(btn, 0x34E424, ICON_CIRCLE_INFO_SOLID);
    sf_create_setting_btn_name(btn, "About");
}

