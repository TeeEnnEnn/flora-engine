#include <stdio.h>
#include <stdlib.h>

#include "flora_widgets.h"
#include "flora_apps.h"
#include "flora_constants.h"

static void calc_widget_dimensions(FloraWidget *widget) {
    if (!widget || !widget->is_visible || widget->child_count == 0) {
        return;
    }

    float totalWidgetWidth = 0.0f;
    float totalWidgetHeight = 0.0f;
    int visible_child_count = 0;

    if (widget->style.layout_direction == LEFT_TO_RIGHT) {
        for (int i = 0; i < widget->child_count; i++) {
            FloraWidget *child = widget->children[i];
            calc_widget_dimensions(child);
            if (child->is_visible) {
                totalWidgetHeight = child->style.sizing.height.value > totalWidgetHeight
                                        ? child->style.sizing.height.value
                                        : totalWidgetHeight;
                totalWidgetWidth += child->style.sizing.width.value;
                visible_child_count++;
            }
        }
        if (visible_child_count > 0) {
            totalWidgetWidth += (visible_child_count - 1) * widget->style.gap.x;
        }
    } else if (widget->style.layout_direction == TOP_TO_BOTTOM) {
        for (int i = 0; i < widget->child_count; i++) {
            FloraWidget *child = widget->children[i];
            calc_widget_dimensions(child);
            if (child->is_visible) {
                totalWidgetHeight += child->style.sizing.height.value;
                totalWidgetWidth = child->style.sizing.width.value > totalWidgetWidth
                                       ? child->style.sizing.width.value
                                       : totalWidgetWidth;
                visible_child_count++;
            }
        }
        if (visible_child_count > 0) {
            totalWidgetHeight += (visible_child_count - 1) * widget->style.gap.y;
        }
    }

    if (widget->style.sizing.width.type == FIT) {
        widget->style.sizing.width.value = totalWidgetWidth + widget->style.padding.left + widget->style.padding.right;
    }
    if (widget->style.sizing.height.type == FIT) {
        widget->style.sizing.height.value = totalWidgetHeight + widget->style.padding.top + widget->style.padding.
                                            bottom;
    }
}

static void calc_child_positions(FloraWidget *widget) {
    if (!widget || !widget->is_visible || widget->child_count == 0) {
        return;
    }

    float offsetX = widget->style.padding.left;
    float offsetY = widget->style.padding.top;
    if (widget->style.layout_direction == LEFT_TO_RIGHT) {
        for (int i = 0; i < widget->child_count; i++) {
            FloraWidget *child = widget->children[i];
            if (!child->is_visible) {
                continue;
            }
            child->style.position.x = widget->style.position.x + offsetX;
            child->style.position.y = widget->style.position.y + offsetY;
            offsetX += child->style.sizing.width.value + widget->style.gap.x;

            calc_child_positions(child);
        }
    } else if (widget->style.layout_direction == TOP_TO_BOTTOM) {
        for (int i = 0; i < widget->child_count; i++) {
            FloraWidget *child = widget->children[i];
            if (!child->is_visible) {
                continue;
            }
            child->style.position.x = widget->style.position.x + offsetX;
            child->style.position.y = widget->style.position.y + offsetY;
            offsetY += child->style.sizing.height.value + widget->style.gap.y;
            calc_child_positions(child);
        }
    }
}

