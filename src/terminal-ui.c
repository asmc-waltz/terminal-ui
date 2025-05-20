#include <terminal-ui.h>

g_app_data *global_data = NULL;
static lv_display_t *drm_disp = NULL;
static lv_indev_t *touch_scr = NULL;

static int sf_init_drm_display() {
    drm_disp = lv_linux_drm_create();
    if (drm_disp == NULL) {
        printf("Error: Failed to initialize the display.\n");
        return LV_RESULT_INVALID;
    }
    lv_display_set_default(drm_disp);
    lv_linux_drm_set_file(drm_disp, "/dev/dri/card0", 32);
    lv_display_set_resolution(drm_disp, UI_SCR_WIDTH, UI_SCR_HEIGHT);
    return 0;
}

static int sf_init_touch_screen() {
    touch_scr = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/event1");
    if (touch_scr == NULL) {
        printf("Error: Failed to initialize touch input device");
        return LV_RESULT_INVALID;
    }
    lv_indev_set_display(touch_scr, drm_disp);
    return 0;
}

void gtimer_handler(lv_timer_t * timer)
{
    lv_tick_inc(UI_LVGL_TIMER_MS);
}

static void sf_create_common_components(void)
{
    gf_create_background(lv_layer_bottom(), 1024, 600);
    gf_create_setting_menu(lv_screen_active());
    gf_create_status_bar(lv_layer_sys());
    gf_create_taskbar(lv_layer_top());
    gf_create_home_indicator(lv_layer_sys());
    // Initialize the default keyboard that will always be accessible on the top layer.
    gf_keyboard_create();
    gf_create_control_center(lv_layer_top());
    gf_create_system_status(lv_layer_top());
}

int main(void) {
    lv_timer_t * task_timer = NULL;

    LV_LOG_USER("******** TERMINAL UI ********");
    // Global data used to manage all created objects and their associated handlers
    global_data = calloc(sizeof(g_app_data), 1);
    LV_ASSERT_NULL(global_data);
    INIT_LIST_HEAD(&global_data->obj_list);
    INIT_LIST_HEAD(&global_data->handler_list);

    // Initialize LVGL and the associated UI hardware
    lv_init();
    sf_init_drm_display();
    sf_init_touch_screen();

    task_timer = lv_timer_create(gtimer_handler, UI_LVGL_TIMER_MS,  NULL);
    if (task_timer == NULL) {
        LV_LOG_ERROR("Failed to create timer for LVGL task handler");
        return LV_RESULT_INVALID;
    }
    lv_timer_ready(task_timer);

    // Initialize LVGL layers as base components
    gf_register_obj(NULL, lv_layer_sys(), ID_LAYER_SYS);
    gf_register_obj(NULL, lv_layer_top(), ID_LAYER_TOP);
    gf_register_obj(NULL, lv_screen_active(), ID_LAYER_ACT);
    gf_register_obj(NULL, lv_layer_bottom(), ID_LAYER_BOT);
 
    // Apply the base configuration to the layers
    gf_config_active_layer();
    gf_config_top_layer();

    // Initialize component styles
    gf_styles_init();

    // Create shared components commonly displayed on screens
    sf_create_common_components();

    // Display the home screen
    gf_create_home_screen();

    while (1) {
        lv_task_handler();
        usleep(1000);
    }

    // If there are no other components, we can safely clear all current style data
    // sf_delete_all_style_data();
    return LV_RESULT_OK;
}

