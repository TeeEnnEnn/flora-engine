#include <stdlib.h>
#include <stdio.h>

#include "flora_events.h"
#include "flora_apps.h"
#include "flora_constants.h"

bool init_event_queue(EventQueue *queue, const int capacity) {
    if (!queue) {
        fprintf(stderr, "Error: Event queue is not initialized\n");
        return false;
    }
    queue->events = (FloraEvent **) malloc(capacity * sizeof(FloraEvent *));
    if (!queue->events) {
        fprintf(stderr, "Error: Failed to allocate memory for event queue\n");
        return false;
    }
    queue->capacity = capacity;
    queue->front = 0;
    queue->back = 0;
    printf("Log: Event queue initialized successfully\n");
    return true;
}

bool destroy_event_queue(EventQueue *queue) {
    if (!queue) {
        fprintf(stderr, "Error: Event queue is not initialized\n");
        return false;
    }
    int i = queue->front;
    while (i != queue->back && queue->events != NULL) {
        free(queue->events[i]);
        i = (i + 1) % queue->capacity;
    }
    if (queue->events != NULL) {
        free(queue->events);
    }
    queue->events = NULL;
    queue->back = 0;
    queue->front = 0;
    queue->capacity = 0;
    printf("Log: Event queue destroyed successfully\n");
    return true;
}

bool enqueue_event(EventQueue *queue, FloraEvent *event) {
    if (!queue || !event) {
        fprintf(stderr, "Error: Event queue or event is not initialized\n");
        return false;
    }
    int next_back = (queue->back + 1) % queue->capacity;

    if (next_back == queue->front) {
        int new_capacity = queue->capacity * GROWTH_FACTOR;
        FloraEvent **new_events = (FloraEvent **) malloc(new_capacity * sizeof(FloraEvent *));
        if (!new_events) {
            fprintf(stderr, "Error: Failed to allocate memory for new event queue\n");
            return false;
        }
        int count = 0;
        int i = queue->front;
        while (i != queue->back) {
            new_events[count++] = queue->events[i];
            i = (i + 1) % queue->capacity;
        }
        free(queue->events);
        queue->events = new_events;
        queue->capacity = new_capacity;
        queue->front = 0;
        queue->back = count;
        printf("Log: Event queue resized successfully\n");
    }
    queue->events[queue->back] = event;
    queue->back = next_back;
#ifdef EVENT_LOGS
    printf("Log: Event enqueued successfully\n");
#endif
    return true;
}

bool dequeue_event(EventQueue *queue, FloraEvent **event) {
    if (!queue || !event) {
        fprintf(stderr, "Error: Event queue or event is not initialized\n");
        return false;
    }
    if (queue->front == queue->back) {
        fprintf(stderr, "Error: Event queue is empty\n");
        return false;
    }
    *event = queue->events[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
#ifdef EVENT_LOGS
    printf("Log: Event dequeued successfully\n");
#endif
    return true;
}

bool is_event_queue_empty(EventQueue *queue) {
    if (!queue) {
        fprintf(stderr, "Error: Event queue is not initialized\n");
        return false;
    }
    return queue->front == queue->back;
}

bool destroy_event(FloraEvent *event) {
    if (!event) {
        fprintf(stderr, "Error: Event is not initialized\n");
        return false;
    }
    free(event);
#ifdef EVENT_LOGS
    printf("Log: Event destroyed successfully\n");
#endif

    return true;
}

static FloraEvent *new_flora_event(SDL_Event *sdl_event) {
    FloraEvent *event = malloc(sizeof(FloraEvent));
    if (!event) {
        fprintf(stderr, "Error: Failed to allocate memory for FloraEvent\n");
        return NULL;
    }
    if (!sdl_event) {
        fprintf(stderr, "Error: SDL event is NULL\n");
        free(event);
        return NULL;
    }
    switch (sdl_event->type) {
        case SDL_EVENT_KEY_DOWN: {
            event->type = FLORA_KEY_DOWN;
            event->as.keyboard = sdl_event->key;
            break;
        }
        case SDL_EVENT_KEY_UP: {
            event->type = FLORA_KEY_UP;
            event->as.keyboard = sdl_event->key;
            break;
        }
        case SDL_EVENT_MOUSE_MOTION: {
            event->type = FLORA_MOUSE_MOVE;
            event->as.mouse_motion = sdl_event->motion;
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            event->type = FLORA_MOUSE_DOWN;
            event->as.mouse_button = sdl_event->button;
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            event->type = FLORA_MOUSE_UP;
            event->as.mouse_button = sdl_event->button;
            break;
        }
        case SDL_EVENT_QUIT: {
            event->type = FLORA_QUIT;
            event->as.quit = sdl_event->quit;
            break;
        }
        default: {
            fprintf(stderr, "Error: unsupported SDL event type %d\n", sdl_event->type);
            event->type = FLORA_UNHANDLED;
            event->as.quit = (SDL_QuitEvent){0};
            break;
        }
    }
    return event;
}

void get_input(FloraApplicationState *state) {
    SDL_Event sdl_event;

    while (SDL_PollEvent(&sdl_event)) {
        switch (sdl_event.type) {
            case SDL_EVENT_QUIT: {
                FloraEvent *event = new_flora_event(&sdl_event);
                enqueue_event(&state->event_queue, event);
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                FloraEvent *event = new_flora_event(&sdl_event);
                enqueue_event(&state->event_queue, event);
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                FloraEvent *event = new_flora_event(&sdl_event);
                enqueue_event(&state->event_queue, event);
                break;
            }
            case SDL_EVENT_MOUSE_MOTION: {
                FloraEvent *event = new_flora_event(&sdl_event);
                enqueue_event(&state->event_queue, event);
                break;
            }
            case SDL_EVENT_KEY_DOWN: {
                FloraEvent *event = new_flora_event(&sdl_event);
                enqueue_event(&state->event_queue, event);
                break;
            }
            case SDL_EVENT_KEY_UP: {
                FloraEvent *event = new_flora_event(&sdl_event);
                enqueue_event(&state->event_queue, event);
                break;
            }
            default: { break; }
        }
    }
}
