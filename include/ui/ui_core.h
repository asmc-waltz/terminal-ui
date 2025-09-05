/**
 * @file ui_core.h
 *
 */

#ifndef G_UI_CORE_H
#define G_UI_CORE_H
/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>

#include <lvgl.h>
#include <list.h>

/*********************
 *      DEFINES
 *********************/
#define HW_DISPLAY_WIDTH                1024
#define HW_DISPLAY_HEIGHT               600

#define ROTATION_0                      LV_DISPLAY_ROTATION_0
#define ROTATION_90                     LV_DISPLAY_ROTATION_90
#define ROTATION_180                    LV_DISPLAY_ROTATION_180
#define ROTATION_270                    LV_DISPLAY_ROTATION_270

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    OBJ_NONE = 0,
    OBJ_BASE,
    OBJ_CONTAINER,
    OBJ_BTN,
    OBJ_SLIDER,
    OBJ_LABEL,
    OBJ_ICON,
    OBJ_SWITCH,
} g_type;

typedef struct {
    int8_t ena_w;
    int8_t ena_h;
    int32_t w;
    int32_t h;
    int32_t pad_w;
    int32_t pad_h;
} g_scale;

typedef struct {
    int32_t x;
    int32_t y;
    lv_obj_t *base;
    int8_t align;
} g_align;

typedef struct {
    int32_t x_mid;      // Latest center point X coordinate
    int32_t y_mid;      // Latest center point Y coordinate
    int32_t par_w;      // Parent width when x_mid was calculated
    int32_t par_h;      // Parent height when y_mid was calculated
    int32_t w;
    int32_t h;
    int8_t rot;
} g_pos;

typedef struct g_obj_t {
    struct list_head node;
    struct list_head child;
    int32_t id;
    lv_obj_t *obj;
    char *name;
    bool visible;
    g_type type;
    g_pos pos;
    g_align aln;
    g_scale scale;
} g_obj_t;

typedef struct {
    struct list_head objs;     /* List of registered UI objects */
} g_ctx_t;

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*=====================
 * Setter functions
 *====================*/
void gf_set_app_ctx(g_ctx_t *ctx);
void gf_gobj_align_to(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                      int32_t x_ofs, int32_t y_ofs);

void gf_gobj_set_pos(lv_obj_t *lobj, int32_t x_ofs, int32_t y_ofs);

int32_t g_set_scr_rot_dir(int32_t rot_dir);
int32_t g_set_scr_size(int32_t width, int32_t hight);
void gf_gobj_set_size(lv_obj_t *lobj, int32_t w, int32_t h);

int32_t gf_gobj_scale_enable_w(g_obj_t *gobj);
int32_t gf_gobj_scale_enable_h(g_obj_t *gobj);
int32_t gf_gobj_scale_disable_w(g_obj_t *gobj);
int32_t gf_gobj_scale_disable_h(g_obj_t *gobj);
int32_t gf_gobj_scale_set_pad_w(g_obj_t *gobj, int32_t pad_w);
int32_t gf_gobj_scale_set_pad_h(g_obj_t *gobj, int32_t pad_h);

/*=====================
 * Getter functions
 *====================*/
g_ctx_t *gf_get_app_ctx(void);
lv_obj_t * gf_get_obj(uint32_t req_id, struct list_head *head_lst);
int32_t g_get_scr_rot_dir();
int32_t g_get_scr_width(int32_t width, int32_t hight);
int32_t g_get_scr_hight();
void gf_gobj_get_size(lv_obj_t *lobj);

/*=====================
 * Other functions
 *====================*/
g_obj_t *gf_register_obj(lv_obj_t *par, lv_obj_t *obj, uint32_t id);
bool gf_remove_obj_and_child(uint32_t req_id, struct list_head *head_lst);
g_ctx_t *gf_create_app_ctx(void);
void gf_destroy_app_ctx(g_ctx_t *ctx);

lv_obj_t * gf_create_box(lv_obj_t *par, uint32_t id);
lv_obj_t * gf_create_container(lv_obj_t *par, uint32_t id);
lv_obj_t * gf_create_text(lv_obj_t *par, uint32_t id, int32_t x, int32_t y, \
                             const char *txt_str);
lv_obj_t * gf_create_sym(lv_obj_t *par, uint32_t id, int32_t x, int32_t y, \
                         const lv_font_t *font, const char *index, \
                         lv_color_t color);
lv_obj_t * gf_create_switch(lv_obj_t *par, uint32_t id);
lv_obj_t * gf_create_btn(lv_obj_t *par, uint32_t id);
lv_obj_t * gf_create_slider(lv_obj_t *par, uint32_t id);

int32_t gf_rotate_obj_tree(g_obj_t *gobj);
int32_t g_obj_rot_calc_size(g_obj_t *gobj);

/**********************
 *      MACROS
 **********************/

#endif /* G_UI_CORE_H */
