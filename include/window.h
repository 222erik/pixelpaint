#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

typedef struct Window {
    SDL_Renderer *renderer;
    SDL_Window *window;

    char *title;
    int width, height;

    void (*construct)(struct Window *, char *, int, int);
    void (*destruct)(struct Window *);
} Window;

void windowConstruct(Window *, char *, int, int); // title, width, height
void windowDestruct(Window *);
