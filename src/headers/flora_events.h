#ifndef FLORA_EVENTS_H
#define FLORA_EVENTS_H

#include <SDL3/SDL.h>

#include "flora.h"

void get_input(FloraApplicationState *state);

typedef enum FloraEventType{
    FLORA_MOUSE_MOVE,
    FLORA_MOUSE_DOWN,
    FLORA_MOUSE_UP,
    FLORA_KEY_DOWN,
    FLORA_KEY_UP,
    FLORA_QUIT,
    FLORA_UNHANDLED,
} FloraEventType;

typedef struct FloraEvent{
    FloraEventType type;

    union {
        SDL_MouseMotionEvent mouse_motion;
        SDL_MouseButtonEvent mouse_button;
        SDL_KeyboardEvent keyboard;
        SDL_QuitEvent quit;
    } as;
} FloraEvent;

typedef struct FloraEventQueue{
    FloraEvent **events;
    int back;
    int front;
    int capacity;
} FloraEventQueue;

bool init_event_queue(FloraEventQueue *queue, int capacity);

bool destroy_event_queue(FloraEventQueue *queue);

bool enqueue_event(FloraEventQueue *queue, FloraEvent *event);

bool dequeue_event(FloraEventQueue *queue, FloraEvent **event);

bool is_event_queue_empty(FloraEventQueue *queue);

bool destroy_event(FloraEvent *event);

#endif //FLORA_EVENTS_H
