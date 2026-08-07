#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flora_screens.h"
#include "flora_constants.h"

FloraScreen *create_screen(const char *name, const on_init_screen on_init_screen,
                           const on_deinit_screen on_deinit_screen) {
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

void update_screen(FloraScreen *screen, FloraApplicationState *state) {
    if (!screen) {
        fprintf(stderr, "Error: Screen is not initialized\n");
        return;
    }

    for (int i = 0; i < screen->widget_count; i++) {
        FloraWidget *widget = screen->widgets[i];
        if (widget && widget->callbacks.update && widget->is_visible) {
            widget->callbacks.update(widget, state);
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
                    // Do this backwards, last rendered is first to receive
                    for (int i = screen->widget_count - 1; i > -1; i--) {
                        FloraWidget *widget = screen->widgets[i];
                        if (widget->is_visible && widget->callbacks.on_mouse_down
                            && widget_contains_point(widget, (int) event->as.mouse_button.x,
                                                     (int) event->as.mouse_button.y)) {
                            widget->callbacks.on_mouse_down(widget, state);
                            break; // Stop after the first widget handles the event
                        }
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
                    state->running = false;
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

void render_screen(FloraScreen *screen, FloraWindow *window) {
    if (!screen || !window) {
        fprintf(stderr, "Error: Invalid screen or window.\n");
        return;
    }

    for (int i = 0; i < screen->widget_count; i++) {
        FloraWidget *widget = screen->widgets[i];
        if (widget->is_visible && widget->callbacks.render) {
            widget->callbacks.render(widget, window);
        }
    }
}

void deinit_screen(FloraApplicationState *state, FloraScreen *screen) {
    (void) state;
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
        printf("Log: Screen destroyed successfully\n");
    }
}
