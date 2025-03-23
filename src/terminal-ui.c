#include <terminal-ui.h>

static void slider_event_cb(lv_event_t * e);
static lv_obj_t * slider_label;

void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target_obj(e);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider));
    lv_label_set_text(slider_label, buf);
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

int main(void) {
    lv_init();

    lv_display_t * display = lv_linux_drm_create();
    if (display == NULL) {
        printf("Error: Failed to initialize the display.\n");
        return -1;
    }
    lv_display_set_default(display);
    lv_linux_drm_set_file(display, "/dev/dri/card0", 32);
    lv_display_set_resolution(display, 1024, 600);


    lv_indev_t *touch = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/event1");
    if (touch == NULL) {
        printf("Error: Failed to initialize touch input device");
        return -1;
    }
    lv_indev_set_display(touch, display);



    // Create and configure the panel
    lv_obj_t * panel = lv_obj_create(lv_scr_act());
    panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(panel, 1024, 600);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0xFFFFFF), 0);

    // Create slider widget
    lv_obj_t * slider = lv_slider_create(panel);
    // lv_obj_center(slider);
    lv_obj_align(slider,  LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_size(slider, 700, 40);
    lv_slider_set_value(slider, 10, LV_ANIM_ON);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_set_style_anim_duration(slider, 2000, 0);
    /*Create a label below the slider*/
    slider_label = lv_label_create(panel);
    lv_label_set_text(slider_label, "0%");

    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);



    lv_obj_t * keyboard  = lv_keyboard_create(panel);
    lv_obj_set_size(keyboard, LV_PCT(100), LV_PCT(50));
    lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);

    while (1) {
        lv_task_handler();
        usleep(5000);
        lv_tick_inc(5);
    }

    return 0;
}

