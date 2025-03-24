#include <lvgl.h>

extern lv_obj_t *act_scr;


extern lv_obj_t *top_scr;
extern lv_obj_t * keyboard;

void top_layer_cfg();
void top_render_keyboard();
void top_delete_keyboard();

void active_layer_cfg();
void act_render_curve_bg();

void bot_render_basement_bg();
