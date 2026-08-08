#ifndef FLORA_APPS_H
#define FLORA_APPS_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdint.h>

#include "flora.h"
#include "flora_events.h"
#include "table.h"




struct FloraApplicationState {
    /* Application Details */
    char* app_name;
    int window_width;
    int window_height;

    uint64_t last_frame_time;
    double delta_time;
    int running;
    FloraEventQueue event_queue;

    Table window_table;
    Table font_table;

    FloraWindow* current_window;

};

int init_flora(FloraApplicationState *app_state, FloraConfig *config);

void deinit_flora(FloraApplicationState *state);

FloraApplicationState *create_flora(FloraConfig *config);

void destroy_flora(FloraApplicationState *state);

void run_flora(FloraApplicationState *state);

FloraWindow *get_current_flora_window(FloraApplicationState *state);

#endif //FLORA_APPS_H
