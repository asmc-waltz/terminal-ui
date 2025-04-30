/**
 * @file gmisc.h
 *
 */

#ifndef G_GMISC_H
#define G_GMISC_H
/*********************
 *      INCLUDES
 *********************/
#include <lvgl.h>
#include <list.h>
#include <style.h>
#include <fonts.h>

/*********************
 *      DEFINES
 *********************/
#define AIC_PHONE                       "phone"
#define AIC_MESSAGE                     "message"
#define AIC_TOOLBOX                     "toolbox"
#define AIC_SETTING                     "setting"

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    char *name;
    bool visible;
} id_data;

typedef enum {
	ID_NONE = 0,
	ID_BG,
	ID_SRC_HOME,
	ID_STATUS_BAR,
	ID_STATUS_BAR_SIGNAL_STRENGTH,
	ID_STATUS_BAR_SIGNAL_TYPE,
	ID_STATUS_BAR_ETHERNET,
	ID_STATUS_BAR_WIFI,
	ID_STATUS_BAR_CLOCK,
	ID_STATUS_BAR_ALERT,
	ID_STATUS_BAR_POWER,
	ID_TASK_BAR,
	ID_TASK_BAR_PHONE,
	ID_TASK_BAR_MESSAGE,
	ID_TASK_BAR_TOOLBOX,
	ID_TASK_BAR_SETTING,
	ID_HOME_INDICATOR,
} g_id;

typedef struct {
    struct list_head node;
    g_id id;
    lv_obj_t *obj;
    char *name;
    bool visible;
} g_obj;

typedef struct {
    struct list_head node;
    g_id id;
    lv_obj_t *obj;
    char *name;
} g_handler;

typedef struct {
    struct list_head obj_list;
    struct list_head handler_list;
} g_app_data;

/**********************
 *  GLOBAL VARIABLES
 **********************/
extern g_app_data *global_data;

extern bool kb_visible;
extern lv_obj_t *glob_kb;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*=====================
 * Setter functions
 *====================*/

/*=====================
 * Getter functions
 *====================*/

/*=====================
 * Other functions
 *====================*/
g_handler * gf_register_handler(uint32_t id, lv_event_cb_t event_cb, lv_event_code_t filter);

g_obj * gf_register_obj(lv_obj_t *obj, uint32_t id);
lv_obj_t * gf_create_obj(lv_obj_t *parent, uint32_t id);
lv_obj_t * gf_get_obj(uint32_t req_id);
lv_obj_t * gf_get_g_obj(uint32_t req_id);
void gf_remove_obj(uint32_t req_id);

lv_obj_t * gf_create_frame(lv_obj_t *parent, uint32_t id, lv_style_t *style, uint32_t w, uint32_t h);
lv_obj_t * gf_create_background(lv_obj_t *parent, int32_t w, int32_t h);

lv_obj_t * gf_create_btn_bg(lv_obj_t *par, uint32_t id, lv_style_t *bg_style, uint32_t bg_color);
lv_obj_t * gf_create_symbol(lv_obj_t *par, uint32_t id, lv_style_t *symbol_style, uint32_t index);

void gf_refresh_all_layer(void);

lv_obj_t * g_create_status_bar(lv_obj_t *parent);

lv_obj_t * gf_create_taskbar(lv_obj_t *parent);
void gf_hide_taskbar();
void gf_show_taskbar();
void gf_delete_taskbar();

lv_obj_t * gf_create_home_indicator(lv_obj_t *parent);
void gf_hide_home_indicator(void);
void gf_show_home_indicator(void);
void gf_delete_home_indicator(void);

void gf_keyboard_create();
void gf_hide_keyboard();
void gf_show_keyboard();
/**********************
 *      MACROS
 **********************/

#endif /* G_GMISC_H */
