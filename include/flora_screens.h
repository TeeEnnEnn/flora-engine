#ifndef FLORA_SCREENS_H
#define FLORA_SCREENS_H

#include "flora_apps.h"

FloraScreen *create_screen(const char *name, on_init_screen on_init_screen, on_deinit_screen on_deinit_screen);

void update_screen(FloraScreen *screen, FloraApplicationState *state);

void render_screen(FloraScreen *screen, FloraWindow *window);

void deinit_screen(FloraApplicationState *state, FloraScreen *screen);

#endif //FLORA_SCREENS_H