static void calc_grow_sizing_left_to_right(const FloraWidget *widget) {
    float remainingWidth = widget->style.sizing.width.value;
    remainingWidth -= widget->style.padding.left + widget->style.padding.right;
    int visible_child_count = 0;

    for (int i = 0; i < widget->child_count; i++) {
        const FloraWidget *child = widget->children[i];
        visible_child_count += child->is_visible ? 1 : 0;
    }

    if (visible_child_count > 0) {
        remainingWidth -= widget->style.gap.x * (visible_child_count - 1);
    }

    if (widget->style.layout_direction == LEFT_TO_RIGHT) {
        for (int i = 0; i < widget->child_count; i++) {
            const FloraWidget *child = widget->children[i];
            if (child->is_visible && child->style.sizing.width.type != GROW) {
                remainingWidth -= child->style.sizing.width.value;
            }
        }
    }
    int growCount = 0;
    for (int i = 0; i < widget->child_count; i++) {
        const FloraWidget *child = widget->children[i];
        if (child->style.sizing.width.type == GROW) {
            growCount++;
        }
    }

    if (growCount > 0) {
        const float growWidth = widget->style.layout_direction == LEFT_TO_RIGHT
                                    ? remainingWidth / growCount
                                    : remainingWidth;
        for (int i = 0; i < widget->child_count; i++) {
            FloraWidget *child = widget->children[i];
            if (child->is_visible && child->style.sizing.width.type == GROW) {
                child->style.sizing.width.value = growWidth;
            }
        }
    }
}

static void calc_grow_sizing_top_to_bottom(const FloraWidget *widget) {
    float remainingHeight = widget->style.sizing.height.value;
    remainingHeight -= widget->style.padding.top + widget->style.padding.bottom;
    int visible_child_count = 0;

    for (int i = 0; i < widget->child_count; i++) {
        FloraWidget *child = widget->children[i];
        visible_child_count += child->is_visible ? 1 : 0;
    }
    if (visible_child_count > 0) {
        remainingHeight -= widget->style.gap.y * (visible_child_count - 1);
    }

    if (widget->style.layout_direction == TOP_TO_BOTTOM) {
        for (int i = 0; i < widget->child_count; i++) {
            const FloraWidget *child = widget->children[i];
            if (child->is_visible && child->style.sizing.height.type != GROW) {
                remainingHeight -= child->style.sizing.height.value;
            }
        }
    }

    int growCount = 0;
    for (int i = 0; i < widget->child_count; i++) {
        const FloraWidget *child = widget->children[i];
        if (child->style.sizing.height.type == GROW) {
            growCount++;
        }
    }

    if (growCount > 0) {
        const float growHeight = widget->style.layout_direction == TOP_TO_BOTTOM
                                     ? remainingHeight / growCount
                                     : remainingHeight;
        for (int i = 0; i < widget->child_count; i++) {
            FloraWidget *child = widget->children[i];
            if (child->is_visible && child->style.sizing.height.type == GROW) {
                child->style.sizing.height.value = growHeight;
            }
        }
    }
}

static void calc_grow_sizing(const FloraWidget *widget) {
    calc_grow_sizing_left_to_right(widget);
    calc_grow_sizing_top_to_bottom(widget);
}

static void render_widget(FloraWidget *widget, FloraApplicationState *state) {
    if (!widget || !widget->is_visible) {
        return;
    }

    switch (widget->type) {
        case FLORA_TEXT: {
            SDL_RenderTexture(state->renderer, widget->as.text.texture, NULL, &(SDL_FRect){
                                  widget->style.position.x, widget->style.position.y, widget->style.sizing.width.value,
                                  widget->style.sizing.height.value
                              });
            break;
        }
        case FLORA_BOX: {
            const FloraWidgetStyle style = widget->style;
            const FloraPosition position = widget->style.position;

            SDL_SetRenderDrawColor(state->renderer, style.inner_colour.r, style.inner_colour.g, style.inner_colour.b,
                                   style.inner_colour.a);
            const SDL_FRect rect = {
                position.x, position.y, widget->style.sizing.width.value, widget->style.sizing.height.value
            };
            SDL_RenderFillRect(state->renderer, &rect);
            SDL_SetRenderDrawColor(state->renderer, style.border_colour.r, style.border_colour.g, style.border_colour.b,
                                   style.border_colour.a);
            SDL_RenderRect(state->renderer, &rect);

            for (int i = 0; i < widget->child_count; i++) {
                FloraWidget *child = widget->children[i];
                if (child && child->is_visible) {
                    render_widget(child, state);
                }
            }
            break;
        }
    }
}

