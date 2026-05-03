#include "window.h"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

void windowConstruct(Window *self, char *title, int width, int height) {
    self->title = title;
    self->width = width;
    self->height = height;

    SDL_CreateWindowAndRenderer(title, width, height, 0, &self->window, &self->renderer);
}

void windowDestruct(Window *self) {
    SDL_DestroyRenderer(self->renderer);
    SDL_DestroyWindow(self->window);
}
