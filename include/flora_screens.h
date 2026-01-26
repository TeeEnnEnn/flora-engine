#ifndef FLORA_SCREENS_H
#define FLORA_SCREENS_H

#include "flora_apps.h"

bool destroy_flora_screen(FloraScreen *screen);

void update_screen(FloraScreen *screen, FloraApplicationState *state);

void render_screen(FloraScreen *screen, FloraApplicationState *state);

FloraScreen *create_screen(const char *name, on_create_screen on_create_screen, on_destroy_screen on_destroy_screen);

void base_destroy_screen(FloraApplicationState *state, FloraScreen *screen);

void base_create_screen(FloraApplicationState *state, FloraScreen *screen);


#endif //FLORA_SCREENS_H
