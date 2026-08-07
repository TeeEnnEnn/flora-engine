#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flora_fonts.h"
#include "flora_apps.h"
#include "table.h"

bool init_fonts() {
    if (!TTF_Init()) {
        fprintf(stderr, "Error: Failed to initialize fonts: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

bool destroy_fonts(FloraApplicationState *state) {
    if (!state) {
        return false;
    }
    for (int i = 0; i < (int) state->font_table.capacity; i++) {
        if (state->font_table.entries[i].element != NULL) {
            FloraFont *flora_font = (FloraFont *) state->font_table.entries[i].element;
            if (flora_font->font) {
                TTF_CloseFont(flora_font->font);
            }
            free(flora_font);
            state->font_table.entries[i].element = NULL;
        }
    }
    deinit_table(&state->font_table);
    TTF_Quit();
    printf("Log: Successfully destroyed fonts.\n");
    return true;
}

TTF_Font* add_font(FloraApplicationState *state, const char *path, const float point_size) {
    if (!path) {
        fprintf(stderr, "Error: Font path not provided.\n");
        return NULL;
    }
    if (!state) {
        fprintf(stderr, "Error: Application not initialized.\n");
        return NULL;
    }

    void *existing = NULL;
    if (get_table(&state->font_table, &existing, path)) {
        FloraFont *flora_font = (FloraFont *) existing;
        if (flora_font) {
            return flora_font->font;
        }
    }

    TTF_Font *font = TTF_OpenFont(path, point_size);
    if (!font) {
        fprintf(stderr, "Error: Failed to open font \"%s\".\n", path);
        return NULL;
    }

    FloraFont *flora_font = calloc(1, sizeof(FloraFont));
    if (!flora_font) {
        TTF_CloseFont(font);
        fprintf(stderr, "Error: Failed to allocate font state.\n");
        return NULL;
    }
    flora_font->font = font;
    flora_font->font_size = point_size;
    strncpy(flora_font->font_path, path, sizeof(flora_font->font_path) - 1);
    flora_font->font_path[sizeof(flora_font->font_path) - 1] = '\0';

    if (!set_table(&state->font_table, flora_font, path)) {
        TTF_CloseFont(font);
        free(flora_font);
        fprintf(stderr, "Error: Failed to store font \"%s\".\n", path);
        return NULL;
    }

    return font;
}
