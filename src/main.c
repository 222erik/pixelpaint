#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <stdio.h>
#include <stdlib.h>
#include "canva.h"
#include "magicnumbers.h"
#include "window.h"

void delay();
void generateColorMap(SDL_Color *map); // Generates a map of all the colors (used in the color bar)
void updateChosenColors(SDL_Color *, SDL_Color *, SDL_Color *, int, int, int,
                        int); // Last two arguments are the width and height of the bars (all the
                              // bars are the same size)

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
    // This window is constructed later

    struct Canva canva = {.construct = canvaConstruct,
                          .destruct = canvaDestruct,
                          .colorIn = canvaColorIn,
                          .saveToPPM = canvaSaveToPPM};

    {
        SDL_Color palette[PALETTE_COLORS] = {
            {0, 0, 0}, {255, 0, 0}, {0, 255, 0}, {0, 0, 255}}; // black, red, green, blue
        canva.construct(&canva, SCREEN_WIDTH, SCREEN_HEIGHT, PALETTE_COLORS, palette, BOX_SIZE);
    }

    SDL_Color *colorMap = malloc(256 * 6 * sizeof(SDL_Color));
    generateColorMap(colorMap);

    // SDL_HideCursor();

    {
        SDL_Event event;
        bool running = true;
        int colorindex = 0; // Index of color in palette that is used to draw (first as default)
        SDL_Color chosenColor = {255, 0,
                                 0}; // The color in the colorWindow that the user has picked
        SDL_Color chosenSaturation = {
            255, 255, 30}; // The color combined with the satureation that the user has picked
        SDL_Color chosenBrightness; // Color, saturation, and brightness combined
        int colorScale = 2; // This devides the length of the color bar with 2 (the bar will be too
                            // big if it isn't shrinked) (it scales down)
        int saturationAndBrightnessScale =
            6 / colorScale; // Same thing as colorScale, but for saturation and brightness
                            // bar (color bar is 6 times big!) (scales up)
        colorWindow.construct(&colorWindow, "color picker", 256 * 6 / colorScale,
                              300); // This needs to be constructed here
        int height =
            colorWindow.height /
            3; // How high the bars will be (one bar each to color, saturation, and brightness)

        while (running) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT) {
                    running = false;
                }
            }

            {
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
                if (keyboardState[SDL_SCANCODE_LALT])
                    updateChosenColors(&chosenColor, &chosenSaturation, &chosenBrightness, cursorX,
                                       cursorY, colorWindow.width, height);
                if (keyboardState[SDL_SCANCODE_LCTRL] && keyboardState[SDL_SCANCODE_S]) {
                    canva.saveToPPM(&canva, "image.ppm");
                }

                // Exit if esc is pressed
                if (keyboardState[SDL_SCANCODE_ESCAPE]) {
                    running = false;
                }
            }

            SDL_Color drawColor = canva.palette[0];
            SDL_SetRenderDrawColor(mainWindow.renderer, drawColor.r, drawColor.g, drawColor.b, 255);
            for (int x = 0; x < canva.boxH; ++x) {
                for (int y = 0; y < canva.boxV; ++y) {
                    SDL_Color boxColor = canva.grid[y * canva.boxH + x];

                    SDL_FRect rect = {.x = x * canva.boxSize,
                                      .y = y * canva.boxSize,
                                      .w = canva.boxSize,
                                      .h = canva.boxSize};

                    if (boxColor.r == drawColor.r && boxColor.g == drawColor.g &&
                        boxColor.b == drawColor.b) {
                        SDL_RenderFillRect(mainWindow.renderer, &rect);
                    } else {
                        drawColor =
                            boxColor; // Change drawColor if it doesn't match with the boxColor
                        SDL_SetRenderDrawColor(mainWindow.renderer, drawColor.r, drawColor.g,
                                               drawColor.b, 255);
                        SDL_RenderFillRect(mainWindow.renderer, &rect);
                    }
                }
            }

            {
                int x = 0, y = 0; // X and Y pos of the box to be drawn in the paletteWindow
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
            }

            {
                for (int count = 0; count < colorWindow.width; count++) {
                    SDL_Color color = colorMap[count * colorScale];
                    SDL_SetRenderDrawColor(colorWindow.renderer, color.r, color.g, color.b, 255);
                    SDL_RenderLine(colorWindow.renderer, count, 0, count,
                                   height); // Draw a vertical line with the right color
                }
            }

            {
                // Saturation (white to chosenColor)
                for (int saturation = 0; saturation < 256; saturation++) {
                    SDL_SetRenderDrawColor(
                        colorWindow.renderer,
                        chosenColor.r + ((255 - chosenColor.r) * (255 - saturation) / 255),
                        chosenColor.g + ((255 - chosenColor.g) * (255 - saturation) / 255),
                        chosenColor.b + ((255 - chosenColor.b) * (255 - saturation) / 255), 255);
                    for (int i = 0; i < saturationAndBrightnessScale; ++i) {
                        SDL_RenderLine(colorWindow.renderer,
                                       saturation * saturationAndBrightnessScale + i, height,
                                       saturation * saturationAndBrightnessScale + i, height * 2);
                    }
                }
            }

            {
                // Brightness (black to chosenColorSaturation)
                for (int brightness = 0; brightness < 256; brightness++) {
                    SDL_SetRenderDrawColor(colorWindow.renderer,
                                           (chosenSaturation.r * brightness / 255),
                                           (chosenSaturation.g * brightness / 255),
                                           (chosenSaturation.b * brightness / 255), 255);
                    for (int i = 0; i < saturationAndBrightnessScale; ++i) {
                        SDL_RenderLine(colorWindow.renderer,
                                       brightness * saturationAndBrightnessScale + i, height * 2,
                                       brightness * saturationAndBrightnessScale + i, height * 3);
                    }
                }
            }

            SDL_RenderPresent(mainWindow.renderer);
            SDL_RenderPresent(paletteWindow.renderer);
            SDL_RenderPresent(colorWindow.renderer);

            delay();
        }
    }

    canva.destruct(&canva);
    mainWindow.destruct(&mainWindow);
    paletteWindow.destruct(&paletteWindow);
    colorWindow.destruct(&colorWindow);

    free(colorMap);

    SDL_Quit();
    return 0;
}

