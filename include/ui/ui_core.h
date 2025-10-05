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
#include <errno.h>

#include <lvgl.h>
#include "list.h"

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
typedef struct ctx ctx_t;

typedef enum {
    OBJ_NONE = 0,
    OBJ_BASE,
    OBJ_LAYOUT_GRID,
    OBJ_LAYOUT_FLEX,
    OBJ_GRID_CELL,
    OBJ_FLEX_CELL,
    OBJ_BOX,
    OBJ_BTN,
    OBJ_SLIDER,
    OBJ_LABEL,
    OBJ_ICON,
    OBJ_SWITCH,
    OBJ_TEXTAREA,
} type_t;

typedef enum {
    LIST_NONE = 0,
    LIST_ROW,
    LIST_COLUMN,
    LIST_END,
} list_t;

struct gobj_t;
typedef struct {
    void *internal;                     /* Internal data */
    void *sub_data;                     /* Sub data */
    type_t sub_type;
    type_t obj_type;
    int8_t rotation;
    struct gobj_t *parent;
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
} obj_data_t;

typedef struct {
    /* Objects without alignment as text and symbol */
    int32_t mid_x;                      // Latest center point X coordinate
    int32_t mid_y;                      // Latest center point Y coordinate
    int32_t par_w;      // Parent width when x_mid was calculated
    int32_t par_h;      // Parent height when y_mid was calculated
    /* Objects with alignment */
    int32_t x;
    int32_t y;
    lv_obj_t *base;
    list_t list;
    int8_t value;
    int8_t scale_x;
    int8_t scale_y;
} obj_align_t;


int32_t add_list_object(lv_obj_t *par, lv_obj_t *lobj);
typedef struct {
    int32_t w;
    int32_t h;
    int8_t par_w_pct;
    int8_t par_h_pct;
    int8_t scale_w;
    int8_t scale_h;
} obj_size_t;

typedef struct gobj_t {
    struct list_head node;
    struct list_head child;
    uint32_t id;
    lv_obj_t *obj;
    char *name;
    obj_size_t size;
    obj_align_t align;
    obj_data_t data;
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
void set_gobj_align(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                      int32_t x_ofs, int32_t y_ofs);
void set_gobj_align_scale_x(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                            int32_t x_ofs_pct, int32_t y_ofs_px);
void set_gobj_align_scale_y(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                            int32_t x_ofs_px, int32_t y_ofs_pct);
void set_gobj_align_scale(lv_obj_t *lobj, lv_obj_t *base, lv_align_t align, \
                             int32_t x_ofs_pct, int32_t y_ofs_pct);
void apply_gobj_align(lv_obj_t *lobj);

void set_gobj_pos(lv_obj_t *lobj, int32_t x_ofs, int32_t y_ofs);
void set_gobj_pos_center(lv_obj_t *lobj);

int32_t set_scr_rotation(int32_t rot_dir);
int32_t set_scr_size(int32_t width, int32_t height);
void set_gobj_size(lv_obj_t *lobj, int32_t px_x, int32_t px_y);
void set_gobj_size_scale_w(lv_obj_t *lobj, int32_t pct_x, int32_t px_y);
void set_gobj_size_scale_h(lv_obj_t *lobj, int32_t px_x, int32_t pct_y);
void set_gobj_size_scale(lv_obj_t *lobj, int32_t pct_x, int32_t pct_y);
void apply_gobj_size(lv_obj_t *lobj);

void set_gobj_data(lv_obj_t *lobj, void *data);
int32_t set_gobj_list_layout(lv_obj_t *lobj, int8_t flow);
int32_t set_list_scroll_dir(gobj_t *gobj);
int32_t update_list_by_rot(gobj_t *gobj);

/*=====================
 * Getter functions
 *====================*/
lv_obj_t *get_obj_by_id(uint32_t req_id, struct list_head *head_lst);
lv_obj_t *get_obj_by_name(const char *name, struct list_head *head_lst);
int32_t get_scr_rotation();
int32_t get_scr_width(void);
int32_t get_scr_height(void);
void *get_gobj_internal_data(lv_obj_t *lobj);
int32_t store_computed_object_size(lv_obj_t *lobj);

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

lv_obj_t *create_box(lv_obj_t *par, const char *name);
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

static inline gobj_t *get_gobj(lv_obj_t *lobj)
{
    return lobj ? (gobj_t *)lobj->user_data : NULL;
}

static inline lv_obj_t *get_lobj(gobj_t *gobj)
{
    return gobj ? (lv_obj_t *)gobj->obj : NULL;
}

static inline gobj_t *l_to_par_gobj(lv_obj_t *lobj)
{
    gobj_t *gobj = lobj ? get_gobj(lobj) : NULL;
    return gobj ? (gobj_t *)gobj->data.parent : NULL;
}

static inline gobj_t *g_to_par_gobj(gobj_t *gobj)
{
    return gobj ? (gobj_t *)gobj->data.parent : NULL;
}

static inline int32_t get_h(lv_obj_t *lobj)
{
    return (int32_t)get_gobj(lobj)->size.h;
}

static inline int32_t get_w(lv_obj_t *lobj)
{
    return (int32_t)get_gobj(lobj)->size.w;
}

static inline int32_t get_par_w(lv_obj_t *lobj)
{
    return lobj ? (int32_t)l_to_par_gobj(lobj)->size.w : 0;
}

static inline int32_t get_par_h(lv_obj_t *lobj)
{
    return lobj ? (int32_t)l_to_par_gobj(lobj)->size.h : 0;
}

static inline int32_t avail_px(int32_t par_size, int32_t percent)
{
    return par_size - ((par_size * percent) / 100);
}

static inline int32_t pct_to_px(int32_t par_pixels, int32_t percent)
{
    return (par_pixels * percent) / 100;
}

static inline int32_t px_to_pct(int32_t par_pixels, int32_t pixels)
{
    return (pixels * 100) / par_pixels;
}

static inline int32_t set_sub_type(lv_obj_t *lobj, type_t type)
{
    gobj_t *gobj;

    gobj = lobj ? get_gobj(lobj) : NULL;
    if (!gobj)
        return -EINVAL;

    gobj->data.sub_type = type;

    return 0;
}

int32_t ui_main_init(ctx_t *ctx);
void ui_main_deinit(ctx_t *ctx);
/**********************
 *      MACROS
 **********************/


lv_obj_t *test_screen(lv_obj_t *par);

#endif /* G_UI_CORE_H */
