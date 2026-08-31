#ifndef FLORA_SCREENS_H
#define FLORA_SCREENS_H

#include "flora.h"
#include "flora_constants.h"

struct FloraScreen {
	screen_callback on_init_screen;
	screen_callback on_deinit_screen;
	FloraWidget **widgets;
	int widget_count;
	int widget_capacity;
	char name[TABLE_KEY_LENGTH];
};

FloraScreen *create_screen(const char *name, screen_callback on_init_screen, screen_callback on_deinit_screen);

void update_screen(FloraScreen *screen, FloraApplicationState *state);

void render_screen(FloraScreen *screen, FloraWindow *window);

void destroy_flora_screen(FloraApplicationState *state, FloraScreen *screen);

#endif // FLORA_SCREENS_H
