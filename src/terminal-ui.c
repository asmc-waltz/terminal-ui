#include <terminal-ui.h>

static lv_display_t *drm_disp = NULL;
static lv_indev_t *touch_scr = NULL;


static void slider_event_cb(lv_event_t * e);
static lv_obj_t * slider_label;

static lv_style_t sty_curve;

void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target_obj(e);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider));
    lv_label_set_text(slider_label, buf);
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

void scr_home(lv_obj_t *parent) {
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_set_size(btn, 200, 200);
    lv_obj_add_style(btn, &sty_curve, 0);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, LV_SYMBOL_WIFI " Wi-Fi");
}

int drm_display_init() {
    drm_disp = lv_linux_drm_create();
    if (drm_disp == NULL) {
        printf("Error: Failed to initialize the display.\n");
        return -1;
    }
    lv_display_set_default(drm_disp);
    lv_linux_drm_set_file(drm_disp, "/dev/dri/card0", 32);
    // lv_display_set_resolution(drm_disp, 1024, 600);
    return 0;
}

int touch_screen_init() {
    touch_scr = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/event1");
    if (touch_scr == NULL) {
        printf("Error: Failed to initialize touch input device");
        return -1;
    }
    lv_indev_set_display(touch_scr, drm_disp);
    return 0;
}

int main(void) {
    lv_init();
    drm_display_init();
    touch_screen_init();
 
    active_layer_cfg();
    bot_render_basement_bg();
    act_render_curve_bg();

    // lv_obj_t * slider = lv_slider_create(panel);
    // // lv_obj_center(slider);
    // lv_obj_align(slider,  LV_ALIGN_TOP_MID, 0, 0);
    // lv_obj_set_size(slider, 700, 40);
    // lv_slider_set_value(slider, 10, LV_ANIM_ON);
    // lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    //
    // lv_obj_set_style_anim_duration(slider, 2000, 0);
    // /*Create a label below the slider*/
    // slider_label = lv_label_create(panel);
    // lv_label_set_text(slider_label, "0%");
    //
    // lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    //
    //
    //
    // lv_obj_t * keyboard  = lv_keyboard_create(panel);
    // lv_obj_set_size(keyboard, LV_PCT(100), LV_PCT(50));
    // lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    // lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);

    // sty_init_curve(&sty_curve);
    // scr_home(panel);

    while (1) {
        lv_task_handler();
        usleep(5000);
        lv_tick_inc(5);
    }

    return 0;
}

