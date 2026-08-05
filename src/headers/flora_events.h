#ifndef FLORA_EVENTS_H
#define FLORA_EVENTS_H

#include <stdbool.h>
#include <SDL3/SDL.h>

typedef struct FloraApplicationState FloraApplicationState;
typedef struct FloraScreen FloraScreen;

void get_input(FloraApplicationState *state);

typedef enum {
    FLORA_MOUSE_MOVE,
    FLORA_MOUSE_DOWN,
    FLORA_MOUSE_UP,
    FLORA_KEY_DOWN,
    FLORA_KEY_UP,
    FLORA_QUIT,
    FLORA_UNHANDLED,
} FloraEventType;

typedef struct {
    FloraEventType type;

    union {
        SDL_MouseMotionEvent mouse_motion;
        SDL_MouseButtonEvent mouse_button;
        SDL_KeyboardEvent keyboard;
        SDL_QuitEvent quit;
    } as;
} FloraEvent;

typedef struct {
    FloraEvent **events;
    int back;
    int front;
    int capacity;
} EventQueue;

bool init_event_queue(EventQueue *queue, int capacity);

bool destroy_event_queue(EventQueue *queue);

bool enqueue_event(EventQueue *queue, FloraEvent *event);

bool dequeue_event(EventQueue *queue, FloraEvent **event);

bool is_event_queue_empty(EventQueue *queue);

bool destroy_event(FloraEvent *event);

#endif //FLORA_EVENTS_H