void base_text_widget_render(FloraWidget *widget, FloraApplicationState *state) {
    if (!widget || !widget->is_visible) {
        return;
    }
    render_widget(widget, state);
}

FloraWidget *create_box_widget(FloraApplicationState *state, FloraWidget *parent,
                               const FloraWidgetStyle style,
                               const FloraWidgetCallbacks callbacks,
                               const bool is_visible) {
    if (!state) {
        fprintf(stderr, "Error: Widget create got invalid application state.");
        return NULL;
    }
    FloraScreen *screen = state->current_screen;
    if (!screen) {
        fprintf(stderr, "Error: No valid screen found to add widgets to.");
        return NULL;
    }

    if (screen->widget_count == screen->widget_capacity) {
        FloraWidget **new_widgets = realloc(screen->widgets,
                                            screen->widget_capacity * GROWTH_FACTOR * sizeof(FloraWidget *));
        if (!new_widgets) {
            fprintf(stderr, "Error: Could not reallocate widgets.");
            return NULL;
        }
        screen->widgets = new_widgets;
        screen->widget_capacity = screen->widget_capacity * GROWTH_FACTOR;
    }

    FloraWidget *new_widget = calloc(1, sizeof(FloraWidget));
    if (!new_widget) {
        fprintf(stderr, "Error: Could not allocate widget.\n");
        return NULL;
    }
    new_widget->id = screen->widget_count++;
    new_widget->is_visible = is_visible;
    new_widget->style = style;
    new_widget->callbacks = callbacks;
    new_widget->child_capacity = INITIAL_CHILD_WIDGET_CAPACITY;
    new_widget->parent = parent;
    new_widget->children = NULL;
    new_widget->child_count = 0;
    new_widget->type = FLORA_BOX;

    screen->widgets[screen->widget_count - 1] = new_widget;

    return new_widget;
}

// Length must be -1 if using a non-owned content.
// if length is non-negative - will take ownership of string
// Length must include null terminator
FloraWidget *create_text_widget(FloraApplicationState *state, FloraWidget *parent, FloraWidgetStyle style,
                                const FloraWidgetCallbacks callbacks, const bool is_visible, char *text,
                                const int length, TTF_Font *font) {
    if (!state) {
        fprintf(stderr, "Error: Widget create got invalid application state.\n");
        return NULL;
    }
    FloraScreen *screen = state->current_screen;
    if (!screen) {
        fprintf(stderr, "Error: No valid screen found to add widgets to.\n");
        return NULL;
    }

    if (screen->widget_count == screen->widget_capacity) {
        FloraWidget **new_widgets = realloc(screen->widgets,
                                            screen->widget_capacity * GROWTH_FACTOR * sizeof(FloraWidget));
        if (!new_widgets) {
            fprintf(stderr, "Error: Could not reallocate widgets.\n");
            return NULL;
        }
        screen->widgets = new_widgets;
        screen->widget_capacity = screen->widget_capacity * GROWTH_FACTOR;
    }
    if (!font) {
        fprintf(stderr, "Error: Text font widget created with a null font.\n");
        return NULL;
    }

    const int length_to_render = length < 0 ? 0 : length;

    SDL_Surface *surface = TTF_RenderText_Blended(font, text, length_to_render,
                                                  (SDL_Color){
                                                      style.text_colour.r, style.text_colour.g,
                                                      style.text_colour.b, style.text_colour.a
                                                  }
    );

    if (!surface) {
        fprintf(stderr, "Error: TTF_RenderText_Blended_Wrapped failed.\n");
        return NULL;
    }
    style.sizing.width.value = (float) surface->w;
    style.sizing.height.value = (float) surface->h;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(state->renderer, surface);

    if (!texture) {
        fprintf(stderr, "Error: SDL_CreateTextureFromSurface failed.\n");
        return NULL;
    }

    // Safely take ownership of string
    char *string = NULL;
    if (length > 0) {
        string = malloc(sizeof(char) * length);
        if (!string) {
            fprintf(stderr, "Error: Could not allocate memory for string.\n");
            SDL_DestroyTexture(texture);
            SDL_DestroySurface(surface);
            return NULL;
        }
        memcpy(string, text, length); // length should include '\0' terminator
    } else {
        string = (char *) text;
    }

    FloraWidget *new_widget = calloc(1, sizeof(FloraWidget));
    if (!new_widget) {
        if (length > 0) {
            free(string);
        }
        fprintf(stderr, "Error: Could not allocate widget.\n");
        return NULL;
    }
    new_widget->id = screen->widget_count++;
    new_widget->is_visible = is_visible;
    new_widget->style = style;
    new_widget->callbacks = callbacks;
    new_widget->child_capacity = INITIAL_CHILD_WIDGET_CAPACITY;
    new_widget->parent = parent;
    new_widget->children = NULL;
    new_widget->child_count = 0;
    new_widget->type = FLORA_TEXT;

    new_widget->as.text.content = string;
    new_widget->as.text.length = length;
    new_widget->as.text.font = font;
    new_widget->as.text.surface = surface;
    new_widget->as.text.texture = texture;

    screen->widgets[screen->widget_count - 1] = new_widget;
    return new_widget;
}

