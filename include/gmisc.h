/**
 * @file gmisc.h
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <lvgl.h>
#include <list.h>

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

/**********************
 *  GLOBAL VARIABLES
 **********************/
extern bool kb_visible;
extern lv_obj_t *glob_kb;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*=====================
 * Setter functions
 *====================*/
char *gf_set_name(lv_obj_t *obj, char *name);

/*=====================
 * Getter functions
 *====================*/
char *gf_get_name(lv_obj_t *obj);

/*=====================
 * Other functions
 *====================*/
lv_obj_t * gf_create_panel(lv_obj_t *parent, lv_style_t *sty, int32_t w, int32_t h);
lv_obj_t * gf_create_background(lv_obj_t *parent, lv_style_t *style, int32_t w, int32_t h);

lv_obj_t * gf_create_icon_bg(lv_obj_t *par, lv_style_t *bg_style, uint32_t bg_color);
lv_obj_t * gf_create_symbol(lv_obj_t *par, lv_style_t *symbol_style, uint32_t index);

void gf_refresh_all_layer(void);
id_data * gf_init_user_data(lv_obj_t *obj);
void gf_free_user_data(lv_obj_t *obj);

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


