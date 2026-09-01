#include "flora_windows.h"
#include <stdio.h>
#include "SDL3/SDL_video.h"
#include "flora.h"
#include "flora_apps.h"
#include "flora_screens.h"


FloraWindow *init_flora_window(FloraApplicationState *state, char *window_title, int window_width, int window_height)
{
	if (!state || !window_title) {
		fprintf(stderr, "Error: Invalid state or window title.\n");
		return NULL;
	}

	FloraWindow *window = calloc(1, sizeof(FloraWindow));
	if (!window) {
		fprintf(stderr, "Error: Failed to allocate window.\n");
		return NULL;
	}

	strncpy(window->name, window_title, TABLE_KEY_LENGTH - 1);
	window->name[TABLE_KEY_LENGTH - 1] = '\0';

	window->window = SDL_CreateWindow(window_title, window_width, window_height, SDL_WINDOW_RESIZABLE);
	if (!window->window) {
		fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
		free(window);
		return NULL;
	}

	window->window_id = SDL_GetWindowID(window->window);
	if (window->window_id == 0) {
		fprintf(stderr, "Error: Failed to get window ID.\n");
		SDL_DestroyWindow(window->window);
		free(window);
		return NULL;
	}

	window->window_width = window_width;
	window->window_height = window_height;

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

	if (FLORA_FALSE == set_table(&state->window_table, window, window->name)) {
		fprintf(stderr, "Error: Failed to register window \"%s\".\n", window->name);
		deinit_table(&window->screens);
		SDL_DestroyRenderer(window->renderer);
		SDL_DestroyWindow(window->window);
		free(window);
		return NULL;
	}

	TTF_TextEngine *text_engine = TTF_CreateSurfaceTextEngine();
	if (NULL == text_engine) {
		fprintf(stderr, "ERROR: Failed to create text engine when creating window \"%s\"", window->name);
		return NULL;
	}
	window->text_engine = text_engine;

	if (window->on_init_window) {
		window->on_init_window(state, window);
	}

	state->current_window = window;
	printf("Log: Window \"%s\" created successfully\n", window->name);
	return window;
}

void deinit_flora_window(FloraApplicationState *state, FloraWindow *window)
{
	if (!window) {
		return;
	}

	for (int i = 0; i < (int)window->screens.capacity; i++) {
		if (window->screens.entries[i].element != NULL) {
			FloraScreen *screen = (FloraScreen *)window->screens.entries[i].element;
			if (screen->on_deinit_screen) {
				screen->on_deinit_screen(state, screen);
			}
			free(screen);
			window->screens.entries[i].element = NULL;
		}
	}
	deinit_table(&window->screens);

	if (window->text_engine) {
		TTF_DestroySurfaceTextEngine(window->text_engine);
		window->text_engine = NULL;
	}
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

FloraWindow *get_flora_window(FloraApplicationState *state, char *name)
{
	if (!state || !name) {
		return NULL;
	}
	void *temp = NULL;
	if (!get_table(&state->window_table, &temp, name)) {
		return NULL;
	}
	return (FloraWindow *)temp;
}

int set_current_flora_window(FloraApplicationState *state, FloraWindow *window)
{
	if (!state || !window) {
		return FLORA_FALSE;
	}
	state->current_window = window;
	return FLORA_TRUE;
}
