#ifndef FLORA_FONTS_H
#define FLORA_FONTS_H

#include <SDL3_ttf/SDL_ttf.h>
#include "flora_events.h"


bool init_fonts();

bool destroy_fonts(FloraApplicationState *state);

TTF_Font *add_font(FloraApplicationState *state, const char *path, float point_size);

#endif // FLORA_FONTS_H
