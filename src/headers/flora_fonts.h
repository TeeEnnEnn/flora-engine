#ifndef FLORA_FONTS_H
#define FLORA_FONTS_H

#include <SDL3_ttf/SDL_ttf.h>

#include "flora.h"

struct FloraFont {
	TTF_Font *font;
	float font_size;
	char font_path[256];
};

int init_flora_fonts();

int deinit_flora_fonts(FloraApplicationState *state);

#endif // FLORA_FONTS_H
