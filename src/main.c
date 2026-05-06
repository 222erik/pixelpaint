#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <stdio.h>
#include "canva.h"
#include "magicnumbers.h"
#include "window.h"

void delay();

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

    struct Window colorWindow = {.construct = windowConstruct, .destruct = windowDestruct};
    colorWindow.construct(&colorWindow, "color picker", 768, 300); // 768 = 256 * 3

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
    SDL_Color chosenColor = {255, 0, 0};
    SDL_Color chosenColorSaturation = {255, 255, 30};
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

        count = 0;
        int height = colorWindow.height / 3; // The color section gets a third of the window
        int colorChange = 2;
        // From red to yellow
        for (int color = 0; color < 256; color += colorChange) {
            SDL_SetRenderDrawColor(colorWindow.renderer, 255, color, 0, 255);
            SDL_RenderLine(colorWindow.renderer, count, 0, count,
                           height); // Draw a vertical line with the right color
            count++;
        }
        // From yellow to green
        for (int color = 255; color >= 0; color -= colorChange) {
            SDL_SetRenderDrawColor(colorWindow.renderer, color, 255, 0, 255);
            SDL_RenderLine(colorWindow.renderer, count, 0, count,
                           height); // Draw a vertical line with the right color
            count++;
        }
        // From green to cyan
        for (int color = 0; color < 256; color += colorChange) {
            SDL_SetRenderDrawColor(colorWindow.renderer, 0, 255, color, 255);
            SDL_RenderLine(colorWindow.renderer, count, 0, count, height);
            count++;
        }
        // From cyan to blue
        for (int color = 255; color >= 0; color -= colorChange) {
            SDL_SetRenderDrawColor(colorWindow.renderer, 0, color, 255, 255);
            SDL_RenderLine(colorWindow.renderer, count, 0, count, height);
            count++;
        }
        // From blue to magenta
        for (int color = 0; color < 256; color += colorChange) {
            SDL_SetRenderDrawColor(colorWindow.renderer, color, 0, 255, 255);
            SDL_RenderLine(colorWindow.renderer, count, 0, count, height);
            count++;
        }
        // From magenta to red
        for (int color = 255; color >= 0; color -= colorChange) {
            SDL_SetRenderDrawColor(colorWindow.renderer, 255, 0, color, 255);
            SDL_RenderLine(colorWindow.renderer, count, 0, count, height);
            count++;
        }

        // Saturation (white to chosenColor)
        count = 0;
        for (int saturation = 0; saturation < 256; saturation++) {
            SDL_SetRenderDrawColor(
                colorWindow.renderer,
                chosenColor.r + ((255 - chosenColor.r) * (255 - saturation) / 256),
                chosenColor.g + ((255 - chosenColor.g) * (255 - saturation) / 256),
                chosenColor.b + ((255 - chosenColor.b) * (255 - saturation) / 256), 255);
            for (int i = 0; i < 6 / colorChange; ++i) {
                SDL_RenderLine(colorWindow.renderer, count + i, height, count + i, height * 2);
            }
            count += 6 / colorChange;
        }

        // Brightness (black to chosenColorSaturation)
        count = 0;
        for (int brightness = 0; brightness < 256; brightness++) {
            SDL_SetRenderDrawColor(colorWindow.renderer,
                                   (chosenColorSaturation.r * brightness / 255),
                                   (chosenColorSaturation.g * brightness / 255),
                                   (chosenColorSaturation.b * brightness / 255), 255);
            for (int i = 0; i < 6 / colorChange; ++i) {
                SDL_RenderLine(colorWindow.renderer, count + i, height * 2, count + i, height * 3);
            }
            count += 6 / colorChange;
        }

        SDL_RenderPresent(mainWindow.renderer);
        SDL_RenderPresent(paletteWindow.renderer);
        SDL_RenderPresent(colorWindow.renderer);

        delay();
    }

    canva.destruct(&canva);
    mainWindow.destruct(&mainWindow);
    paletteWindow.destruct(&paletteWindow);
    colorWindow.destruct(&colorWindow);
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
