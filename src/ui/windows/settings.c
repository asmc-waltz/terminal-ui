/**
 * @file setting.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
// #define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL defined locally will override the global setting in this file"
#endif
#include "log.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <lvgl.h>
#include "list.h"
#include "ui/ui_core.h"
#include "ui/fonts.h"
#include "ui/comps.h"
#include "ui/windows.h"
#include "main.h"

/*********************
 *      DEFINES
 *********************/
#define SETTING_USED_HEIGHT             (TOP_BAR_PAD_TOP + TOP_BAR_HEIGHT + \
                                         SETTING_PAD_TOP + \
                                         SETTING_PAD_BOT) // %
#define SETTING_CONTAINTER_ALIGN        SETTING_PAD_TOP // %

#define SETTING_MENU_BAR_HOR_WIDTH      30 // %
#define SETTING_MENU_BAR_VER_WIDTH      40 // %

#define SETTING_MENU_HOR_ALIGN          (SETTING_PAD_TOP + SETTING_PAD_BOT) // %
#define SETTING_MENU_VER_ALIGN          (SETTING_PAD_TOP + SETTING_PAD_BOT) // %

#define SETTING_CONT_BG_COLOR           0x636D7A
#define SETTING_MENU_BG_COLOR           0x8F9DB0
#define SETTING_MENU_BTN_BG_COLOR       0xADBACC

#define SETTING_DETAIL_BG_COLOR         SETTING_MENU_BG_COLOR
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
/*
 * The setting detail menu is available only when the screen is in landscape
 * mode.
 */
static void detail_setting_post_rot_resize_adjust_cb(lv_obj_t *lobj)
{
    lv_obj_t *par;
    int32_t obj_w = 0, obj_h = 0;
    int32_t scr_rot;

    if (!lobj)
        return;

    par = lv_obj_get_parent(lobj);
    if (!par)
        return;

    scr_rot = get_scr_rotation();

    if (scr_rot == ROTATION_0 || scr_rot == ROTATION_180) {
        obj_w = avail_px(get_w(par),
                                      SETTING_PAD_LEFT +
                                      SETTING_MENU_BAR_HOR_WIDTH +
                                      SETTING_PAD_MID_HOR +
                                      SETTING_PAD_RIGHT);
        obj_h = avail_px(get_h(par),
                                      SETTING_MENU_HOR_ALIGN);
    } else if (scr_rot == ROTATION_90 || scr_rot == ROTATION_270) {
        obj_w = avail_px(get_w(par),
                                      SETTING_MENU_VER_ALIGN);
        obj_h = avail_px(get_h(par),
                                      SETTING_PAD_LEFT +
                                      SETTING_MENU_BAR_VER_WIDTH +
                                      SETTING_PAD_MID_VER +
                                      SETTING_PAD_RIGHT);
    }

    set_gobj_size(lobj, obj_w, obj_h);
}

static lv_obj_t *create_setting_detail(lv_obj_t *par)
{
    int32_t obj_w, obj_h;
    lv_obj_t *detail_box;

    if (!par)
        return NULL;

    /* Create container box for menu bar */
    detail_box = create_box(par, "pages.setting.detail_box");
    if (!detail_box)
        return NULL;

    /* Calculate menu bar size as percentage of parent size */
    obj_w = avail_px(get_w(par), \
                                  SETTING_PAD_LEFT + \
                                  SETTING_MENU_BAR_HOR_WIDTH + \
                                  SETTING_PAD_MID_HOR + \
                                  SETTING_PAD_RIGHT);
    obj_h = avail_px(get_h(par), SETTING_MENU_HOR_ALIGN);

    set_gobj_size(detail_box, obj_w, obj_h);
    lv_obj_set_style_bg_color(detail_box, \
                              lv_color_hex(SETTING_DETAIL_BG_COLOR), 0);
    set_gobj_align(detail_box, par, LV_ALIGN_RIGHT_MID, \
                     -pct_to_px(get_w(par), SETTING_PAD_RIGHT), 0);

    gobj_t *gobj = get_gobj(detail_box);
    gobj->data.post_rot_resize_adjust_cb = \
        detail_setting_post_rot_resize_adjust_cb;


    /*------------------------------------------------------------------------*/
    lv_obj_t *detail_space = create_brightness_setting(detail_box);
    /*------------------------------------------------------------------------*/

    return detail_box;
}

