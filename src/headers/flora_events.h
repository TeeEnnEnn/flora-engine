#ifndef FLORA_EVENTS_H
#define FLORA_EVENTS_H

#include <SDL3/SDL.h>

#include "SDL3/SDL_events.h"
#include "flora.h"

void get_input(FloraApplicationState *state);

typedef enum FloraEventType {
	FLORA_MOUSE_MOVE,
	FLORA_MOUSE_DOWN,
	FLORA_MOUSE_UP,
	FLORA_KEY_DOWN,
	FLORA_KEY_UP,
	FLORA_QUIT,
	FLORA_WINDOW_RESIZED,
	FLORA_UNHANDLED,
} FloraEventType;

typedef struct FloraEvent {
	FloraEventType type;

	union {
		SDL_MouseMotionEvent mouse_motion;
		SDL_MouseButtonEvent mouse_button;
		SDL_KeyboardEvent keyboard;
		SDL_QuitEvent quit;
		SDL_WindowEvent window_event;
	} as;
} FloraEvent;

typedef struct FloraEventQueue {
	FloraEvent **events;
	int back;
	int front;
	int capacity;
} FloraEventQueue;

int init_event_queue(FloraEventQueue *queue, int capacity);

int destroy_event_queue(FloraEventQueue *queue);

int enqueue_event(FloraEventQueue *queue, FloraEvent *event);

int dequeue_event(FloraEventQueue *queue, FloraEvent **event);

int is_event_queue_empty(FloraEventQueue *queue);

int destroy_event(FloraEvent *event);

#endif // FLORA_EVENTS_H
