/**
 * @file curved_lines.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <style.h>
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
static LIST_HEAD(HEAD_OF_STYLES);

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
static lv_style_t * sf_create_style_data(char id)
{
    g_style *p_style = NULL;

    LV_LOG_USER("Register style id %d", id);
    LV_ASSERT_NULL(id);
    p_style = malloc(sizeof(g_style));
    LV_ASSERT_NULL(p_style);
    p_style->id = id;
    list_add_tail(&p_style->node, &HEAD_OF_STYLES);

    return &p_style->style;
}

static void sf_background_style_init(lv_style_t *p_style)
{
    lv_style_init(p_style);
    lv_style_set_radius(p_style, 20);
    lv_style_set_clip_corner(p_style, true);
    lv_style_set_border_color(p_style, lv_color_hex(0x000000));
    lv_style_set_border_width(p_style, 0);
    lv_style_set_border_post(p_style, true);
    lv_style_set_shadow_color(p_style, lv_color_hex(0x000000));
    lv_style_set_shadow_width(p_style, 50);
    lv_style_set_shadow_spread(p_style, 10);
    lv_style_set_outline_color(p_style, lv_color_hex(0x000000));
    lv_style_set_outline_width(p_style, 2);
    lv_style_set_outline_pad(p_style, 5);
    lv_style_set_line_color(p_style, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_line_width(p_style, 1);
    lv_style_set_bg_color(p_style, lv_color_hex(0x478DDF));
    lv_style_set_bg_grad_color(p_style, lv_color_hex(0x66D569));
    lv_style_set_bg_grad_dir(p_style, LV_GRAD_DIR_HOR);
    lv_style_set_bg_opa(p_style, LV_OPA_COVER);
}

static void sf_taskbar_style_init(lv_style_t *p_style)
{
    lv_style_init(p_style);
    lv_style_set_radius(p_style, 30);
    lv_style_set_bg_opa(p_style, LV_OPA_TRANSP);
    lv_style_set_bg_color(p_style, lv_color_black());
    lv_style_set_border_color(p_style, lv_color_black());
    lv_style_set_border_width(p_style, 0);
    lv_style_set_border_post(p_style, true);
    lv_style_set_pad_all(p_style, 10);
    lv_style_set_shadow_color(p_style, lv_color_black());
    lv_style_set_shadow_width(p_style, 0);
    lv_style_set_shadow_spread(p_style, 0);
    lv_style_set_outline_color(p_style, lv_color_black());
    lv_style_set_outline_width(p_style, 0);
    lv_style_set_outline_pad(p_style, 0);
}

static void sf_app_icon_bg_style_init(lv_style_t *p_style)
{
    lv_style_init(p_style);
    lv_style_set_radius(p_style, 20);
    lv_style_set_size(p_style, 79, 79);
    lv_style_set_outline_color(p_style, lv_color_black());
    lv_style_set_outline_width(p_style, 0);
    lv_style_set_outline_pad(p_style, 0);
}

static void sf_app_sym_style_init(lv_style_t *p_style)
{
    lv_style_init(p_style);
    lv_style_set_size(p_style, 48, 48);
    lv_style_set_text_font(p_style, &terminal_icons_48);
    lv_style_set_text_color(p_style, lv_color_white());
}

static void sf_status_bar_style_init(lv_style_t *p_style)
{
    lv_style_init(p_style);
    lv_style_set_radius(p_style, 20);
    lv_style_set_bg_opa(p_style, LV_OPA_TRANSP);
    lv_style_set_bg_color(p_style, lv_color_black());
    lv_style_set_text_font(p_style, &terminal_icons_32);
    lv_style_set_text_color(p_style, lv_color_black());
    lv_style_set_border_color(p_style, lv_color_hex(0x000000));
    lv_style_set_border_width(p_style, 0);
    lv_style_set_border_post(p_style, true);
    lv_style_set_pad_all(p_style, 0);
    lv_style_set_shadow_color(p_style, lv_color_hex(0x000000));
    lv_style_set_shadow_width(p_style, 0);
    lv_style_set_shadow_spread(p_style, 0);
    lv_style_set_outline_color(p_style, lv_color_hex(0x000000));
    lv_style_set_outline_width(p_style, 0);
    lv_style_set_outline_pad(p_style, 0);
}

static void sf_sts_sym_style_init(lv_style_t *p_style)
{
    lv_style_init(p_style);
    lv_style_set_size(p_style, 32, 32);
    lv_style_set_text_font(p_style, &terminal_icons_32);
    lv_style_set_text_color(p_style, lv_color_white());
}

static void sf_in_app_icon_bg_style_init(lv_style_t *p_style)
{
    lv_style_init(p_style);
    lv_style_set_radius(p_style, 9);
    lv_style_set_size(p_style, 37, 37);
    lv_style_set_outline_color(p_style, lv_color_black());
    lv_style_set_outline_width(p_style, 0);
    lv_style_set_outline_pad(p_style, 0);


    lv_style_set_border_width(p_style, 0);
    lv_style_set_pad_all(p_style, 0);
    lv_style_set_shadow_width(p_style, 0);
}

static void sf_in_app_sym_style_init(lv_style_t *p_style)
{
    lv_style_init(p_style);
    lv_style_set_size(p_style, 20, 20);
    lv_style_set_text_font(p_style, &terminal_icons_20);
    lv_style_set_text_color(p_style, lv_color_black());

    lv_style_set_border_width(p_style, 0);
    lv_style_set_outline_width(p_style, 0);
    lv_style_set_outline_pad(p_style, 0);
    lv_style_set_pad_all(p_style, 0);
    lv_style_set_shadow_width(p_style, 0);
}

static void sf_home_indicator_style_init(lv_style_t *p_style)
{
    lv_style_init(p_style);
    lv_style_set_bg_color(p_style, lv_color_black());
    lv_style_set_radius(p_style, 5);
    lv_style_set_size(p_style, 315, 10);
    lv_style_set_border_width(p_style, 0);
    lv_style_set_pad_all(p_style, 0);
}

static void sf_setting_main_ctr_style_init(lv_style_t *p_style)
{
    lv_style_init(p_style);
    lv_style_set_size(p_style, 332, 530);
    lv_style_set_flex_flow(p_style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_layout(p_style, LV_LAYOUT_FLEX);
    lv_style_set_bg_color(p_style, lv_color_hex(0xE9EDF2));
    lv_style_set_pad_all(p_style, 15);
    lv_style_set_pad_gap(p_style, 0);
    lv_style_set_border_width(p_style, 1);
    lv_style_set_border_color(p_style, lv_color_hex(0x979797));
    lv_style_set_pad_row(p_style, 20);
    lv_style_set_anim_duration(p_style, 50);
}

static void sf_setting_main_child_ctr_style_init(lv_style_t *p_style)
{
    lv_style_init(p_style);
    lv_style_set_size(p_style, LV_PCT(100), LV_SIZE_CONTENT);
    lv_style_set_bg_opa(p_style, LV_OPA_TRANSP);
    lv_style_set_pad_all(p_style, 0);
    lv_style_set_pad_gap(p_style, 0);
    lv_style_set_border_width(p_style, 0);
}

static void sf_setting_btn_style_init(lv_style_t *p_style)
{
    lv_style_init(p_style);
    lv_style_set_layout(p_style, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(p_style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(p_style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(p_style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_track_place(p_style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_pad_all(p_style, 5);
    lv_style_set_pad_gap(p_style, 0);
    lv_style_set_size(p_style, LV_PCT(100), LV_SIZE_CONTENT);
    lv_style_set_radius(p_style, 8);
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_style_t * gf_get_lv_style(char style_id)
{
    g_style *p_style = NULL;

    list_for_each_entry(p_style, &HEAD_OF_STYLES, node)
    if (p_style->id != 0) {
        if (p_style->id == style_id) {
            // LV_LOG_USER("Style REQ id is detected %d", p_style->id);
            break;
        } else {
            // TODO
            continue;
        }
    }

    return &p_style->style;
}

void gf_styles_init(void)
{
    lv_style_t *p_style;
    if (list_empty(&HEAD_OF_STYLES)) {
        LV_LOG_USER("Style list is empty: init");
    } else {
        LV_LOG_WARN("Style list has been initialized: do nothing!");
        return;
    }

    p_style = sf_create_style_data(STY_BG);
    LV_ASSERT_NULL(p_style);
    sf_background_style_init(p_style);

    p_style = sf_create_style_data(STY_STATUS_BAR);
    LV_ASSERT_NULL(p_style);
    sf_status_bar_style_init(p_style);

    p_style = sf_create_style_data(STY_TASKBAR);
    LV_ASSERT_NULL(p_style);
    sf_taskbar_style_init(p_style);

    p_style = sf_create_style_data(STY_BG_ICON_79);
    LV_ASSERT_NULL(p_style);
    sf_app_icon_bg_style_init(p_style);

    p_style = sf_create_style_data(STY_SYM_48);
    LV_ASSERT_NULL(p_style);
    sf_app_sym_style_init(p_style);

    p_style = sf_create_style_data(STY_SYM_32);
    LV_ASSERT_NULL(p_style);
    sf_sts_sym_style_init(p_style);

    p_style = sf_create_style_data(STY_BG_ICON_37);
    LV_ASSERT_NULL(p_style);
    sf_in_app_icon_bg_style_init(p_style);

    p_style = sf_create_style_data(STY_SYM_20);
    LV_ASSERT_NULL(p_style);
    sf_in_app_sym_style_init(p_style);

    p_style = sf_create_style_data(STY_HOME_INDICATOR);
    LV_ASSERT_NULL(p_style);
    sf_home_indicator_style_init(p_style);

    p_style = sf_create_style_data(STY_SETTING_MAIN_CTR);
    LV_ASSERT_NULL(p_style);
    sf_setting_main_ctr_style_init(p_style);

    p_style = sf_create_style_data(STY_SETTING_MAIN_CHILD_CTR);
    LV_ASSERT_NULL(p_style);
    sf_setting_main_child_ctr_style_init(p_style);

    p_style = sf_create_style_data(STY_SETTING_BTN);
    LV_ASSERT_NULL(p_style);
    sf_setting_btn_style_init(p_style);
}

void sf_delete_all_style_data(void)
{
    g_style *p_style = NULL;
    g_style *p_tmp = NULL;

    list_for_each_entry_safe(p_style, p_tmp, &HEAD_OF_STYLES, node)
    if (p_style->id != 0) {
        LV_LOG_USER("Free Style id %d", p_style->id);
        list_del(&p_style->node);
        free(p_style);
    }
}