static void menu_bar_post_rot_resize_adjust_cb(lv_obj_t *lobj)
{
    lv_obj_t *par;
    int32_t obj_w = 0, obj_h = 0;
    int32_t scr_rot;

    if (!lobj)
        return;

    par = lv_obj_get_parent(lobj);
    if (!par)
        return;

    scr_rot = get_scr_rotation();

    if (scr_rot == ROTATION_0 || scr_rot == ROTATION_180) {
        obj_w = pct_to_px(get_w(par), SETTING_MENU_BAR_HOR_WIDTH);
        obj_h = avail_px(get_h(par),
                                      SETTING_MENU_HOR_ALIGN);
    } else if (scr_rot == ROTATION_90 || scr_rot == ROTATION_270) {
        obj_w = avail_px(get_w(par), SETTING_MENU_VER_ALIGN);
        obj_h = pct_to_px(get_h(par), SETTING_MENU_BAR_VER_WIDTH);
    }

    set_gobj_size(lobj, obj_w, obj_h);
}

static lv_obj_t *create_menu_bar_btn(lv_obj_t *par, lv_obj_t *ref, \
                                     const char *name, const char* index, \
                                     const char* str)
{
    lv_obj_t *btn_box, *sym_box, *text_box;

    btn_box = create_btn(par, name);
    if (btn_box == NULL) {
        return NULL;
    }

    sym_box = create_symbol_box(btn_box, NULL, SETTING_SYM_FONTS, index);
    if (sym_box == NULL) {
        remove_obj_and_child_by_name(name, &get_gobj(par)->child);
        return NULL;
    }

    text_box = create_text_box(btn_box, NULL, &lv_font_montserrat_24, str);
    if (text_box == NULL) {
        remove_obj_and_child_by_name(name, &get_gobj(par)->child);
        return NULL;
    }

    set_gobj_size_scale_w(btn_box, 96, pct_to_px(get_h(sym_box), 200));

    lv_obj_set_style_bg_color(btn_box, \
                              lv_color_hex(SETTING_MENU_BTN_BG_COLOR), 0);

    align_gobj_list_item(par, btn_box, 0, pct_to_px(get_w(par), 2));

    set_gobj_align(sym_box, btn_box, LV_ALIGN_TOP_LEFT, \
                     pct_to_px(get_h(btn_box), 50), \
                     (get_h(btn_box) - lv_obj_get_height(sym_box)) / 2);
    set_gobj_align(text_box, sym_box, LV_ALIGN_OUT_RIGHT_MID, \
                     pct_to_px(get_h(btn_box), 50), 0);

    return btn_box;
}


/*
 * The setting menu bar is placed on the left side of the screen by default.
 * It will expand to full screen when the screen is rotated to vertical mode.
 */
