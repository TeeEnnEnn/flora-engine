#ifndef FLORA_APPS_H
#define FLORA_APPS_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>
#include <stdint.h>

#include "flora_constants.h"
#include "flora_widgets.h"
#include "table.h"

struct FloraApplicationState {
    SDL_Window *window;
    SDL_Renderer *renderer;

    TTF_Font *font;
    float font_size;

    uint64_t last_frame_time;
    double delta_time;
    int window_width;
    int window_height;
    bool running;
    EventQueue event_queue;
    Table screen_table;
    FloraScreen *current_screen;
};

typedef void (*on_create_screen)(FloraApplicationState *state, FloraScreen *screen);

typedef void (*on_destroy_screen)(FloraApplicationState *state, FloraScreen *screen);

struct FloraScreen {
    on_create_screen on_create_screen;
    on_destroy_screen on_destroy_screen;
    FloraWidget **widgets;
    int widget_count;
    int widget_capacity;
    char name[TABLE_KEY_LENGTH];
};


bool init_application(FloraApplicationState *state, const char *title, int width, int height);

bool destroy_application(FloraApplicationState *state);

bool create_window(FloraApplicationState *state, const char *title);

bool destroy_window(FloraApplicationState *state);

/**
 * Set a named screen as the current screen
 * @param state The flora application state
 * @param name The name of the screen to set as current
 * @return true if the named screen is set as current, false otherwise
 */
bool set_current_screen(FloraApplicationState *state, const char *name);


/**
 * Add a screen to the application state
 * @param state The flora application state
 * @param screen The new screen to be added to the state's screen table
 * @return true if the screen is added successfully, false otherwise
 */
bool add_screen(FloraApplicationState *state, FloraScreen *screen);

#endif //FLORA_APPS_H
