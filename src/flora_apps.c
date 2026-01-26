#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "flora_apps.h"
#include "SDL3/SDL_init.h"
#include "flora_events.h"
#include "flora_fonts.h"
#include "flora_screens.h"

bool create_window(FloraApplicationState *state, const char *title) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    state->window = SDL_CreateWindow(title, state->window_width, state->window_height, SDL_WINDOW_RESIZABLE);
    if (!state->window) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }
    state->renderer = SDL_CreateRenderer(state->window, NULL);
    if (!state->renderer) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(state->window);
        SDL_Quit();
        return false;
    }
    printf("Log: Window created successfully\n");
    return true;
}

bool destroy_window(FloraApplicationState *state) {
    if (!state->window) {
        fprintf(stderr, "Error: Window is not initialized\n");
        return false;
    }
    SDL_DestroyRenderer(state->renderer);
    SDL_DestroyWindow(state->window);
    state->window = NULL;
    state->renderer = NULL;
    state->running = false;

    if (!destroy_event_queue(&state->event_queue)) {
        fprintf(stderr, "Error: Failed to destroy event queue\n");
        return false;
    }

    printf("Log: Window destroyed successfully\n");
    return true;
}

bool init_application(FloraApplicationState *state, const char *title, const int width, const int height) {
    state->window_width = width;
    state->window_height = height;
    if (!create_window(state, title)) {
        fprintf(stderr, "Error: Failed to create window\n");
        return false;
    }

    FloraScreen *base_screen = create_screen(INITIAL_SCREEN_NAME, NULL, NULL);
    if (!base_screen) {
        fprintf(stderr, "Error: Failed to create screen\n");
        return false;
    }
    base_screen->on_create_screen = base_create_screen;
    base_screen->on_destroy_screen = base_destroy_screen;
    state->current_screen = base_screen;

    if (!init_table(&state->screen_table, INITIAL_TABLE_CAPACITY)) {
        fprintf(stderr, "Error: Failed to initialize screen table\n");
        return false;
    }

    if (!add_screen(state, state->current_screen)) {
        fprintf(stderr, "Error: Failed to add %s \n", state->current_screen->name);
        return false;
    }

    if (!init_event_queue(&state->event_queue, 64)) {
        fprintf(stderr, "Error: Failed to initialize event queue\n");
        return false;
    }

    if (!init_fonts()) {
        return false;
    }

    if (state->current_screen->on_create_screen) {
        state->current_screen->on_create_screen(state, state->current_screen);
    }

    printf("Log: Application state initialized successfully\n");
    return true;
}

bool destroy_application(FloraApplicationState *state) {
    destroy_fonts(state);
    for (int i = 0; i < INITIAL_TABLE_CAPACITY; i++) {
        FloraScreen *screen = (FloraScreen *) state->screen_table.entries[i].element;
        if (!screen || !screen->on_destroy_screen) {
            continue;
        }
        screen->on_destroy_screen(state, screen);
    }

    if (!destroy_window(state)) {
        fprintf(stderr, "Error: Failed to destroy window\n");
        return false;
    }
    if (!destroy_event_queue(&state->event_queue)) {
        fprintf(stderr, "Error: Failed to destroy event queue\n");
        return false;
    }

    destroy_table(&state->screen_table);
    printf("Log: Application state destroyed successfully\n");
    return true;
}

bool add_screen(FloraApplicationState *state, FloraScreen *screen) {
    if (!screen) {
        fprintf(stderr, "Error: <screen> is not initialized\n");
        return false;
    }
    if (!state) {
        fprintf(stderr, "Error: <state> is not initialized\n");
        return false;
    }
    if (screen->name[0] == '\0') {
        fprintf(stderr, "Error: <name> not provided\n");
    }
    if (!set_table(&state->screen_table, screen, screen->name)) {
        fprintf(stderr, "Error: Failed to add screen \"%s\"\n", screen->name);
        return false;
    }
    return true;
}

bool set_current_screen(FloraApplicationState *state, const char *name) {
    if (!state) {
        fprintf(stderr, "Error: <state> is not initialized\n");
        return false;
    }
    if (name[0] == '\0') {
        fprintf(stderr, "Error: <name> not provided\n");
        return false;
    }

    void *temp = NULL;
    const bool screen_exists = get_table(&state->screen_table, &temp, name);
    if (!screen_exists) {
        fprintf(stderr, "Error: Could not find screen \"%s\"\n", name);
        return false;
    }
    if (temp == NULL) {
        fprintf(stderr, "Error: Could not find screen \"%s\"\n", name);
        return false;
    }
    state->current_screen = (FloraScreen *) temp;
    state->current_screen->on_create_screen(state, state->current_screen);
    return true;
}
