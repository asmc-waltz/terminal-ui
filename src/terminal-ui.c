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

void my_timer(lv_timer_t * timer)
{
  /* Use the user_data */
  // uint32_t * user_data = lv_timer_get_user_data(timer);
  // printf("my_timer called with user data: %d\n", *user_data);
  //
  // /* Do something with LVGL */
  // if(something_happened) {
  //   something_happened = false;
  //   lv_button_create(lv_screen_active());
  // }
    lv_tick_inc(5);

}



int main(void) {
    lv_init();
    drm_display_init();
    touch_screen_init();
 
    // style init
    style_curved_lines_init(&sty_curve);
    icons_20_style_init();
    icons_32_style_init();
    icons_48_style_init();

    // Initialize the layer
    active_layer_cfg();
    top_layer_cfg();

    // Display the home screen
    home_screen(&sty_curve);
    // Initialize the default keyboard that will always be accessible on the top layer.
    keyboard_create();


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



    // lv_example_list_1();





    static uint32_t user_data = 10;
    lv_timer_t * timer = lv_timer_create(my_timer, 5,  &user_data);
    lv_timer_ready(timer);

    while (1) {
        lv_task_handler();
        // usleep(5000);
        // lv_tick_inc(5);
    }

    return 0;
}

