#include <stdio.h>
#include <stdlib.h>

#include "flora_screens.h"
#include "flora_constants.h"
#include "flora_fonts.h"

void change_screen_to_demo(FloraWidget *widget, FloraApplicationState *state) {
    if (!set_current_screen(state, "demo")) {
        printf("Log: Error: Could not change screen to demo.\n");
    };
    printf("Tried to change current screen\n");
}

FloraScreen *create_screen(const char *name, const on_create_screen on_create_screen,
                           const on_destroy_screen on_destroy_screen) {
    FloraScreen *screen = calloc(1, sizeof(FloraScreen));
    if (!screen) {
        fprintf(stderr, "Error: Failed to allocate screen\n");
        return NULL;
    }
    strcpy(screen->name, name);
    screen->on_create_screen = on_create_screen ? on_create_screen : NULL;
    screen->on_destroy_screen = on_destroy_screen ? on_destroy_screen : NULL;
    screen->widget_capacity = INITIAL_WIDGET_CAPACITY;
    screen->widget_count = 0;
    screen->widgets = calloc(screen->widget_capacity, sizeof(FloraWidget *));
    if (!screen->widgets) {
        fprintf(stderr, "Error: Failed to allocate screen widgets\n");
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

void demo_create_screen(FloraApplicationState *state, FloraScreen *screen) {
    TTF_Font *font = add_font(state, OPEN_SANS_FONT_PATH, 18);
    TTF_Font *title_font = add_font(state, OPEN_SANS_FONT_PATH, 32);

    // Main container - full window with dark background
    FloraWidget *mainContainer = create_box_widget(
        state,
        NULL,
        (FloraWidgetStyle){
            .inner_colour = FLORA_SLATE_900,
            .border_colour = FLORA_SLATE_700,
            .padding = (FloraPadding){20.0f, 20.0f, 20.0f, 20.0f},
            .gap = (FloraGap){.x = 0.0f, .y = 20.0f},
            .layout_direction = TOP_TO_BOTTOM,
            .sizing = (FloraSizing){
                .width = FLORA_WIDTH_FIXED(760),
                .height = FLORA_HEIGHT_FIT(0)
            },
            .position = (FloraPosition){.x = 20, .y = 20}
        },
        (FloraWidgetCallbacks){
            .update = NULL,
            .render = base_widget_render,
            .on_mouse_down = NULL,
            .on_destroy = NULL
        },
        true
    );

    // Header section
    FloraWidget *header = create_box_widget(
        state,
        mainContainer,
        (FloraWidgetStyle){
            .inner_colour = FLORA_INDIGO_600,
            .border_colour = FLORA_INDIGO_400,
            .padding = (FloraPadding){20.0f, 20.0f, 15.0f, 15.0f},
            .gap = (FloraGap){.x = 0.0f, .y = 10.0f},
            .layout_direction = TOP_TO_BOTTOM,
            .sizing = (FloraSizing){
                .width = FLORA_WIDTH_GROW(1),
                .height = FLORA_HEIGHT_FIT(0)
            },
            .position = (FloraPosition){.x = 0, .y = 0}
        },
        (FloraWidgetCallbacks){
            .update = NULL,
            .render = base_widget_render,
            .on_mouse_down = base_box_widget_on_mouse_down,
            .on_destroy = NULL
        },
        true
    );

    FloraWidget *headerTitle = create_text_widget(
        state, header,
        (FloraWidgetStyle){
            .position = (FloraPosition){.x = 0, .y = 0},
            .sizing = {.width = FLORA_WIDTH_FIXED(0), .height = FLORA_HEIGHT_FIXED(0)},
            .text_colour = FLORA_WHITE,
            .font_size = 32
        },
        (FloraWidgetCallbacks){.render = base_text_widget_render},
        true, "Flora Engine Demo", 18, title_font
    );

    FloraWidget *headerSubtitle = create_text_widget(
        state, header,
        (FloraWidgetStyle){
            .position = (FloraPosition){.x = 0, .y = 0},
            .sizing = {.width = FLORA_WIDTH_FIXED(0), .height = FLORA_HEIGHT_FIXED(0)},
            .text_colour = FLORA_INDIGO_200,
            .font_size = 18
        },
        (FloraWidgetCallbacks){.render = base_text_widget_render},
        true, "Click widgets to randomize colors", 34, font
    );

    add_child_widget(header, headerTitle);
    add_child_widget(header, headerSubtitle);
    add_child_widget(mainContainer, header);

    // Button row
    FloraWidget *buttonRow = create_box_widget(
        state,
        mainContainer,
        (FloraWidgetStyle){
            .inner_colour = FLORA_SLATE_800,
            .border_colour = FLORA_SLATE_600,
            .padding = (FloraPadding){15.0f, 15.0f, 15.0f, 15.0f},
            .gap = (FloraGap){.x = 15.0f, .y = 0.0f},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = (FloraSizing){
                .width = FLORA_WIDTH_GROW(1),
                .height = FLORA_HEIGHT_FIT(0)
            },
            .position = (FloraPosition){.x = 0, .y = 0}
        },
        (FloraWidgetCallbacks){
            .update = NULL,
            .render = base_widget_render,
            .on_mouse_down = NULL,
            .on_destroy = NULL
        },
        true
    );

    // buttons
    const FloraColour button_colors[] = {FLORA_BLUE_500, FLORA_GREEN_500, FLORA_AMBER_500};

    for (int i = 0; i < 3; i++) {
        const char *button_labels[] = {"Primary", "Success", "Warning"};
        FloraWidget *button = create_box_widget(
            state,
            buttonRow,
            (FloraWidgetStyle){
                .inner_colour = button_colors[i],
                .border_colour = FLORA_WHITE,
                .padding = (FloraPadding){12.0f, 12.0f, 10.0f, 10.0f},
                .gap = (FloraGap){.x = 0.0f, .y = 0.0f},
                .layout_direction = LEFT_TO_RIGHT,
                .sizing = (FloraSizing){
                    .width = FLORA_WIDTH_GROW(1),
                    .height = FLORA_HEIGHT_FIT(0)
                },
                .position = (FloraPosition){.x = 0, .y = 0}
            },
            (FloraWidgetCallbacks){
                .update = NULL,
                .render = base_widget_render,
                .on_mouse_down = base_box_widget_on_mouse_down,
                .on_destroy = NULL
            },
            true
        );

        FloraWidget *buttonText = create_text_widget(
            state, button,
            (FloraWidgetStyle){
                .position = (FloraPosition){.x = 0, .y = 0},
                .sizing = {.width = FLORA_WIDTH_FIXED(0), .height = FLORA_HEIGHT_FIXED(0)},
                .text_colour = FLORA_WHITE,
                .font_size = 18
            },
            (FloraWidgetCallbacks){.render = base_text_widget_render},
            true, (char *) button_labels[i], (int) strlen(button_labels[i]), font
        );

        add_child_widget(button, buttonText);
        add_child_widget(buttonRow, button);
    }

    add_child_widget(mainContainer, buttonRow);

    // Content area with two columns
    FloraWidget *contentRow = create_box_widget(
        state,
        mainContainer,
        (FloraWidgetStyle){
            .inner_colour = FLORA_SLATE_800,
            .border_colour = FLORA_SLATE_600,
            .padding = (FloraPadding){15.0f, 15.0f, 15.0f, 15.0f},
            .gap = (FloraGap){.x = 20.0f, .y = 0.0f},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = (FloraSizing){
                .width = FLORA_WIDTH_GROW(1),
                .height = FLORA_HEIGHT_FIXED(300)
            },
            .position = (FloraPosition){.x = 0, .y = 0}
        },
        (FloraWidgetCallbacks){
            .update = NULL,
            .render = base_widget_render,
            .on_mouse_down = NULL,
            .on_destroy = NULL
        },
        true
    );

    // Left column - Card stack
    FloraWidget *leftColumn = create_box_widget(
        state,
        contentRow,
        (FloraWidgetStyle){
            .inner_colour = FLORA_SLATE_700,
            .border_colour = FLORA_SLATE_500,
            .padding = (FloraPadding){15.0f, 15.0f, 15.0f, 15.0f},
            .gap = (FloraGap){.x = 0.0f, .y = 15.0f},
            .layout_direction = TOP_TO_BOTTOM,
            .sizing = (FloraSizing){
                .width = FLORA_WIDTH_GROW(1),
                .height = FLORA_HEIGHT_GROW(1)
            },
            .position = (FloraPosition){.x = 0, .y = 0}
        },
        (FloraWidgetCallbacks){
            .update = NULL,
            .render = base_widget_render,
            .on_mouse_down = base_box_widget_on_mouse_down,
            .on_destroy = NULL
        },
        true
    );

    const FloraColour card_colors[] = {FLORA_PURPLE_600, FLORA_PINK_600, FLORA_ROSE_600};

    for (int i = 0; i < 3; i++) {
        const char *card_texts[] = {"Card 1", "Card 2", "Card 3"};
        FloraWidget *card = create_box_widget(
            state,
            leftColumn,
            (FloraWidgetStyle){
                .inner_colour = card_colors[i],
                .border_colour = FLORA_WHITE,
                .padding = (FloraPadding){10.0f, 10.0f, 8.0f, 8.0f},
                .gap = (FloraGap){.x = 0.0f, .y = 0.0f},
                .layout_direction = LEFT_TO_RIGHT,
                .sizing = (FloraSizing){
                    .width = FLORA_WIDTH_GROW(1),
                    .height = FLORA_HEIGHT_GROW(1)
                },
                .position = (FloraPosition){.x = 0, .y = 0}
            },
            (FloraWidgetCallbacks){
                .update = NULL,
                .render = base_widget_render,
                .on_mouse_down = base_box_widget_on_mouse_down,
                .on_destroy = NULL
            },
            true
        );

        FloraWidget *cardText = create_text_widget(
            state, card,
            (FloraWidgetStyle){
                .position = (FloraPosition){.x = 0, .y = 0},
                .sizing = {.width = FLORA_WIDTH_FIXED(0), .height = FLORA_HEIGHT_FIXED(0)},
                .text_colour = FLORA_WHITE,
                .font_size = 18
            },
            (FloraWidgetCallbacks){.render = base_text_widget_render},
            true, (char *) card_texts[i], -1, font
        );

        add_child_widget(card, cardText);
        add_child_widget(leftColumn, card);
    }

    add_child_widget(contentRow, leftColumn);

    // Right column - Nested layout
    FloraWidget *rightColumn = create_box_widget(
        state,
        contentRow,
        (FloraWidgetStyle){
            .inner_colour = FLORA_CYAN_700,
            .border_colour = FLORA_CYAN_400,
            .padding = (FloraPadding){15.0f, 15.0f, 15.0f, 15.0f},
            .gap = (FloraGap){.x = 0.0f, .y = 15.0f},
            .layout_direction = TOP_TO_BOTTOM,
            .sizing = (FloraSizing){
                .width = FLORA_WIDTH_GROW(1),
                .height = FLORA_HEIGHT_GROW(1)
            },
            .position = (FloraPosition){.x = 0, .y = 0}
        },
        (FloraWidgetCallbacks){
            .update = NULL,
            .render = base_widget_render,
            .on_mouse_down = base_box_widget_on_mouse_down,
            .on_destroy = NULL
        },
        true
    );

    FloraWidget *topBox = create_box_widget(
        state,
        rightColumn,
        (FloraWidgetStyle){
            .inner_colour = FLORA_EMERALD_600,
            .border_colour = FLORA_EMERALD_300,
            .padding = (FloraPadding){10.0f, 10.0f, 8.0f, 8.0f},
            .gap = (FloraGap){.x = 0.0f, .y = 0.0f},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = (FloraSizing){
                .width = FLORA_WIDTH_GROW(1),
                .height = FLORA_HEIGHT_GROW(2)
            },
            .position = (FloraPosition){.x = 0, .y = 0}
        },
        (FloraWidgetCallbacks){
            .update = NULL,
            .render = base_widget_render,
            .on_mouse_down = base_box_widget_on_mouse_down,
            .on_destroy = NULL
        },
        true
    );

    FloraWidget *topBoxText = create_text_widget(
        state, topBox,
        (FloraWidgetStyle){
            .position = (FloraPosition){.x = 0, .y = 0},
            .sizing = {.width = FLORA_WIDTH_FIXED(0), .height = FLORA_HEIGHT_FIXED(0)},
            .text_colour = FLORA_WHITE,
            .font_size = 18
        },
        (FloraWidgetCallbacks){.render = base_text_widget_render},
        true, "Nested Layout", 14, font
    );

    add_child_widget(topBox, topBoxText);
    add_child_widget(rightColumn, topBox);

    // Bottom horizontal split
    FloraWidget *bottomRow = create_box_widget(
        state,
        rightColumn,
        (FloraWidgetStyle){
            .inner_colour = FLORA_TEAL_600,
            .border_colour = FLORA_TEAL_300,
            .padding = (FloraPadding){10.0f, 10.0f, 10.0f, 10.0f},
            .gap = (FloraGap){.x = 10.0f, .y = 0.0f},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = (FloraSizing){
                .width = FLORA_WIDTH_GROW(1),
                .height = FLORA_HEIGHT_GROW(1)
            },
            .position = (FloraPosition){.x = 0, .y = 0}
        },
        (FloraWidgetCallbacks){
            .update = NULL,
            .render = base_widget_render,
            .on_mouse_down = base_box_widget_on_mouse_down,
            .on_destroy = NULL
        },
        true
    );

    for (int i = 0; i < 2; i++) {
        FloraWidget *smallBox = create_box_widget(
            state,
            bottomRow,
            (FloraWidgetStyle){
                .inner_colour = FLORA_SKY_500,
                .border_colour = FLORA_SKY_200,
                .padding = (FloraPadding){8.0f, 8.0f, 6.0f, 6.0f},
                .gap = (FloraGap){.x = 0.0f, .y = 0.0f},
                .layout_direction = LEFT_TO_RIGHT,
                .sizing = (FloraSizing){
                    .width = FLORA_WIDTH_GROW(1),
                    .height = FLORA_HEIGHT_GROW(1)
                },
                .position = (FloraPosition){.x = 0, .y = 0}
            },
            (FloraWidgetCallbacks){
                .update = NULL,
                .render = base_widget_render,
                .on_mouse_down = base_box_widget_on_mouse_down,
                .on_destroy = NULL
            },
            true
        );

        add_child_widget(bottomRow, smallBox);
    }

    add_child_widget(rightColumn, bottomRow);
    add_child_widget(contentRow, rightColumn);
    add_child_widget(mainContainer, contentRow);

    // Footer
    FloraWidget *footer = create_box_widget(
        state,
        mainContainer,
        (FloraWidgetStyle){
            .inner_colour = FLORA_SLATE_700,
            .border_colour = FLORA_SLATE_500,
            .padding = (FloraPadding){15.0f, 15.0f, 10.0f, 10.0f},
            .gap = (FloraGap){.x = 0.0f, .y = 0.0f},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = (FloraSizing){
                .width = FLORA_WIDTH_GROW(1),
                .height = FLORA_HEIGHT_FIT(0)
            },
            .position = (FloraPosition){.x = 0, .y = 0}
        },
        (FloraWidgetCallbacks){
            .update = NULL,
            .render = base_widget_render,
            .on_mouse_down = base_box_widget_on_mouse_down,
            .on_destroy = NULL
        },
        true
    );

    FloraWidget *footerText = create_text_widget(
        state, footer,
        (FloraWidgetStyle){
            .position = (FloraPosition){.x = 0, .y = 0},
            .sizing = {.width = FLORA_WIDTH_FIXED(0), .height = FLORA_HEIGHT_FIXED(0)},
            .text_colour = FLORA_SLATE_300,
            .font_size = 18
        },
        (FloraWidgetCallbacks){.render = base_text_widget_render},
        true, "Press ESC to quit", 18, font
    );

    add_child_widget(footer, footerText);
    add_child_widget(mainContainer, footer);
}

void render_screen(FloraScreen *screen, FloraApplicationState *state) {
    if (!screen || !state) {
        fprintf(stderr, "Error: Invalid screen or state.\n");
        return;
    }

    for (int i = 0; i < screen->widget_count; i++) {
        FloraWidget *widget = screen->widgets[i];
        if (widget->is_visible && widget->callbacks.render) {
            widget->callbacks.render(widget, state);
        }
    }
}

void base_destroy_screen(FloraApplicationState *state, FloraScreen *screen) {
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
    screen->on_destroy_screen = NULL;
    screen->on_create_screen = NULL;
}

void base_create_screen(FloraApplicationState *state, FloraScreen *screen) {
    FloraWidget *image_widget = create_image_widget(
        state,
        NULL,
        (FloraWidgetStyle){
            .inner_colour = FLORA_WHITE,
            .border_colour = FLORA_BLACK,
            .padding = {PADDING_X(12), PADDING_Y(12)},
            .gap = (FloraGap){0, 0},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = (FloraSizing){
                .width = FLORA_WIDTH_FIXED(500),
                .height = FLORA_HEIGHT_FIT(500)
            },
            .position = (FloraPosition){.x = 0, .y = 0}
        },
        (FloraWidgetCallbacks){
            .update = NULL,
            .render = base_widget_render,
            .on_mouse_down = NULL,
            .on_destroy = NULL
        },
        true,
        "D:\\flora-engine\\assets\\cat.jpg",
        -1
    );

    FloraWidget *baseWidget = create_box_widget(
        state,
        NULL,
        (FloraWidgetStyle){
            .inner_colour = FLORA_SLATE_500,
            .border_colour = FLORA_WHITE,
            .padding = (FloraPadding){
                .left = 20.0f,
                .right = 20.0f,
                .top = 10.0f,
                .bottom = 10.0f
            },
            .gap = (FloraGap){.x = 25.0f, .y = 0.0f},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = (FloraSizing){
                .width = FLORA_WIDTH_FIXED(460),
                .height = FLORA_HEIGHT_FIT(50)
            },
            .position = (FloraPosition){.x = 100, .y = 100}
        },
        (FloraWidgetCallbacks){
            .update = NULL,
            .render = base_widget_render,
            .on_mouse_down = base_box_widget_on_mouse_down,
            .on_destroy = NULL
        },
        true
    );

    FloraWidget *child1 = create_box_widget(
        state,
        baseWidget,
        (FloraWidgetStyle){
            .inner_colour = FLORA_INDIGO_500,
            .border_colour = FLORA_WHITE,
            .padding = (FloraPadding){0, 0, 0, 0},
            .gap = (FloraGap){0, 0},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = (FloraSizing){
                .width = FLORA_WIDTH_GROW(125),
                .height = FLORA_HEIGHT_GROW(50)
            },
            .position = (FloraPosition){.x = 50, .y = 50}
        },
        (FloraWidgetCallbacks){
            .update = NULL,
            .render = base_widget_render,
            .on_mouse_down = base_box_widget_on_mouse_down,
            .on_destroy = NULL
        },
        true
    );

    FloraWidget *child2 = create_box_widget(
        state,
        baseWidget,
        (FloraWidgetStyle){
            .inner_colour = FLORA_EMERALD_500,
            .border_colour = FLORA_WHITE,
            .padding = (FloraPadding){
                .left = 10.0f,
                .right = 10.0f,
                .top = 5.0f,
                .bottom = 5.0f
            },
            .gap = (FloraGap){.x = 0.0f, .y = 20.0f},
            .layout_direction = TOP_TO_BOTTOM,
            .sizing = (FloraSizing){
                .width = FLORA_WIDTH_GROW(100),
                .height = FLORA_HEIGHT_FIT(100)
            },
            .position = (FloraPosition){.x = 50, .y = 50}
        },
        (FloraWidgetCallbacks){
            .update = NULL,
            .render = base_widget_render,
            .on_mouse_down = base_box_widget_on_mouse_down,
            .on_destroy = NULL
        },
        true
    );

    FloraWidget *child3 = create_box_widget(
        state,
        child2,
        (FloraWidgetStyle){
            .inner_colour = FLORA_CYAN_500,
            .border_colour = FLORA_WHITE,
            .padding = (FloraPadding){0, 0, 0, 0},
            .gap = (FloraGap){0, 0},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = (FloraSizing){
                .width = FLORA_WIDTH_GROW(50),
                .height = FLORA_HEIGHT_GROW(50)
            },
            .position = (FloraPosition){.x = 50, .y = 50}
        },
        (FloraWidgetCallbacks){
            .update = NULL,
            .render = base_widget_render,
            .on_mouse_down = base_box_widget_on_mouse_down,
            .on_destroy = NULL
        },
        true
    );

    FloraWidget *child4 = create_box_widget(
        state,
        child2,
        (FloraWidgetStyle){
            .inner_colour = FLORA_AMBER_500,
            .border_colour = FLORA_WHITE,
            .padding = (FloraPadding){0, 0, 0, 0},
            .gap = (FloraGap){0, 0},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = (FloraSizing){
                .width = FLORA_WIDTH_GROW(90),
                .height = FLORA_HEIGHT_GROW(60)
            },
            .position = (FloraPosition){.x = 50, .y = 50}
        },
        (FloraWidgetCallbacks){
            .update = NULL,
            .render = base_widget_render,
            .on_mouse_down = base_box_widget_on_mouse_down,
            .on_destroy = NULL
        },
        true
    );

    add_child_widget(baseWidget, child1);
    add_child_widget(baseWidget, child2);
    add_child_widget(child2, child3);
    add_child_widget(child2, child4);

    TTF_Font *font = add_font(state, OPEN_SANS_FONT_PATH, 24);

    FloraWidget *child5 = create_text_widget(state, child1,
                                             (FloraWidgetStyle){
                                                 .position = (FloraPosition){.x = 50, .y = 50},
                                                 .sizing = {
                                                     .width = FLORA_WIDTH_FIXED(40), .height = FLORA_HEIGHT_FIXED(24)
                                                 },
                                                 .text_colour = FLORA_WHITE, .font_size = 24
                                             }, (FloraWidgetCallbacks){.render = base_text_widget_render}, true,
                                             "Hello World.", 13, font);
    add_child_widget(child1, child5);

    FloraWidget *next_screen_button = create_box_widget(state, NULL, (FloraWidgetStyle){
                                                            .position = (FloraPosition){.x = 25, .y = 25},
                                                            .sizing = {
                                                                .width = FLORA_WIDTH_GROW(176),
                                                                .height = FLORA_HEIGHT_GROW(48)
                                                            },
                                                            .text_colour = FLORA_WHITE, .font_size = 24,
                                                            .border_colour = FLORA_WHITE,
                                                            .inner_colour = FLORA_GRAY_900,
                                                            .padding = {PADDING(5)}
                                                        },
                                                        (FloraWidgetCallbacks){
                                                            .on_destroy = NULL,
                                                            .on_mouse_down = change_screen_to_demo,
                                                            .update = NULL,
                                                            .render = base_widget_render,
                                                        }, true);
    FloraWidget *next_screen_button_text = create_text_widget(state, next_screen_button,
                                                              (FloraWidgetStyle){.text_colour = FLORA_WHITE},
                                                              (FloraWidgetCallbacks){}, true, "Next Screen",
                                                              12, font);
    add_child_widget(next_screen_button, next_screen_button_text);
}
