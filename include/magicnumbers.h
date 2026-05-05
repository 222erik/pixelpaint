#pragma once
#include <SDL3/SDL.h>

// This file only contains macros used by main.c
#define REFRESH_RATE 60
#define BOX_SIZE 100
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define PALETTE_COLORS 4 // Number of colors in palette
#define PALETTE_SCREEN_WIDTH_LEN                                                                   \
    2 // How many boxes of colors will be shown horizontally in the palette window
#define PALETTE_SCREEN_HEIGHT_LEN                                                                  \
    ((PALETTE_COLORS + PALETTE_SCREEN_WIDTH_LEN - 1) /                                             \
     PALETTE_SCREEN_WIDTH_LEN) // Boxes vertically (how many rows of boxes)
