/**
 * @file ui_core.h
 *
 */

#ifndef G_UI_CORE_H
#define G_UI_CORE_H
/*********************
 *      INCLUDES
 *********************/
#include <stdlib.h>
#include <stdint.h>

#include <lvgl.h>
#include <list.h>

/*********************
 *      DEFINES
 *********************/
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
    OBJ_BOX,
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
    /* direct callback definition */
    int32_t (*rot_redraw_cb)(lv_obj_t *lobj);
    /*
     * Resize callback will be called to overwrite the calculated size data
     * To keep the previous size for the original object is some situation.
     */
    void (*post_rot_resize_adjust_cb)(lv_obj_t *lobj);
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

struct g_obj;
typedef struct g_obj {
    struct list_head node;
    struct list_head child;
    uint32_t id;
    lv_obj_t *obj;
    struct g_obj *par;
    char *name;
    g_type type;
    g_pos pos;
    g_align aln;
    g_scale scale;
} g_obj;

typedef struct {
    struct list_head objs;     /* List of registered UI objects */
} g_ctx;

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*=====================
 * Setter functions
 *====================*/
void gf_set_app_ctx(g_ctx *ctx);
void gf_gobj_align_to(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                      int32_t x_ofs, int32_t y_ofs);

void gf_gobj_set_pos(lv_obj_t *lobj, int32_t x_ofs, int32_t y_ofs);

int32_t g_set_scr_rot_dir(int32_t rot_dir);
int32_t g_set_scr_size(int32_t width, int32_t height);
void gf_gobj_set_size(lv_obj_t *lobj, int32_t w, int32_t h);

int32_t gf_obj_scale_enable_w(lv_obj_t *lobj);
int32_t gf_obj_scale_enable_h(lv_obj_t *lobj);
int32_t gf_obj_scale_disable_w(lv_obj_t *lobj);
int32_t gf_obj_scale_disable_h(lv_obj_t *lobj);
int32_t gf_obj_scale_set_pad_w(lv_obj_t *lobj, int32_t pad_w);
int32_t gf_obj_scale_set_pad_h(lv_obj_t *lobj, int32_t pad_h);

/*=====================
 * Getter functions
 *====================*/
g_ctx *gf_get_app_ctx(void);
lv_obj_t * gf_get_obj(uint32_t req_id, struct list_head *head_lst);
lv_obj_t *gf_get_obj_by_name(const char *name, struct list_head *head_lst);
g_obj *get_gobj(lv_obj_t *lobj);
g_obj *get_par_gobj(lv_obj_t *lobj);
int32_t g_get_scr_rot_dir();
int32_t g_get_scr_width(void);
int32_t g_get_scr_height(void);
void gf_gobj_get_size(lv_obj_t *lobj);

/*=====================
 * Other functions
 *====================*/
g_obj *gf_register_obj(lv_obj_t *par, lv_obj_t *obj, const char *name);
int32_t gf_remove_obj_and_child_by_name(const char *name, \
                                        struct list_head *head_lst);
int32_t gf_remove_obj_and_child(uint32_t req_id, struct list_head *head_lst);
int32_t gf_remove_children(g_obj *par);
g_ctx *gf_create_app_ctx(void);
void gf_destroy_app_ctx(g_ctx *ctx);

lv_obj_t * gf_create_base(lv_obj_t *par, const char *name);
lv_obj_t * gf_create_box(lv_obj_t *par, const char *name);
lv_obj_t * gf_create_container(lv_obj_t *par, const char *name);
lv_obj_t * gf_create_text(lv_obj_t *par, const char *name, int32_t x, int32_t y, \
                             const char *txt_str);
lv_obj_t * gf_create_sym(lv_obj_t *par, const char *name, int32_t x, int32_t y, \
                         const lv_font_t *font, const char *index, \
                         lv_color_t color);
lv_obj_t * gf_create_switch(lv_obj_t *par, const char *name);
lv_obj_t * gf_create_btn(lv_obj_t *par, const char *name);
lv_obj_t * gf_create_slider(lv_obj_t *par, const char *name);

int32_t refresh_obj_tree_layout(g_obj *gobj);
int32_t g_obj_rot_calc_size(g_obj *gobj);

static inline int32_t obj_height(lv_obj_t *lobj)
{
    return ((g_obj *)lobj->user_data)->pos.h;
}

static inline int32_t obj_width(lv_obj_t *lobj)
{
    return ((g_obj *)lobj->user_data)->pos.w;
}

static inline int32_t obj_aln_x(lv_obj_t *lobj)
{
    return abs(((g_obj *)lobj->user_data)->aln.x);
}

static inline int32_t obj_aln_y(lv_obj_t *lobj)
{
    return abs(((g_obj *)lobj->user_data)->aln.y);
}

static inline int32_t obj_scale_h(lv_obj_t *par)
{
    return ((g_obj *)par->user_data)->scale.h;
}

static inline int32_t obj_scale_w(lv_obj_t *par)
{
    return ((g_obj *)par->user_data)->scale.w;
}

static inline int32_t calc_pixels(int32_t par_size, int32_t percent)
{
    return (par_size * percent) / 100;
}

static inline int32_t calc_pixels_remaining(int32_t par_size, int32_t percent)
{
    return par_size - ((par_size * percent) / 100);
}
/**********************
 *      MACROS
 **********************/

#endif /* G_UI_CORE_H */
