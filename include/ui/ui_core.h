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
#include "list.h"
#include "main.h"

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
    OBJ_TEXTAREA,
} g_type;

typedef struct {
    int8_t ena_w;
    int8_t ena_h;
    int32_t w;
    int32_t h;
    int32_t pad_w;
    int32_t pad_h;
    /*
     * For some objects like the keyboard, the size and ratio are different
     * between horizontal and vertical modes. Therefore, we must redraw the
     * object to a compatible ratio before performing the component rotation.
     */
    int32_t (*pre_rot_redraw_cb)(lv_obj_t *lobj);
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
    void *obj_data;
    g_type type;
    g_pos pos;
    g_align aln;
    g_scale scale;
} g_obj;

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*=====================
 * Setter functions
 *====================*/
void align_gobj_to(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                      int32_t x_ofs, int32_t y_ofs);

void set_gobj_pos(lv_obj_t *lobj, int32_t x_ofs, int32_t y_ofs);
void set_gobj_pos_center(lv_obj_t *lobj);

int32_t set_scr_rotation(int32_t rot_dir);
int32_t set_scr_size(int32_t width, int32_t height);
void set_gobj_size(lv_obj_t *lobj, int32_t w, int32_t h);

int32_t enable_scale_w(lv_obj_t *lobj);
int32_t enable_scale_h(lv_obj_t *lobj);
int32_t disable_scale_w(lv_obj_t *lobj);
int32_t disable_scale_h(lv_obj_t *lobj);
int32_t set_obj_scale_pad_w(lv_obj_t *lobj, int32_t pad_w);
int32_t set_obj_scale_pad_h(lv_obj_t *lobj, int32_t pad_h);
void set_gobj_data(lv_obj_t *lobj, void *data);

/*=====================
 * Getter functions
 *====================*/
lv_obj_t *get_obj_by_id(uint32_t req_id, struct list_head *head_lst);
lv_obj_t *get_obj_by_name(const char *name, struct list_head *head_lst);
g_obj *get_gobj(lv_obj_t *lobj);
g_obj *get_gobj_parent(lv_obj_t *lobj);
int32_t get_scr_rotation();
int32_t get_scr_width(void);
int32_t get_scr_height(void);
void gobj_get_size(lv_obj_t *lobj);
void *get_gobj_data(lv_obj_t *lobj);

/*=====================
 * Other functions
 *====================*/
g_obj *register_obj(lv_obj_t *par, lv_obj_t *obj, const char *name);
int32_t remove_obj_and_child_by_name(const char *name, \
                                        struct list_head *head_lst);
int32_t remove_obj_and_child(uint32_t req_id, struct list_head *head_lst);
int32_t remove_children(g_obj *par);
int32_t init_ui_object_ctx(ctx_t *ctx);
void destroy_ui_object_ctx(ctx_t *ctx);

lv_obj_t * create_base(lv_obj_t *par, const char *name);
lv_obj_t * create_box(lv_obj_t *par, const char *name);
lv_obj_t * create_container(lv_obj_t *par, const char *name);
lv_obj_t * create_text(lv_obj_t *par, const char *name, \
                          const lv_font_t *font, const char *txt_str);
lv_obj_t *create_text_box(lv_obj_t *par, const char *name, \
                          const lv_font_t *font, const char *str);
lv_obj_t * create_sym(lv_obj_t *par, const char *name, \
        const lv_font_t *font, const char *index);
lv_obj_t *create_symbol_box(lv_obj_t *par, const char *name, \
                            const lv_font_t *font, const char *index);

lv_obj_t * create_switch(lv_obj_t *par, const char *name);
lv_obj_t * create_btn(lv_obj_t *par, const char *name);
lv_obj_t * create_slider(lv_obj_t *par, const char *name);

lv_obj_t *get_box_child(lv_obj_t *lobj);

int32_t refresh_obj_tree_layout(g_obj *gobj);
int32_t calc_gobj_rotated_size(g_obj *gobj);

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
