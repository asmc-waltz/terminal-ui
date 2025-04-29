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
lv_style_t icons_size_20;
lv_style_t icons_size_32;
lv_style_t sts_bar_style;
lv_style_t task_bar_style;

lv_style_t bg_79;
lv_style_t sym_48;
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

    LV_ASSERT_NULL(id);
    p_style = calloc(sizeof(p_style), 1);
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

    lv_style_set_bg_opa(p_style, LV_OPA_COVER);
    lv_style_set_bg_color(p_style, lv_color_hex(0xffffff));

    lv_style_set_border_color(p_style, lv_color_hex(0x000000));
    lv_style_set_border_width(p_style, 0);
    lv_style_set_border_post(p_style, true);

    lv_style_set_shadow_color(p_style, lv_color_hex(0x000000));
    lv_style_set_shadow_width(p_style, 50);
    lv_style_set_shadow_spread(p_style, 10);

    lv_style_set_outline_color(p_style, lv_color_hex(0x000000));
    lv_style_set_outline_width(p_style, 2);
    lv_style_set_outline_pad(p_style, 5);

    lv_style_set_text_color(p_style, lv_palette_darken(LV_PALETTE_GREY, 5));
    lv_style_set_line_color(p_style, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_line_width(p_style, 1);
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_style_t * gf_get_lv_style(char style_id)
{
    g_style *p_style = NULL;

    list_for_each_entry(p_style, &HEAD_OF_STYLES, node)
    if (p_style->id != 0) {
        LV_LOG_USER("Style id is detected %d", p_style->id);
        if (p_style->id == style_id) {
            LV_LOG_USER("Style REQ id is detected %d", p_style->id);
            break;
        } else {
            // TODO
            continue;
        }
    }

    return &p_style->style;
}


void icons_20_style_init(void)
{
    lv_style_init(&icons_size_20);
    lv_style_set_text_font(&icons_size_20, &terminal_icons_20);
    lv_style_set_text_color(&icons_size_20, lv_color_hex(0x00FF00));
}

void icons_32_style_init(void)
{
    lv_style_init(&icons_size_32);
    lv_style_set_text_font(&icons_size_32, &terminal_icons_32);
    lv_style_set_text_color(&icons_size_32, lv_color_hex(0x00FF00));
}

void style_application_icons_init(void)
{
    lv_style_init(&bg_79);
    lv_style_set_radius(&bg_79, 20);
    lv_style_set_size(&bg_79, 79, 79);
    lv_style_set_outline_color(&bg_79, lv_color_hex(0x000000));
    lv_style_set_outline_width(&bg_79, 0);
    lv_style_set_outline_pad(&bg_79, 0);

    lv_style_init(&sym_48);
    lv_style_set_size(&sym_48, 48, 48);
    lv_style_set_text_font(&sym_48, &terminal_icons_48);
    lv_style_set_text_color(&sym_48, lv_color_hex(0xffffff));
}

void status_bar_style_init(void)
{
    lv_style_init(&sts_bar_style);
    lv_style_set_radius(&sts_bar_style, 20);
    lv_style_set_bg_opa(&sts_bar_style, LV_OPA_TRANSP);
    lv_style_set_bg_color(&sts_bar_style, lv_color_black());
    lv_style_set_text_font(&sts_bar_style, &terminal_icons_32);
    lv_style_set_text_color(&sts_bar_style, lv_color_black());
    lv_style_set_border_color(&sts_bar_style, lv_color_hex(0x000000));
    lv_style_set_border_width(&sts_bar_style, 0);
    lv_style_set_border_post(&sts_bar_style, true);
    lv_style_set_pad_all(&sts_bar_style, 0);
    lv_style_set_shadow_color(&sts_bar_style, lv_color_hex(0x000000));
    lv_style_set_shadow_width(&sts_bar_style, 0);
    lv_style_set_shadow_spread(&sts_bar_style, 0);
    lv_style_set_outline_color(&sts_bar_style, lv_color_hex(0x000000));
    lv_style_set_outline_width(&sts_bar_style, 0);
    lv_style_set_outline_pad(&sts_bar_style, 0);
}

void task_bar_style_init(void)
{
    lv_style_init(&task_bar_style);
    lv_style_set_radius(&task_bar_style, 30);
    lv_style_set_bg_opa(&task_bar_style, LV_OPA_TRANSP);
    lv_style_set_bg_color(&task_bar_style, lv_color_black());
    lv_style_set_text_font(&task_bar_style, &terminal_icons_48);
    lv_style_set_text_color(&task_bar_style, lv_color_white());
    lv_style_set_border_color(&task_bar_style, lv_color_hex(0x000000));
    lv_style_set_border_width(&task_bar_style, 0);
    lv_style_set_border_post(&task_bar_style, true);
    lv_style_set_pad_all(&task_bar_style, 10);
    lv_style_set_shadow_color(&task_bar_style, lv_color_hex(0x000000));
    lv_style_set_shadow_width(&task_bar_style, 0);
    lv_style_set_shadow_spread(&task_bar_style, 0);
    lv_style_set_outline_color(&task_bar_style, lv_color_hex(0x000000));
    lv_style_set_outline_width(&task_bar_style, 0);
    lv_style_set_outline_pad(&task_bar_style, 0);
}


void styles_init(void)
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

    icons_20_style_init();
    icons_32_style_init();
    style_application_icons_init();
    status_bar_style_init();
    task_bar_style_init();
}
