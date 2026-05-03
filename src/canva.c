#include "canva.h"
#include <stdlib.h>

void canvaConstruct(Canva *self, int width, int height, int paletteCount, SDL_Color *palette,
                    int boxSize) {
    self->width = width;
    self->height = height;
    self->boxV = height / boxSize;
    self->boxH = width / boxSize;
    self->paletteCount = paletteCount;
    self->boxSize = boxSize;

    self->grid = malloc(sizeof(SDL_Color) * self->boxH * self->boxV);
    for (int i = 0; i < self->boxH * self->boxV; ++i)
        self->grid[i] = palette[0];
    self->palette = malloc(sizeof(SDL_Color) * paletteCount);
    memcpy(self->palette, palette, sizeof(SDL_Color) * paletteCount);
}

void canvaDestruct(Canva *self) {
    free(self->grid);
    free(self->palette);
}

void canvaColorIn(Canva *self, int cursorX, int cursorY, int colorindex) {
    self->grid[(cursorY / self->boxSize) * self->boxH + (cursorX / self->boxSize)] =
        self->palette[colorindex];
}
