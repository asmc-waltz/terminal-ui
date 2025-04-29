/**
 * @file terminal-ui.h
 *
 */

#ifndef G_TERMINAL_UI_H
#define G_TERMINAL_UI_H
/*********************
 *      INCLUDES
 *********************/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <lvgl.h>
#include <list.h>
#include <fonts.h>
#include <style.h>
#include <layers.h>
#include <gmisc.h>
#include <comm.h>
#include <style.h>

/*********************
 *      DEFINES
 *********************/
typedef struct {
    lv_display_t *p_drm_disp;
    lv_indev_t *p_touch_dev;
} terminal_data;

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

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

/**********************
 *      MACROS
 **********************/

#endif /* G_TERMINAL_UI_H */
