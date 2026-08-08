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
    bool is_visible;

    union {
        struct {} box;

        struct {}image;

        struct {
            TTF_Font *font;
            int length;
            char *content;
            SDL_Surface *surface;
            SDL_Texture *texture;
        } text;
    } as;
};

int cleanup_widget(FloraWidget *widget);

int widget_contains_point(FloraWidget *widget, int x, int y);

#endif //FLORA_WIDGETS_H
