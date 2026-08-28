#ifndef FLORA_WIDGETS_H
#define FLORA_WIDGETS_H
#include <SDL3_ttf/SDL_ttf.h>

#include "flora.h"

struct FloraWidget {
	int id;
	FloraWidgetType type;
	FloraWidget *parent;
	FloraWidget **children;
	FloraWidgetStyle style;
	FloraWidgetCallbacks callbacks;
	int child_count;
	int child_capacity;
	int is_visible;
	int is_dirty; // TODO: split to layout dirty paint dirty

	union {
		struct {
		} box;
		struct {
		} image;
		struct {
		} scroll_box;

		struct {
			/* Font used to render this text */
			TTF_Font *font;
			/* The length of the text content */
			int content_length;
			/* Allocated text content */
			char *content;
			TTF_Text *ttf_text;
			SDL_Surface *surface;
			SDL_Texture *texture;
		} text;
	} as;
};

int cleanup_widget(FloraWidget *widget);

int layout_widget(FloraWidget *widget);

int render_widget(FloraWindow *window, FloraWidget *widget);

int widget_contains_point(FloraWidget *widget, int x, int y);

#endif // FLORA_WIDGETS_H
