#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL3_ttf/SDL_ttf.h"
#include "flora.h"
#include "flora_apps.h"
#include "flora_constants.h"
#include "flora_events.h"
#include "flora_fonts.h"
#include "flora_screens.h"
#include "flora_windows.h"
#include "table.h"

int init_flora(FloraApplicationState *app_state, FloraConfig *config)
{
	if (!app_state) {
		return FLORA_FALSE;
	}

	if (config) {
		app_state->app_name = config->app_name ? config->app_name : DEFAULT_APP_NAME;
	} else {
		app_state->app_name = DEFAULT_APP_NAME;
	}

	const int event_queue_capacity = config && config->event_queue_capacity > 0 ? config->event_queue_capacity
																				: DEFAULT_EVENT_QUEUE_CAPACITY;
	if (!init_event_queue(&app_state->event_queue, event_queue_capacity)) {
		fprintf(stderr, "Error: Failed to initialize event queue.\n");
		return FLORA_FALSE;
	}

	const int use_fonts = config ? config->use_fonts : DEFAULT_USE_FONTS;
	if (use_fonts == FLORA_TRUE) {
		if (!init_flora_fonts()) {
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

	const int font_capacity = config && config->font_table_initial_capacity > 0 ? config->font_table_initial_capacity
																				: DEFAULT_FONT_TABLE_CAPACITY;
	if (!init_table(&app_state->font_table, font_capacity)) {
		fprintf(stderr, "Error: Failed to initialize font table.\n");
		return FLORA_FALSE;
	}

	printf("Log: Application state initialized successfully\n");
	return FLORA_TRUE;
}

FloraApplicationState *create_flora(FloraConfig *config)
{
	FloraApplicationState *state = calloc(1, sizeof(FloraApplicationState));
	if (!state) {
		fprintf(stderr, "Error: Failed to allocate application state.\n");
		return NULL;
	}
	if (!init_flora(state, config)) {
		fprintf(stderr, "Error: Failed to initialize application state.\n");
		free(state);
		return NULL;
	}
	return state;
}

void destroy_flora(FloraApplicationState *state)
{
	if (!state) {
		return;
	}
	deinit_flora(state);
	free(state);
}

void resume_flora(FloraApplicationState *app_state)
{
	app_state->running = FLORA_TRUE;
}

void pause_flora(FloraApplicationState *app_state)
{
	app_state->running = FLORA_FALSE;
}

FloraWindow *get_current_flora_window(FloraApplicationState *state)
{
	if (!state) {
		return NULL;
	}
	return state->current_window;
}

void run_flora(FloraApplicationState *state)
{
	if (!state) {
		return;
	}

	while (state->running) {
		FloraWindow *window = state->current_window;
		if (!window || !window->current_screen) {
			break;
		}

		FloraScreen *screen = window->current_screen;
		const uint64_t frame_start = SDL_GetTicks();
		get_input(state);
		update_screen(screen, state);
		SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
		SDL_RenderClear(window->renderer);
		render_screen(screen, window);
		SDL_RenderPresent(window->renderer);

		const uint64_t frame_end = SDL_GetTicks();
		state->delta_time = (frame_end - frame_start) / 1000.0f;
		state->last_frame_time = frame_end;
	}
}

void deinit_flora(FloraApplicationState *state)
{
	pause_flora(state);

	deinit_flora_fonts(state);

	for (int i = 0; i < (int)state->window_table.capacity; i++) {
		if (state->window_table.entries[i].element != NULL) {
			FloraWindow *window = (FloraWindow *)state->window_table.entries[i].element;
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