bool add_child_widget(FloraWidget *widget, FloraWidget *child) {
    if (!widget) {
        fprintf(stderr, "Error: Widget add got invalid parent widget.\n");
        return false;
    }
    if (!child) {
        fprintf(stderr, "Error: Child widget add got invalid child widget.\n");
        return false;
    }
    if (!widget->children) {
        widget->child_capacity = INITIAL_CHILD_WIDGET_CAPACITY;
        widget->child_count = 0;
        widget->children = calloc(widget->child_capacity, sizeof(FloraWidget *));
        if (!widget->children) {
            fprintf(stderr, "Error: Could not allocate widgets.\n");
            return false;
        }
    }
    if (widget->child_count == widget->child_capacity && widget->children) {
        FloraWidget **new_widgets = realloc(widget->children,
                                            widget->child_capacity * GROWTH_FACTOR * sizeof(FloraWidget *));
        if (!new_widgets) {
            fprintf(stderr, "Error: Could not reallocate widgets.\n");
            return false;
        }
        widget->children = new_widgets;
        widget->child_capacity = widget->child_capacity * GROWTH_FACTOR;
    }
    widget->children[widget->child_count++] = child;
    child->parent = widget;
    return true;
}

void base_box_widget_render(FloraWidget *widget, FloraApplicationState *state) {
    if (!widget || !widget->is_visible) {
        return;
    }

    calc_widget_dimensions(widget);
    calc_grow_sizing(widget);
    calc_child_positions(widget);
    render_widget(widget, state);
}

void base_box_widget_update(FloraWidget *widget, FloraApplicationState *state) {
    (void) widget;
}

void base_box_widget_on_mouse_down(FloraWidget *widget, FloraApplicationState *state) {
    (void) state;
    widget->style.inner_colour = (FloraColour){
        .r = rand() % 255, .g = rand() % 255, .b = rand() % 255, .a = rand() % 255
    };
    widget->style.border_colour = (FloraColour){
        .r = rand() % 255, .g = rand() % 255, .b = rand() % 255, .a = rand() % 255
    };
}

bool widget_contains_point(FloraWidget *widget, const int x, const int y) {
    if (!widget || !widget->is_visible) {
        return false;
    }

    return x >= widget->style.position.x && x < widget->style.position.x + widget->style.sizing.width.value &&
           y >= widget->style.position.y && y < widget->style.position.y + widget->style.sizing.height.value;
}

bool destroy_flora_widget(FloraWidget *widget) {
    if (!widget) {
        return false;
    }
    if (widget->children) {
        free(widget->children);
    }
    return true;
}
