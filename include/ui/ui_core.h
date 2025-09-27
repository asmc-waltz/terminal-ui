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

#define DIS_SCALE                       0
#define ENA_SCALE                       1
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
} type_t;

typedef enum {
    FLEX_NONE = 0,
    FLEX_ROW,
    FLEX_COLUMN,
    FLEX_END,
} flex_t;

typedef struct {
    int8_t ena_w;
    int8_t ena_h;
    int32_t w;
    int32_t h;
    int32_t pad_w; // %
    int32_t pad_h; // %
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
} scale_t;

typedef struct {
    int32_t x;
    int32_t y;
    lv_obj_t *base;
    flex_t flex;
    int8_t align;
    int8_t scale;
} align_t;


int32_t add_list_object(lv_obj_t *par, lv_obj_t *lobj);
typedef struct {
    int32_t x_mid;      // Latest center point X coordinate
    int32_t y_mid;      // Latest center point Y coordinate
    int32_t par_w;      // Parent width when x_mid was calculated
    int32_t par_h;      // Parent height when y_mid was calculated
    int32_t w;
    int32_t h;
    int8_t rot;
    int8_t scale;
} pos_t;

struct gobj_t;
typedef struct gobj_t {
    struct list_head node;
    struct list_head child;
    uint32_t id;
    lv_obj_t *obj;
    struct gobj_t *par;
    char *name;
    void *obj_data;
    type_t type;
    pos_t pos;
    align_t aln;
    scale_t scale;
} gobj_t;

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*=====================
 * Setter functions
 *====================*/
void align_gobj_fix(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                      int32_t x_ofs, int32_t y_ofs);

void set_gobj_pos(lv_obj_t *lobj, int32_t x_ofs, int32_t y_ofs);
void set_gobj_pos_center(lv_obj_t *lobj);

int32_t set_scr_rotation(int32_t rot_dir);
int32_t set_scr_size(int32_t width, int32_t height);
void set_gobj_size(lv_obj_t *lobj, int32_t w, int32_t h);
void set_gobj_size_scale(lv_obj_t *lobj, int32_t w, int32_t h);

int32_t enable_scale_w(lv_obj_t *lobj);
int32_t enable_scale_h(lv_obj_t *lobj);
int32_t disable_scale_w(lv_obj_t *lobj);
int32_t disable_scale_h(lv_obj_t *lobj);
int32_t set_obj_scale_pad_w(lv_obj_t *lobj, int32_t pad_w);
int32_t set_obj_scale_pad_h(lv_obj_t *lobj, int32_t pad_h);
void set_gobj_data(lv_obj_t *lobj, void *data);
int32_t set_gobj_list_layout(lv_obj_t *lobj, int8_t flow);
int32_t set_flex_scroll_dir(gobj_t *gobj);
int32_t update_flex_by_rot(gobj_t *gobj);

/*=====================
 * Getter functions
 *====================*/
lv_obj_t *get_obj_by_id(uint32_t req_id, struct list_head *head_lst);
lv_obj_t *get_obj_by_name(const char *name, struct list_head *head_lst);
gobj_t *get_gobj(lv_obj_t *lobj);
gobj_t *get_gobj_parent(lv_obj_t *lobj);
int32_t get_scr_rotation();
int32_t get_scr_width(void);
int32_t get_scr_height(void);
void gobj_get_size(lv_obj_t *lobj);
void *get_gobj_data(lv_obj_t *lobj);

/*=====================
 * Other functions
 *====================*/
gobj_t *register_obj(lv_obj_t *par, lv_obj_t *obj, const char *name);
int32_t remove_obj_and_child_by_name(const char *name, \
                                        struct list_head *head_lst);
int32_t remove_obj_and_child(uint32_t req_id, struct list_head *head_lst);
int32_t remove_children(gobj_t *par);
int32_t init_ui_object_ctx(ctx_t *ctx);
void destroy_ui_object_ctx(ctx_t *ctx);

lv_obj_t *create_base(lv_obj_t *par, const char *name);
lv_obj_t *create_box(lv_obj_t *par, const char *name);
lv_obj_t *create_container(lv_obj_t *par, const char *name);
lv_obj_t *create_text(lv_obj_t *par, const char *name, \
                          const lv_font_t *font, const char *txt_str);
lv_obj_t *create_text_box(lv_obj_t *par, const char *name, \
                          const lv_font_t *font, const char *str);
lv_obj_t *create_sym(lv_obj_t *par, const char *name, \
        const lv_font_t *font, const char *index);
lv_obj_t *create_symbol_box(lv_obj_t *par, const char *name, \
                            const lv_font_t *font, const char *index);

lv_obj_t *create_switch(lv_obj_t *par, const char *name);
lv_obj_t *create_textarea(lv_obj_t *par, const char *name);
lv_obj_t *create_btn(lv_obj_t *par, const char *name);
lv_obj_t *create_slider(lv_obj_t *par, const char *name);

lv_obj_t *get_box_child(lv_obj_t *lobj);

int32_t refresh_obj_tree_layout(gobj_t *gobj);
int32_t calc_gobj_rotated_size(gobj_t *gobj);

int32_t align_gobj_list_item(lv_obj_t *par, lv_obj_t *lobj, int32_t x_ofs, \
                             int32_t y_ofs);
int32_t update_list_align_by_rot(gobj_t *gobj_par);

static inline int32_t obj_height(lv_obj_t *lobj)
{
    return ((gobj_t *)lobj->user_data)->pos.h;
}

static inline int32_t obj_width(lv_obj_t *lobj)
{
    return ((gobj_t *)lobj->user_data)->pos.w;
}

static inline int32_t obj_aln_x(lv_obj_t *lobj)
{
    return abs(((gobj_t *)lobj->user_data)->aln.x);
}

static inline int32_t obj_aln_y(lv_obj_t *lobj)
{
    return abs(((gobj_t *)lobj->user_data)->aln.y);
}

static inline int32_t obj_scale_h(lv_obj_t *par)
{
    return ((gobj_t *)par->user_data)->scale.h;
}

static inline int32_t obj_scale_w(lv_obj_t *par)
{
    return ((gobj_t *)par->user_data)->scale.w;
}

static inline int32_t calc_pixels(int32_t par_size, int32_t percent)
{
    return (par_size * percent) / 100;
}

static inline int32_t calc_pixels_remaining(int32_t par_size, int32_t percent)
{
    return par_size - ((par_size * percent) / 100);
}

int32_t ui_main_init(ctx_t *ctx);
void ui_main_deinit(ctx_t *ctx);
/**********************
 *      MACROS
 **********************/

#endif /* G_UI_CORE_H */
