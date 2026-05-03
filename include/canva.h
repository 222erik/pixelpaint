#pragma once

#include <SDL3/SDL.h>

typedef struct Canva {
    int width, height;
    int boxV, boxH;   // How many boxes vertically and horizontally
    int paletteCount; // How many colors in palette
    int boxSize;
    SDL_Color *palette;
    SDL_Color *grid;

    void (*construct)(struct Canva *, int, int, int, SDL_Color *, int);
    void (*destruct)(struct Canva *);
    void (*colorIn)(struct Canva *, int, int, int);
} Canva;

void canvaConstruct(Canva *, int, int, int, SDL_Color *,
                    int); // width, height, palette count, palette
void canvaDestruct(Canva *);

// Fill in a pixel in the grid by the cursor position
void canvaColorIn(Canva *, int, int, int); // cursorX, cursorY, color # in palette