static lv_obj_t *create_menu_bar(lv_obj_t *par)
{
    int32_t obj_w, obj_h;
    lv_obj_t *menu_bar;

    if (!par)
        return NULL;

    /* Create container box for menu bar */
    menu_bar = create_box(par, SETTING_PAGE_NAME".menu_bar");
    if (!menu_bar)
        return NULL;

    /* Calculate menu bar size as percentage of parent size */
    obj_w = pct_to_px(get_w(par), SETTING_MENU_BAR_HOR_WIDTH);
    obj_h = avail_px(get_h(par), SETTING_MENU_HOR_ALIGN);

    set_gobj_size(menu_bar, obj_w, obj_h);
    lv_obj_set_style_bg_color(menu_bar, lv_color_hex(SETTING_MENU_BG_COLOR), 0);
    set_gobj_align(menu_bar, par, LV_ALIGN_LEFT_MID, \
                  pct_to_px(get_w(par), SETTING_PAD_LEFT), 0);
    /*
     * In case of the menu bar, the layout changes based on the rotation.
     * So we don't need to expand the width and height. The object size will
     * change according to the callback when the object is rotated.
     */
    gobj_t *gobj = get_gobj(menu_bar);
    gobj->data.post_rot_resize_adjust_cb = menu_bar_post_rot_resize_adjust_cb;

    // lv_obj_t *network_container = create_container(menu_bar, SETTING_PAGE_NAME".menu_bar.network");
    set_gobj_list_layout(menu_bar, FLEX_COLUMN);

    /*------------------------------------------------------------------------*/
    lv_obj_t *setting_btn;

    setting_btn = create_menu_bar_btn(menu_bar, menu_bar, \
                                      "pages.setting.menu_bar.airplane", \
                                      ICON_PLANE_SOLID, \
                                      "Airplane");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.wifi", \
                                      ICON_WIFI_SOLID, \
                                      "Wi-Fi");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.bluetooth", \
                                      ICON_SHIELD_SOLID, \
                                      "Bluetooth");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.cellular", \
                                      ICON_TOWER_CELL_SOLID, \
                                      "Cellular");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.ethernet", \
                                      ICON_ETHERNET_SOLID, \
                                      "Ethernet");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.network", \
                                      ICON_NETWORK_WIRED_SOLID, \
                                      "Network");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.filter", \
                                      ICON_FILTER_SOLID, \
                                      "Filter");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.hostspot", \
                                      ICON_SHARE_NODES_SOLID, \
                                      "Hostspot");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.timer", \
                                      ICON_HOURGLASS_START_SOLID, \
                                      "Timer");

    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.phone", \
                                      ICON_PHONE_SOLID, \
                                      "Phone");

    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.message", \
                                      ICON_COMMENT_SOLID, \
                                      "Message");

    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.brightness", \
                                      ICON_CIRCLE_HALF_STROKE_SOLID, \
                                      "Brightness");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.volume", \
                                      ICON_VOLUME_HIGH_SOLID, \
                                      "Volume");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.rotation", \
                                      ICON_ROTATE_SOLID, \
                                      "Rotation");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.alert", \
                                      ICON_BELL_SOLID, \
                                      "Alert");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.keyboard", \
                                      ICON_KEYBOARD, \
                                      "Keyboard");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.gnss", \
                                      ICON_SATELLITE_DISH_SOLID, \
                                      "GNSS");

    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.datetime", \
                                      ICON_CALENDAR_DAYS_SOLID, \
                                      "Datatime");

    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.general", \
                                      ICON_GEAR_SOLID, \
                                      "General");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.information", \
                                      ICON_CIRCLE_INFO_SOLID, \
                                      "Information");

    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.power", \
                                      ICON_PLUG_SOLID, \
                                      "Power");
    setting_btn = create_menu_bar_btn(menu_bar, setting_btn, \
                                      "pages.setting.menu_bar.poweroff", \
                                      ICON_POWER_OFF_SOLID, \
                                      "Power Off");

    /*------------------------------------------------------------------------*/

    return menu_bar;
}

static lv_obj_t *create_setting_container(lv_obj_t *par)
{
    lv_obj_t *cont;
    int32_t obj_w, obj_h;

    if (!par)
        return NULL;

    /* Create container box for the setting menu bar and detail setting */
    cont = create_box(par, SETTING_PAGE_NAME);
    if (!cont)
        return NULL;

    /* Calculate setting container size as percentage of parent size */
    obj_w = pct_to_px(get_w(par), SETTING_WIDTH);
    obj_h = avail_px(get_h(par), SETTING_USED_HEIGHT);

    set_gobj_size(cont, obj_w, obj_h);
    lv_obj_set_style_bg_color(cont, lv_color_hex(SETTING_CONT_BG_COLOR), 0);

    return cont;
}

static int32_t subtract_top_space(ctx_t *ctx, lv_obj_t *par,
                                  int32_t base, bool is_hor)
{
    int32_t result = base;

    if (!ctx->scr.now.top.obj)
        return result;

    if (is_hor) {
        result -= pct_to_px(get_h(par), ctx->scr.now.top.upper_space);
        result -= get_h(ctx->scr.now.top.obj);
        result -= pct_to_px(get_h(par), ctx->scr.now.top.under_space);
        result -= pct_to_px(get_h(par),
                              (SETTING_PAD_TOP + SETTING_PAD_BOT));

        LOG_TRACE("HOR top bar: top space %d, w %d, h %d, bot space %d",
                  pct_to_px(get_h(par), ctx->scr.now.top.upper_space),
                  get_w(ctx->scr.now.top.obj),
                  get_h(ctx->scr.now.top.obj),
                  pct_to_px(get_h(par), ctx->scr.now.top.under_space));
    } else {
        result -= pct_to_px(get_w(par), ctx->scr.now.top.upper_space);
        result -= get_w(ctx->scr.now.top.obj);
        result -= pct_to_px(get_w(par), ctx->scr.now.top.under_space);
        result -= pct_to_px(get_w(par),
                              (SETTING_PAD_TOP + SETTING_PAD_BOT));

        LOG_TRACE("VER top bar: top space %d, w %d, h %d, bot space %d",
                  pct_to_px(get_w(par), ctx->scr.now.top.upper_space),
                  get_w(ctx->scr.now.top.obj),
                  get_h(ctx->scr.now.top.obj),
                  pct_to_px(get_w(par), ctx->scr.now.top.under_space));
    }

    return result;
}

