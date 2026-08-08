#ifndef FLORA_WINDOWS_H
#define FLORA_WINDOWS_H

#include <SDL3/SDL.h>

#include "table.h"
#include "flora_constants.h"
#include "flora.h"

struct FloraWindow {
    on_init_window on_init_window;
    on_deinit_window on_deinit_window;
    SDL_Window *window;
    SDL_Renderer *renderer;
    Table screens;
    FloraScreen *current_screen;
    char name[TABLE_KEY_LENGTH];
};


#endif //FLORA_WINDOWS_H
