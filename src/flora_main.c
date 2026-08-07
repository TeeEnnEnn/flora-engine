#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "flora_constants.h"
#include "flora_apps.h"
#include "flora_events.h"
#include "flora_screens.h"
#include "flora_fonts.h"
#include "table.h"

FloraWindow* init_flora_window(FloraApplicationState* state, char* window_title) {
    if (!state || !window_title) {
        fprintf(stderr, "Error: Invalid state or window title.\n");
        return NULL;
    }

    FloraWindow* window = calloc(1, sizeof(FloraWindow));
    if (!window) {
        fprintf(stderr, "Error: Failed to allocate window.\n");
        return NULL;
    }

    strncpy(window->name, window_title, TABLE_KEY_LENGTH - 1);
    window->name[TABLE_KEY_LENGTH - 1] = '\0';

    window->window = SDL_CreateWindow(window_title, state->window_width, state->window_height,
                                      SDL_WINDOW_RESIZABLE);
    if (!window->window) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        free(window);
        return NULL;
    }

    window->renderer = SDL_CreateRenderer(window->window, NULL);
    if (!window->renderer) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window->window);
        free(window);
        return NULL;
    }

    if (!init_table(&window->screens, DEFAULT_SCREEN_TABLE_CAPACITY)) {
        fprintf(stderr, "Error: Failed to initialize screen table.\n");
        SDL_DestroyRenderer(window->renderer);
        SDL_DestroyWindow(window->window);
        free(window);
        return NULL;
    }

    if (!set_table(&state->window_table, window, window->name)) {
        fprintf(stderr, "Error: Failed to register window \"%s\".\n", window->name);
        deinit_table(&window->screens);
        SDL_DestroyRenderer(window->renderer);
        SDL_DestroyWindow(window->window);
        free(window);
        return NULL;
    }

    if (window->on_init_window) {
        window->on_init_window(state, window);
    }

    state->current_window = window;
    printf("Log: Window \"%s\" created successfully\n", window->name);
    return window;
}

void deinit_flora_window(FloraApplicationState* state, FloraWindow* window) {
    if (!window) {
        return;
    }

    for (int i = 0; i < (int) window->screens.capacity; i++) {
        if (window->screens.entries[i].element != NULL) {
            FloraScreen* screen = (FloraScreen*) window->screens.entries[i].element;
            if (screen->on_deinit_screen) {
                screen->on_deinit_screen(state, screen);
            }
            free(screen);
            window->screens.entries[i].element = NULL;
        }
    }
    deinit_table(&window->screens);

    if (window->renderer) {
        SDL_DestroyRenderer(window->renderer);
        window->renderer = NULL;
    }
    if (window->window) {
        SDL_DestroyWindow(window->window);
        window->window = NULL;
    }
    printf("Log: Window \"%s\" destroyed successfully\n", window->name);
}

FloraWindow* get_flora_window(FloraApplicationState* state, char* name) {
    if (!state || !name) {
        return NULL;
    }
    void* temp = NULL;
    if (!get_table(&state->window_table, &temp, name)) {
        return NULL;
    }
    return (FloraWindow*) temp;
}

int set_current_flora_window(FloraApplicationState* state, FloraWindow* window) {
    if (!state || !window) {
        return FLORA_FALSE;
    }
    state->current_window = window;
    return FLORA_TRUE;
}

FloraScreen* init_flora_screen(FloraApplicationState* state, FloraWindow* window, char* screen_name,
                               on_init_screen on_init_screen, on_deinit_screen on_deinit_screen) {
    if (!state || !window || !screen_name) {
        fprintf(stderr, "Error: Invalid state, window or screen name.\n");
        return NULL;
    }

    FloraScreen* screen = create_screen(screen_name, on_init_screen, on_deinit_screen);
    if (!screen) {
        return NULL;
    }

    if (!set_table(&window->screens, screen, screen->name)) {
        fprintf(stderr, "Error: Failed to register screen \"%s\".\n", screen->name);
        free(screen);
        return NULL;
    }

    printf("Log: Screen \"%s\" created successfully\n", screen->name);
    return screen;
}