static int32_t subtract_bot_space(ctx_t *ctx, lv_obj_t *par,
                                  int32_t base, bool is_hor)
{
    int32_t result = base;

    if (!ctx->scr.now.bot.obj)
        return result;

    if (is_hor) {
        result -= pct_to_px(get_h(par), ctx->scr.now.bot.upper_space);
        result -= get_h(ctx->scr.now.bot.obj);
        result -= pct_to_px(get_h(par), ctx->scr.now.bot.under_space);

        LOG_TRACE("HOR keyboard: top space %d, w %d, h %d, bot space %d",
                  pct_to_px(get_h(par), ctx->scr.now.bot.upper_space),
                  get_w(ctx->scr.now.bot.obj),
                  get_h(ctx->scr.now.bot.obj),
                  pct_to_px(get_h(par), ctx->scr.now.bot.under_space));
    } else {
        result -= pct_to_px(get_w(par), ctx->scr.now.bot.upper_space);
        result -= get_w(ctx->scr.now.bot.obj);
        result -= pct_to_px(get_w(par), ctx->scr.now.bot.under_space);

        LOG_TRACE("VER keyboard: top space %d, w %d, h %d, bot space %d",
                  pct_to_px(get_w(par), ctx->scr.now.bot.upper_space),
                  get_w(ctx->scr.now.bot.obj),
                  get_h(ctx->scr.now.bot.obj),
                  pct_to_px(get_w(par), ctx->scr.now.bot.under_space));
    }

    return result;
}

/*--------------------------------------------------------------
 * Main adjustment function
 *-------------------------------------------------------------*/
static lv_obj_t *setting_container_post_rot_resize_adjust_cb(lv_obj_t *cont)
{
    ctx_t *ctx;
    int32_t obj_w, obj_h, scr_rot;
    lv_obj_t *par;

    ctx = get_ctx();
    if (!cont || !ctx)
        return NULL;

    par = lv_obj_get_parent(cont);
    if (!par)
        return NULL;

    scr_rot = get_scr_rotation();

    /* Refresh layout tree when rotated */
    if (scr_rot != ROTATION_0 && ctx->scr.now.bot.obj)
        refresh_obj_tree_layout(get_gobj(ctx->scr.now.bot.obj));

    if (scr_rot == ROTATION_0 || scr_rot == ROTATION_180) {
        obj_w = pct_to_px(get_w(par), SETTING_WIDTH);
        obj_h = get_h(par);

        obj_h = subtract_top_space(ctx, par, obj_h, true);
        obj_h = subtract_bot_space(ctx, par, obj_h, true);

    } else if (scr_rot == ROTATION_90 || scr_rot == ROTATION_270) {
        obj_h = pct_to_px(get_h(par), SETTING_WIDTH);
        obj_w = get_w(par);

        obj_w = subtract_top_space(ctx, par, obj_w, false);
        obj_w = subtract_bot_space(ctx, par, obj_w, false);
    }

    if (obj_w <= 0 || obj_h <= 0) {
        LOG_WARN("Invalid object size detected: width [%d] - height [%d]",
                 obj_w, obj_h);
    } else {
        LOG_TRACE("Setting container size: width [%d] - height [%d]",
                  obj_w, obj_h);
        set_gobj_size(cont, obj_w, obj_h);
    }

    return cont;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t *create_setting_window(ctx_t *ctx)
{
    lv_obj_t *par;
    lv_obj_t *setting_ctn, *menu_bar, *detail;

    if (!ctx || !ctx->scr.now.obj)
        return NULL;

    par = ctx->scr.now.obj;

    setting_ctn = create_setting_container(par);
    if (!setting_ctn)
        return NULL;

    set_gobj_align_scale(setting_ctn, ctx->scr.now.top.obj, LV_ALIGN_OUT_BOTTOM_MID, \
                     0,\
                     SETTING_CONTAINTER_ALIGN);

    get_gobj(setting_ctn)->data.post_rot_resize_adjust_cb = \
                                    setting_container_post_rot_resize_adjust_cb;

    menu_bar = create_menu_bar(setting_ctn);
    if (!menu_bar)
        goto exit_err;

    detail = create_setting_detail(setting_ctn);
    if (!detail)
        goto exit_err;

    ctx->scr.now.mid.obj = setting_ctn;
    ctx->scr.now.mid.upper_space = SETTING_CONTAINTER_ALIGN;
    ctx->scr.now.mid.under_space = 0;

    return setting_ctn;

exit_err:
    if (remove_obj_and_child_by_name(SETTING_PAGE_NAME, &get_gobj(par)->child))
        LOG_WARN("Setting container object not found");

    return NULL;
}
