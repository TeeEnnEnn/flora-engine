#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flora.h"
#include "flora_apps.h"
#include "flora_constants.h"
#include "flora_events.h"
#include "flora_screens.h"
#include "flora_widgets.h"
#include "flora_windows.h"

FloraScreen *init_flora_screen(FloraApplicationState *state, FloraWindow *window, char *screen_name,
							   screen_callback on_init_screen, screen_callback on_deinit_screen)
{
	if (!state || !window || !screen_name) {
		fprintf(stderr, "Error: Invalid state, window or screen name.\n");
		return NULL;
	}

	FloraScreen *screen = create_screen(screen_name, on_init_screen, on_deinit_screen);
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

void deinit_flora_screen(FloraApplicationState *state, FloraWindow *window, char *screen_name)
{
	if (!state || !window || !screen_name) {
		return;
	}
	FloraScreen *screen = get_flora_screen(window, screen_name);
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

FloraScreen *get_flora_screen(FloraWindow *window, char *name)
{
	if (!window || !name) {
		return NULL;
	}
	void *temp = NULL;
	if (!get_table(&window->screens, &temp, name)) {
		return NULL;
	}
	return (FloraScreen *)temp;
}

int set_current_flora_screen(FloraApplicationState *state, FloraScreen *screen)
{
	if (!state || !state->current_window || !screen) {
		return FLORA_FALSE;
	}

	FloraWindow *window = state->current_window;
	if (window->current_screen && window->current_screen != screen && window->current_screen->on_deinit_screen) {
		window->current_screen->on_deinit_screen(state, window->current_screen);
	}

	window->current_screen = screen;
	if (screen->on_init_screen) {
		screen->on_init_screen(state, screen);
	}
	printf("Log: Current screen set to \"%s\"\n", screen->name);
	return FLORA_TRUE;
}

FloraScreen *create_screen(const char *name, const screen_callback on_init_screen,
						   const screen_callback on_deinit_screen)
{
	FloraScreen *screen = calloc(1, sizeof(FloraScreen));
	if (!screen) {
		fprintf(stderr, "Error: Failed to allocate screen\n");
		return NULL;
	}
	strncpy(screen->name, name, TABLE_KEY_LENGTH - 1);
	screen->name[TABLE_KEY_LENGTH - 1] = '\0';
	screen->on_init_screen = on_init_screen ? on_init_screen : NULL;
	screen->on_deinit_screen = on_deinit_screen ? on_deinit_screen : NULL;
	screen->widget_capacity = INITIAL_WIDGET_CAPACITY;
	screen->widget_count = 0;
	screen->widgets = calloc(screen->widget_capacity, sizeof(FloraWidget *));
	if (!screen->widgets) {
		fprintf(stderr, "Error: Failed to allocate screen widgets\n");
		free(screen);
		return NULL;
	}
	return screen;
}

void update_screen(FloraScreen *screen, FloraApplicationState *state)
{
	if (!screen) {
		fprintf(stderr, "Error: Screen is not initialized\n");
		return;
	}

	for (int i = 0; i < screen->widget_count; i++) {
		FloraWidget *widget = screen->widgets[i];
		if (widget && widget->callbacks.on_update && widget->is_visible) {
			widget->callbacks.on_update(widget, state);
		}
	}

	while (!is_event_queue_empty(&state->event_queue)) {
		FloraEvent *event;
		if (dequeue_event(&state->event_queue, &event)) {
			switch (event->type) {
			case FLORA_MOUSE_MOVE: {
				destroy_event(event);
				break;
			}
			case FLORA_MOUSE_DOWN: {
				FloraWidget *hit_widget = find_deepest_containing_widget(screen, event->as.mouse_button.x,
																		 event->as.mouse_button.y,
																		 ON_MOUSE_DOWN_CALLBACK);
				if (hit_widget != NULL) {
					hit_widget->callbacks.on_mouse_down(hit_widget, state);
					printf("Log: FLORA_MOUSE_DOWN - HIT: widget new id, %d\n", hit_widget->id);
				}

				destroy_event(event);
				break;
			}
			case FLORA_MOUSE_UP: {
				destroy_event(event);
				break;
			}
			case FLORA_KEY_DOWN: {
				switch (event->as.keyboard.key) {
				case SDLK_ESCAPE: {
					state->running = !state->running;
					break;
				}
				default: {
					printf("Log: Key Pressed: %s\n", SDL_GetKeyName(event->as.keyboard.key));
					break;
				}
				}
				destroy_event(event);
				break;
			}
			case FLORA_KEY_UP: {
				destroy_event(event);
				break;
			}
			case FLORA_QUIT: {
				state->running = FLORA_FALSE;
				destroy_event(event);
				break;
			}
			case FLORA_WINDOW_RESIZED: {
				printf("Log: FLORA_WINDOW_RESIZED - %dx%d\n", event->as.window_event.window_width,
					   event->as.window_event.window_height);

				// mark all the widgets as dirty
				for (int i = 0; i < screen->widget_count; i++) {
					screen->widgets[i]->is_dirty = FLORA_TRUE;
				}
				destroy_event(event);
				break;
			}
			case FLORA_UNHANDLED: {
				printf("Log: Unhandled Event: %d\n", event->type);
				destroy_event(event);
				break;
			}
			default: {
				printf("Log: Unsupported event type: %d\n", event->type);
				destroy_event(event);
				break;
			}
			}
		} else {
			fprintf(stderr, "Error: Failed to dequeue event.\n");
		}
	}
}

void render_screen(FloraScreen *screen, FloraWindow *window)
{
	if (!screen || !window) {
		fprintf(stderr, "Error: Invalid screen or window.\n");
		return;
	}

	for (int i = 0; i < screen->widget_count; i++) {
		FloraWidget *widget = screen->widgets[i];
		if (widget->parent != NULL)
			continue; // only run render for parents
		if (widget->is_visible == FLORA_FALSE)
			continue; // skip roots that are not visible and do not have a render function

		if (widget->is_dirty == FLORA_TRUE) {
			layout_widget(widget);
		}
		render_widget(window, widget);
	}

	// reset dirty flag
	for (int i = 0; i < screen->widget_count; i++) {
		FloraWidget *widget = screen->widgets[i];
		widget->is_dirty = FLORA_FALSE;
	}
}

void destroy_flora_screen(FloraApplicationState *state, FloraScreen *screen)
{
	(void)state;
	if (!screen) {
		fprintf(stderr, "Error: Screen is not initialized\n");
		return;
	}
	if (screen->widgets) {
		for (int i = 0; i < screen->widget_count; i++) {
			cleanup_widget(screen->widgets[i]);
			free(screen->widgets[i]);
			screen->widgets[i] = NULL;
		}
		free(screen->widgets);
		screen->widgets = NULL;
		screen->widget_count = 0;
		screen->widget_capacity = 0;
		printf("Log: Screen destroyed successfully\n");
	}
}
