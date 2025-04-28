/**
 * @file gmisc.h
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <lvgl.h>

/*********************
 *      DEFINES
 *********************/

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
lv_obj_t * g_create_panel(lv_obj_t *parent, lv_style_t *sty, int32_t w, int32_t h);
lv_obj_t * g_create_background(lv_obj_t *parent, lv_style_t *style, int32_t w, int32_t h);

void gf_refresh_all_layer(void);
id_data * gf_init_user_data(lv_obj_t *obj);
void gf_free_user_data(lv_obj_t *obj);

lv_obj_t * g_create_status_bar(lv_obj_t *parent);
lv_obj_t * gf_create_taskbar(lv_obj_t *parent);
void gf_hide_taskbar();
void gf_show_taskbar();
void gf_delete_taskbar();

/**********************
 *      MACROS
 **********************/


