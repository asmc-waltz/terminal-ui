#include <lvgl.h>

extern lv_obj_t *act_scr;
extern bool kb_visible;

extern lv_obj_t *top_scr;
extern lv_obj_t *glob_kb;

void top_layer_cfg();
void top_keyboard_create();
void top_hidden_keyboard();
void top_show_keyboard();

void active_layer_cfg();
void render_curve_bg_act_layer();

void render_curve_bg_bot_layer();