void deinit_flora_screen(FloraApplicationState* state, FloraWindow* window, char* screen_name) {
    if (!state || !window || !screen_name) {
        return;
    }
    FloraScreen* screen = get_flora_screen(window, screen_name);
    if (!screen) {
        return;
    }
    if (screen->on_deinit_screen) {
        screen->on_deinit_screen(state, screen);
    }
    free(screen);
    set_table(&window->screens, NULL, screen_name);
    printf("Log: Screen \"%s\" destroyed successfully\n", screen_name);
}

FloraScreen* get_flora_screen(FloraWindow* window, char* name) {
    if (!window || !name) {
        return NULL;
    }
    void* temp = NULL;
    if (!get_table(&window->screens, &temp, name)) {
        return NULL;
    }
    return (FloraScreen*) temp;
}

int set_current_flora_screen(FloraApplicationState* state, FloraScreen* screen) {
    if (!state || !state->current_window || !screen) {
        return FLORA_FALSE;
    }

    FloraWindow* window = state->current_window;
    if (window->current_screen && window->current_screen != screen
        && window->current_screen->on_deinit_screen) {
        window->current_screen->on_deinit_screen(state, window->current_screen);
    }

    window->current_screen = screen;
    if (screen->on_init_screen) {
        screen->on_init_screen(state, screen);
    }
    printf("Log: Current screen set to \"%s\"\n", screen->name);
    return FLORA_TRUE;
}

int init_flora(FloraApplicationState* app_state, FloraConfig* config) {
    if (!app_state) {
        return FLORA_FALSE;
    }

    if (config) {
        app_state->app_name = config->app_name ? config->app_name : DEFAULT_APP_NAME;
        app_state->window_width = config->window_width > 0 ? config->window_width : DEFAULT_WINDOW_WIDTH;
        app_state->window_height = config->window_height > 0 ? config->window_height : DEFAULT_WINDOW_HEIGHT;
    } else {
        app_state->app_name = DEFAULT_APP_NAME;
        app_state->window_width = DEFAULT_WINDOW_WIDTH;
        app_state->window_height = DEFAULT_WINDOW_HEIGHT;
    }

    const int event_queue_capacity = config && config->event_queue_capacity > 0
                                         ? config->event_queue_capacity
                                         : DEFAULT_EVENT_QUEUE_CAPACITY;
    if (!init_event_queue(&app_state->event_queue, event_queue_capacity)) {
        fprintf(stderr, "Error: Failed to initialize event queue.\n");
        return FLORA_FALSE;
    }

    const int use_fonts = config ? config->use_fonts : DEFAULT_USE_FONTS;
    if (use_fonts == FLORA_TRUE) {
        if (!init_fonts()) {
            return FLORA_FALSE;
        }
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return FLORA_FALSE;
    }

    const int window_capacity = config && config->window_table_initial_capacity > 0
                                    ? config->window_table_initial_capacity
                                    : DEFAULT_WINDOW_TABLE_CAPACITY;
    if (!init_table(&app_state->window_table, window_capacity)) {
        fprintf(stderr, "Error: Failed to initialize window table.\n");
        return FLORA_FALSE;
    }

    const int font_capacity = config && config->font_table_initial_capacity > 0
                                  ? config->font_table_initial_capacity
                                  : DEFAULT_FONT_TABLE_CAPACITY;
    if (!init_table(&app_state->font_table, font_capacity)) {
        fprintf(stderr, "Error: Failed to initialize font table.\n");
        return FLORA_FALSE;
    }

    printf("Log: Application state initialized successfully\n");
    return FLORA_TRUE;
}

void resume_flora(FloraApplicationState* app_state) {
    app_state->running = true;
}

void pause_flora(FloraApplicationState* app_state) {
    app_state->running = false;
}

void deinit_flora(FloraApplicationState* state) {
    pause_flora(state);

    destroy_fonts(state);

    for (int i = 0; i < (int) state->window_table.capacity; i++) {
        if (state->window_table.entries[i].element != NULL) {
            FloraWindow* window = (FloraWindow*) state->window_table.entries[i].element;
            deinit_flora_window(state, window);
            free(window);
            state->window_table.entries[i].element = NULL;
        }
    }
    deinit_table(&state->window_table);

    if (!destroy_event_queue(&state->event_queue)) {
        fprintf(stderr, "Error: Failed to destroy event queue\n");
    }

    SDL_Quit();
    printf("Log: Application state destroyed successfully\n");
}
