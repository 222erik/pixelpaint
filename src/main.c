#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <stdio.h>
#include "canva.h"
#include "magicnumbers.h"
#include "window.h"

int main(int argc, char *argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init Error: %s%c", SDL_GetError(), '\n');
        return 1;
    }

    // This window is the main window where the pixelart is shown
    struct Window mainWindow = {.construct = windowConstruct, .destruct = windowDestruct};
    mainWindow.construct(&mainWindow, "pixelpaint", SCREEN_WIDTH, SCREEN_HEIGHT);

    // This window is the color palette to remind the user which colors are
    // used
    struct Window paletteWindow = {.construct = windowConstruct, .destruct = windowDestruct};
    paletteWindow.construct(&paletteWindow, "palette", PALETTE_SCREEN_WIDTH_LEN * BOX_SIZE,
                            PALETTE_SCREEN_HEIGHT_LEN * BOX_SIZE);

    struct Canva canva = {.construct = canvaConstruct,
                          .destruct = canvaDestruct,
                          .colorIn = canvaColorIn,
                          .saveToPPM = canvaSaveToPPM};

    {
        SDL_Color palette[PALETTE_COLORS] = {
            {0, 0, 0}, {255, 0, 0}, {0, 255, 0}, {0, 0, 255}}; // black, red, green, blue
        canva.construct(&canva, SCREEN_WIDTH, SCREEN_HEIGHT, PALETTE_COLORS, palette, BOX_SIZE);
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
            canva.colorIn(&canva, cursorX, cursorY,
                          colorindex); // Paint if shift is pressed
        if (keyboardState[SDL_SCANCODE_LCTRL] && keyboardState[SDL_SCANCODE_S]) {
            canva.saveToPPM(&canva, "image.ppm");
        }

        // Exit if esc is pressed
        if (keyboardState[SDL_SCANCODE_ESCAPE]) {
            running = false;
        }

        SDL_Color drawColor = canva.palette[0];
        SDL_Color targetColor;
        SDL_SetRenderDrawColor(mainWindow.renderer, drawColor.r, drawColor.g, drawColor.b, 255);
        for (int x = 0; x < canva.boxH; ++x) {
            for (int y = 0; y < canva.boxV; ++y) {
                targetColor = canva.grid[y * canva.boxH + x];

                SDL_FRect rect = {.x = x * canva.boxSize,
                                  .y = y * canva.boxSize,
                                  .w = canva.boxSize,
                                  .h = canva.boxSize};

                if (targetColor.r == drawColor.r && targetColor.g == drawColor.g &&
                    targetColor.b == drawColor.b) {
                    SDL_RenderFillRect(mainWindow.renderer, &rect);
                } else {
                    drawColor = targetColor;
                    SDL_SetRenderDrawColor(mainWindow.renderer, drawColor.r, drawColor.g,
                                           drawColor.b, 255);
                    SDL_RenderFillRect(mainWindow.renderer, &rect);
                }
            }
        }

        int x = 0, y = 0;

        int count = 0;
        while (y < PALETTE_SCREEN_HEIGHT_LEN) {
            SDL_FRect rect = {.x = x * canva.boxSize,
                              .y = y * canva.boxSize,
                              .w = canva.boxSize,
                              .h = canva.boxSize};
            SDL_SetRenderDrawColor(paletteWindow.renderer, canva.palette[count].r,
                                   canva.palette[count].g, canva.palette[count].b, 255);
            SDL_RenderFillRect(paletteWindow.renderer, &rect);

            count++;
            x++;
            if (x >= PALETTE_SCREEN_WIDTH_LEN) {
                x = 0;
                y++;
            }
        }

        SDL_RenderPresent(mainWindow.renderer);
        SDL_RenderPresent(paletteWindow.renderer);
    }

    canva.destruct(&canva);
    mainWindow.destruct(&mainWindow);
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
