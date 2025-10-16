/* SPDX-License-Identifier: GPL-2.0 */
/*
 * color_palette_256.h - 256-level pastel color palette declarations
 *
 * This header provides extern declarations for color level tables.
 * The actual definitions are in color_palette_256.c to avoid duplicate
 * instances across translation units.
 */

#ifndef COLOR_PALETTE_256_H
#define COLOR_PALETTE_256_H

#include <stdint.h>

extern const uint32_t color_gray_levels[256];
extern const uint32_t color_gray_levels_inv[256];

extern const uint32_t color_red_levels[256];
extern const uint32_t color_red_levels_inv[256];

extern const uint32_t color_green_levels[256];
extern const uint32_t color_green_levels_inv[256];

extern const uint32_t color_blue_levels[256];
extern const uint32_t color_blue_levels_inv[256];

extern const uint32_t color_yellow_levels[256];
extern const uint32_t color_yellow_levels_inv[256];

extern const uint32_t color_cyan_levels[256];
extern const uint32_t color_cyan_levels_inv[256];

extern const uint32_t color_magenta_levels[256];
extern const uint32_t color_magenta_levels_inv[256];

extern const uint32_t color_orange_levels[256];
extern const uint32_t color_orange_levels_inv[256];

extern const uint32_t color_purple_levels[256];
extern const uint32_t color_purple_levels_inv[256];

extern const uint32_t color_pink_levels[256];
extern const uint32_t color_pink_levels_inv[256];

#endif /* COLOR_PALETTE_256_H */
