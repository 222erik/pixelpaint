#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <stdio.h>
#include "canva.h"
#include "window.h"

#define REFRESH_RATE 60

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

int main(int argc, char *argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init Error: %s%c", SDL_GetError(), '\n');
        return 1;
    }

    struct Window window = {.construct = windowConstruct, .destruct = windowDestruct};
    window.construct(&window, "pixelpaint", SCREEN_WIDTH, SCREEN_HEIGHT);

    struct Canva canva = {
        .construct = canvaConstruct, .destruct = canvaDestruct, .colorIn = canvaColorIn};

    {
        SDL_Color palette[4] = {
            {0, 0, 0}, {255, 0, 0}, {0, 255, 0}, {0, 0, 255}}; // black, red, green, blue
        canva.construct(&canva, SCREEN_WIDTH, SCREEN_HEIGHT, 4, palette, 100);
    }

    // SDL_HideCursor();

    SDL_Event event;
    bool running = true;
    int colorindex = 0; // First as default
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        const bool *keyboardState = SDL_GetKeyboardState(NULL);
        float cursorX, cursorY;
        SDL_GetMouseState(&cursorX, &cursorY);

        if (keyboardState[SDL_SCANCODE_1])
            colorindex = 0;
        else if (keyboardState[SDL_SCANCODE_2])
            colorindex = 1;
        else if (keyboardState[SDL_SCANCODE_3])
            colorindex = 2;
        else if (keyboardState[SDL_SCANCODE_4])
            colorindex = 3;
        if (keyboardState[SDL_SCANCODE_LSHIFT])
            canva.colorIn(&canva, cursorX, cursorY, colorindex);

        // Exit if esc is pressed
        if (keyboardState[SDL_SCANCODE_ESCAPE]) {
            running = false;
        }

        SDL_Color drawColor = canva.palette[0];
        SDL_Color targetColor;
        SDL_SetRenderDrawColor(window.renderer, drawColor.r, drawColor.g, drawColor.b, 255);
        for (int x = 0; x < canva.boxH; ++x) {
            for (int y = 0; y < canva.boxV; ++y) {
                targetColor = canva.grid[y * canva.boxH + x];

                SDL_FRect rect = {.x = x * canva.boxSize,
                                  .y = y * canva.boxSize,
                                  .w = canva.boxSize,
                                  .h = canva.boxSize};

                if (targetColor.r == drawColor.r && targetColor.g == drawColor.g &&
                    targetColor.b == drawColor.b) {
                    SDL_RenderFillRect(window.renderer, &rect);
                } else {
                    drawColor = targetColor;
                    SDL_SetRenderDrawColor(window.renderer, drawColor.r, drawColor.g, drawColor.b,
                                           255);
                    SDL_RenderFillRect(window.renderer, &rect);
                }
            }
        }
        SDL_RenderPresent(window.renderer);

        printf("%d\n", canva.grid[0].r);
    }

    canva.destruct(&canva);
    window.destruct(&window);
    SDL_Quit();
    return 0;
}

void delay() {
    static Uint32 lastUpdate = 0;
    Uint32 currentTime = SDL_GetTicks();

    if (currentTime - lastUpdate < 1000 / REFRESH_RATE) {
        SDL_Delay(1000 / REFRESH_RATE - (currentTime - lastUpdate));
    }
}
