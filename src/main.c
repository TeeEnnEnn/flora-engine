#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "flora_constants.h"
#include "flora_apps.h"
#include "flora_events.h"
#include "flora_screens.h"

void application_loop(FloraApplicationState *state) {
    while (state->running) {
        FloraScreen *screen = state->current_screen;
        const uint64_t frame_start = SDL_GetTicks();
        get_input(state);
        update_screen(screen, state);
        SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
        SDL_RenderClear(state->renderer);
        render_screen(screen, state);
        SDL_RenderPresent(state->renderer);

        const uint64_t frame_end = SDL_GetTicks();

        state->delta_time = (frame_end - frame_start) / 1000.0f;
        // printf("Log: Frames per second (FPS) = %f\n", 1 / state->delta_time);
        state->last_frame_time = frame_end;
    }
}

int main(const int argc, char **argv) {
    int width = INITIAL_WINDOW_WIDTH;
    int height = INITIAL_WINDOW_HEIGHT;
    const char *title = WINDOW_TITLE;
    if (argc < 4) {
        printf("Alternate Usage: %s <width> <height> <title>\n", argv[0]);
        printf("Using: %s %d %d %s\n", argv[0], width, height, "Flora Engine");
    } else {
        width = strtol(argv[1], NULL, 10);
        height = strtol(argv[2], NULL, 10);
        title = argv[3];
    }

    FloraApplicationState *state = calloc(1, sizeof(FloraApplicationState));
    if (!init_application(state, title, width, height)) {
        fprintf(stderr, "Error: Failed to initialize application\n");
        return FLORA_ENGINE_FATAL;
    }
    state->running = true;
    application_loop(state);

    if (!destroy_application(state)) {
        fprintf(stderr, "Error: Failed to destroy application\n");
        return FLORA_ENGINE_FATAL;
    }
    free(state);
    return FLORA_ENGINE_SUCCESS;
}