void delay() {
    static Uint32 lastUpdate = 0;
    Uint32 currentTime = SDL_GetTicks();

    if (currentTime - lastUpdate < 1000 / REFRESH_RATE) {
        SDL_Delay(1000 / REFRESH_RATE - (currentTime - lastUpdate));
    }
    lastUpdate = SDL_GetTicks();
}

void generateColorMap(SDL_Color *map) {
    int count = 0;
    // From red to yellow
    for (int color = 0; color < 256; color++) {
        (map + count)->r = 255;
        (map + count)->g = color;
        (map + count)->b = 0;
        count++;
    }
    // From yellow to green
    for (int color = 255; color >= 0; color--) {
        (map + count)->r = color;
        (map + count)->g = 255;
        (map + count)->b = 0;
        count++;
    }
    // From green to cyan
    for (int color = 0; color < 256; color++) {
        (map + count)->r = 0;
        (map + count)->g = 255;
        (map + count)->b = color;
        count++;
    }
    // From cyan to blue
    for (int color = 255; color >= 0; color--) {
        (map + count)->r = 0;
        (map + count)->g = color;
        (map + count)->b = 255;
        count++;
    }
    // From blue to magenta
    for (int color = 0; color < 256; color++) {
        (map + count)->r = color;
        (map + count)->g = 0;
        (map + count)->b = 255;
        count++;
    }
    // From magenta to red
    for (int color = 255; color >= 0; color--) {
        (map + count)->r = 255;
        (map + count)->g = 0;
        (map + count)->b = color;
        count++;
    }
}

void updateChosenColors(SDL_Color *chosenColor, SDL_Color *chosenSaturation,
                        SDL_Color *chosenBrightness, int cursorX, int cursorY, int width,
                        int height) {
    if (cursorY < height) {
    }
}
