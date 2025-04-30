#include <terminal-ui.h>

static lv_display_t *drm_disp = NULL;
static lv_indev_t *touch_scr = NULL;

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
    LV_LOG_USER("******** TERMINAL UI ********");
    g_app_data *p_data = NULL;
    p_data = calloc(sizeof(g_app_data), 1);
    LV_ASSERT_NULL(p_data);

    lv_init();
    drm_display_init();
    touch_screen_init();
 
    gf_styles_init();

    // Initialize the layer
    active_layer_cfg();
    top_layer_cfg();

    // Display the home screen
    gf_create_home_screen();
    // Initialize the default keyboard that will always be accessible on the top layer.
    gf_keyboard_create();

    // If there are no other components, we can safely clear all current style data
    sf_delete_all_style_data();

    static uint32_t user_data = 10;
    lv_timer_t * timer = lv_timer_create(my_timer, 5,  &user_data);
    lv_timer_ready(timer);

    while (1) {
        lv_task_handler();
    }

    return 0;
}

