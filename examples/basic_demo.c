#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "flora.h"

#define OPEN_SANS_FONT_PATH "assets/fonts/Open_Sans/OpenSans-VariableFont_wdth,wght.ttf"

void change_screen_to_demo(FloraWidget *widget, FloraApplicationState *state) {
    (void) widget;
    FloraScreen *demo = get_flora_screen(get_current_flora_window(state), "demo");
    if (!demo) {
        printf("Log: Error: Could not find demo screen.\n");
        return;
    }
    if (!set_current_flora_screen(state, demo)) {
        printf("Log: Error: Could not change screen to demo.\n");
    }
    printf("Tried to change current screen\n");
}

void demo_create_screen(FloraApplicationState *state, FloraScreen *screen) {
    (void) screen;
    FloraFont *font = add_flora_font(state, OPEN_SANS_FONT_PATH, 18);
    FloraFont *title_font = add_flora_font(state, OPEN_SANS_FONT_PATH, 32);

    // Main container - full window with dark background
    FloraWidget *mainContainer = create_flora_box_widget(state, NULL, FLORA_TRUE,
        (FloraWidgetStyle){
            .inner_colour = FLORA_SLATE_900,
            .border_colour = FLORA_SLATE_700,
            .padding = PADDING(20),
            .gap = {.x = 0.0f, .y = 20.0f},
            .layout_direction = TOP_TO_BOTTOM,
            .sizing = {.width = FLORA_WIDTH_FIXED(760), .height = FLORA_HEIGHT_FIT(0)},
            .position = {.x = 20, .y = 20},
        });

    // Header section
    FloraWidget *header = create_flora_box_widget(state, mainContainer, FLORA_TRUE,
        (FloraWidgetStyle){
            .inner_colour = FLORA_INDIGO_600,
            .border_colour = FLORA_INDIGO_400,
            .padding = {20.0f, 20.0f, 15.0f, 15.0f},
            .gap = {.x = 0.0f, .y = 10.0f},
            .layout_direction = TOP_TO_BOTTOM,
            .sizing = {.width = FLORA_WIDTH_GROW(1), .height = FLORA_HEIGHT_FIT(0)},
            .position = {.x = 0, .y = 0},
        });
    set_flora_widget_on_mouse_down(header, base_box_widget_on_mouse_down);

    FloraWidget *headerTitle = create_flora_text_widget(state, header, FLORA_TRUE, "Flora Engine Demo", title_font,
        (FloraWidgetStyle){.text_colour = FLORA_WHITE, .font_size = 32});

    FloraWidget *headerSubtitle = create_flora_text_widget(state, header, FLORA_TRUE,
                                                           "Click widgets to randomize colors", font,
        (FloraWidgetStyle){.text_colour = FLORA_INDIGO_200, .font_size = 18});

    add_flora_child_widget(header, headerTitle);
    add_flora_child_widget(header, headerSubtitle);
    add_flora_child_widget(mainContainer, header);

    // Button row
    FloraWidget *buttonRow = create_flora_box_widget(state, mainContainer, FLORA_TRUE,
        (FloraWidgetStyle){
            .inner_colour = FLORA_SLATE_800,
            .border_colour = FLORA_SLATE_600,
            .padding = PADDING(15),
            .gap = {.x = 15.0f, .y = 0.0f},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = {.width = FLORA_WIDTH_GROW(1), .height = FLORA_HEIGHT_FIT(0)},
            .position = {.x = 0, .y = 0},
        });

    // buttons
    const FloraColour button_colors[] = {FLORA_BLUE_500, FLORA_GREEN_500, FLORA_AMBER_500};
    const char *button_labels[] = {"Primary", "Success", "Warning"};

    for (int i = 0; i < 3; i++) {
        FloraWidget *button = create_flora_box_widget(state, buttonRow, FLORA_TRUE,
            (FloraWidgetStyle){
                .inner_colour = button_colors[i],
                .border_colour = FLORA_WHITE,
                .padding = {12.0f, 12.0f, 10.0f, 10.0f},
                .gap = {.x = 0.0f, .y = 0.0f},
                .layout_direction = LEFT_TO_RIGHT,
                .sizing = {.width = FLORA_WIDTH_GROW(1), .height = FLORA_HEIGHT_FIT(0)},
                .position = {.x = 0, .y = 0},
            });
        set_flora_widget_on_mouse_down(button, base_box_widget_on_mouse_down);

        FloraWidget *buttonText = create_flora_text_widget(state, button, FLORA_TRUE, button_labels[i], font,
            (FloraWidgetStyle){.text_colour = FLORA_WHITE, .font_size = 18});

        add_flora_child_widget(button, buttonText);
        add_flora_child_widget(buttonRow, button);
    }

    add_flora_child_widget(mainContainer, buttonRow);

    // Content area with two columns
    FloraWidget *contentRow = create_flora_box_widget(state, mainContainer, FLORA_TRUE,
        (FloraWidgetStyle){
            .inner_colour = FLORA_SLATE_800,
            .border_colour = FLORA_SLATE_600,
            .padding = PADDING(15),
            .gap = {.x = 20.0f, .y = 0.0f},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = {.width = FLORA_WIDTH_GROW(1), .height = FLORA_HEIGHT_FIXED(300)},
            .position = {.x = 0, .y = 0},
        });

    // Left column - Card stack
    FloraWidget *leftColumn = create_flora_box_widget(state, contentRow, FLORA_TRUE,
        (FloraWidgetStyle){
            .inner_colour = FLORA_SLATE_700,
            .border_colour = FLORA_SLATE_500,
            .padding = PADDING(15),
            .gap = {.x = 0.0f, .y = 15.0f},
            .layout_direction = TOP_TO_BOTTOM,
            .sizing = {.width = FLORA_WIDTH_GROW(1), .height = FLORA_HEIGHT_GROW(1)},
            .position = {.x = 0, .y = 0},
        });
    set_flora_widget_on_mouse_down(leftColumn, base_box_widget_on_mouse_down);

    const FloraColour card_colors[] = {FLORA_PURPLE_600, FLORA_PINK_600, FLORA_ROSE_600};
    const char *card_texts[] = {"Card 1", "Card 2", "Card 3"};

    for (int i = 0; i < 3; i++) {
        FloraWidget *card = create_flora_box_widget(state, leftColumn, FLORA_TRUE,
            (FloraWidgetStyle){
                .inner_colour = card_colors[i],
                .border_colour = FLORA_WHITE,
                .padding = {10.0f, 10.0f, 8.0f, 8.0f},
                .gap = {.x = 0.0f, .y = 0.0f},
                .layout_direction = LEFT_TO_RIGHT,
                .sizing = {.width = FLORA_WIDTH_GROW(1), .height = FLORA_HEIGHT_GROW(1)},
                .position = {.x = 0, .y = 0},
            });
        set_flora_widget_on_mouse_down(card, base_box_widget_on_mouse_down);

        FloraWidget *cardText = create_flora_text_widget(state, card, FLORA_TRUE, card_texts[i], font,
            (FloraWidgetStyle){.text_colour = FLORA_WHITE, .font_size = 18});

        add_flora_child_widget(card, cardText);
        add_flora_child_widget(leftColumn, card);
    }

    add_flora_child_widget(contentRow, leftColumn);

    // Right column - Nested layout
    FloraWidget *rightColumn = create_flora_box_widget(state, contentRow, FLORA_TRUE,
        (FloraWidgetStyle){
            .inner_colour = FLORA_CYAN_700,
            .border_colour = FLORA_CYAN_400,
            .padding = PADDING(15),
            .gap = {.x = 0.0f, .y = 15.0f},
            .layout_direction = TOP_TO_BOTTOM,
            .sizing = {.width = FLORA_WIDTH_GROW(1), .height = FLORA_HEIGHT_GROW(1)},
            .position = {.x = 0, .y = 0},
        });
    set_flora_widget_on_mouse_down(rightColumn, base_box_widget_on_mouse_down);

    FloraWidget *topBox = create_flora_box_widget(state, rightColumn, FLORA_TRUE,
        (FloraWidgetStyle){
            .inner_colour = FLORA_EMERALD_600,
            .border_colour = FLORA_EMERALD_300,
            .padding = {10.0f, 10.0f, 8.0f, 8.0f},
            .gap = {.x = 0.0f, .y = 0.0f},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = {.width = FLORA_WIDTH_GROW(1), .height = FLORA_HEIGHT_GROW(2)},
            .position = {.x = 0, .y = 0},
        });
    set_flora_widget_on_mouse_down(topBox, base_box_widget_on_mouse_down);

    FloraWidget *topBoxText = create_flora_text_widget(state, topBox, FLORA_TRUE, "Nested Layout", font,
        (FloraWidgetStyle){.text_colour = FLORA_WHITE, .font_size = 18});

    add_flora_child_widget(topBox, topBoxText);
    add_flora_child_widget(rightColumn, topBox);

    // Bottom horizontal split
    FloraWidget *bottomRow = create_flora_box_widget(state, rightColumn, FLORA_TRUE,
        (FloraWidgetStyle){
            .inner_colour = FLORA_TEAL_600,
            .border_colour = FLORA_TEAL_300,
            .padding = PADDING(10),
            .gap = {.x = 10.0f, .y = 0.0f},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = {.width = FLORA_WIDTH_GROW(1), .height = FLORA_HEIGHT_GROW(1)},
            .position = {.x = 0, .y = 0},
        });

    for (int i = 0; i < 2; i++) {
        FloraWidget *smallBox = create_flora_box_widget(state, bottomRow, FLORA_TRUE,
            (FloraWidgetStyle){
                .inner_colour = FLORA_SKY_500,
                .border_colour = FLORA_SKY_200,
                .padding = {8.0f, 8.0f, 6.0f, 6.0f},
                .gap = {.x = 0.0f, .y = 0.0f},
                .layout_direction = LEFT_TO_RIGHT,
                .sizing = {.width = FLORA_WIDTH_GROW(1), .height = FLORA_HEIGHT_GROW(1)},
                .position = {.x = 0, .y = 0},
            });
        set_flora_widget_on_mouse_down(smallBox, base_box_widget_on_mouse_down);

        add_flora_child_widget(bottomRow, smallBox);
    }

    add_flora_child_widget(rightColumn, bottomRow);
    add_flora_child_widget(contentRow, rightColumn);
    add_flora_child_widget(mainContainer, contentRow);

    // Footer
    FloraWidget *footer = create_flora_box_widget(state, mainContainer, FLORA_TRUE,
        (FloraWidgetStyle){
            .inner_colour = FLORA_SLATE_700,
            .border_colour = FLORA_SLATE_500,
            .padding = {15.0f, 15.0f, 10.0f, 10.0f},
            .gap = {.x = 0.0f, .y = 0.0f},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = {.width = FLORA_WIDTH_GROW(1), .height = FLORA_HEIGHT_FIT(0)},
            .position = {.x = 0, .y = 0},
        });
    set_flora_widget_on_mouse_down(footer, base_box_widget_on_mouse_down);

    FloraWidget *footerText = create_flora_text_widget(state, footer, FLORA_TRUE, "Press ESC to quit", font,
        (FloraWidgetStyle){.text_colour = FLORA_SLATE_300, .font_size = 18});

    add_flora_child_widget(footer, footerText);
    add_flora_child_widget(mainContainer, footer);
}

void base_create_screen(FloraApplicationState *state, FloraScreen *screen) {
    (void) screen;
    FloraWidget *baseWidget = create_flora_box_widget(state, NULL, FLORA_TRUE,
        (FloraWidgetStyle){
            .inner_colour = FLORA_SLATE_500,
            .border_colour = FLORA_WHITE,
            .padding = {20.0f, 20.0f, 10.0f, 10.0f},
            .gap = {.x = 25.0f, .y = 0.0f},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = {.width = FLORA_WIDTH_FIXED(460), .height = FLORA_HEIGHT_FIT(50)},
            .position = {.x = 100, .y = 100},
        });
    set_flora_widget_on_mouse_down(baseWidget, base_box_widget_on_mouse_down);

    FloraWidget *child1 = create_flora_box_widget(state, baseWidget, FLORA_TRUE,
        (FloraWidgetStyle){
            .inner_colour = FLORA_INDIGO_500,
            .border_colour = FLORA_WHITE,
            .padding = {0.0f, 0.0f, 0.0f, 0.0f},
            .gap = {.x = 0.0f, .y = 0.0f},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = {.width = FLORA_WIDTH_GROW(125), .height = FLORA_HEIGHT_GROW(50)},
            .position = {.x = 50, .y = 50},
        });
    set_flora_widget_on_mouse_down(child1, base_box_widget_on_mouse_down);

    FloraWidget *child2 = create_flora_box_widget(state, baseWidget, FLORA_TRUE,
        (FloraWidgetStyle){
            .inner_colour = FLORA_EMERALD_500,
            .border_colour = FLORA_WHITE,
            .padding = {10.0f, 10.0f, 5.0f, 5.0f},
            .gap = {.x = 0.0f, .y = 20.0f},
            .layout_direction = TOP_TO_BOTTOM,
            .sizing = {.width = FLORA_WIDTH_GROW(100), .height = FLORA_HEIGHT_FIT(100)},
            .position = {.x = 50, .y = 50},
        });
    set_flora_widget_on_mouse_down(child2, base_box_widget_on_mouse_down);

    FloraWidget *child3 = create_flora_box_widget(state, child2, FLORA_TRUE,
        (FloraWidgetStyle){
            .inner_colour = FLORA_CYAN_500,
            .border_colour = FLORA_WHITE,
            .padding = {0.0f, 0.0f, 0.0f, 0.0f},
            .gap = {.x = 0.0f, .y = 0.0f},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = {.width = FLORA_WIDTH_GROW(50), .height = FLORA_HEIGHT_GROW(50)},
            .position = {.x = 50, .y = 50},
        });
    set_flora_widget_on_mouse_down(child3, base_box_widget_on_mouse_down);

    FloraWidget *child4 = create_flora_box_widget(state, child2, FLORA_TRUE,
        (FloraWidgetStyle){
            .inner_colour = FLORA_AMBER_500,
            .border_colour = FLORA_WHITE,
            .padding = {0.0f, 0.0f, 0.0f, 0.0f},
            .gap = {.x = 0.0f, .y = 0.0f},
            .layout_direction = LEFT_TO_RIGHT,
            .sizing = {.width = FLORA_WIDTH_GROW(90), .height = FLORA_HEIGHT_GROW(60)},
            .position = {.x = 50, .y = 50},
        });
    set_flora_widget_on_mouse_down(child4, base_box_widget_on_mouse_down);

    add_flora_child_widget(baseWidget, child1);
    add_flora_child_widget(baseWidget, child2);
    add_flora_child_widget(child2, child3);
    add_flora_child_widget(child2, child4);

    FloraFont *font = add_flora_font(state, OPEN_SANS_FONT_PATH, 24);

    FloraWidget *child5 = create_flora_text_widget(state, child1, FLORA_TRUE, "Hello World.", font,
        (FloraWidgetStyle){
            .text_colour = FLORA_WHITE,
            .font_size = 24,
            .sizing = {.width = FLORA_WIDTH_FIXED(40), .height = FLORA_HEIGHT_FIXED(24)},
            .position = {.x = 50, .y = 50},
        });
    add_flora_child_widget(child1, child5);

    FloraWidget *next_screen_button = create_flora_box_widget(state, NULL, FLORA_TRUE,
        (FloraWidgetStyle){
            .text_colour = FLORA_WHITE,
            .font_size = 24,
            .inner_colour = FLORA_GRAY_900,
            .border_colour = FLORA_WHITE,
            .padding = PADDING(5),
            .sizing = {.width = FLORA_WIDTH_GROW(176), .height = FLORA_HEIGHT_GROW(48)},
            .position = {.x = 25, .y = 25},
        });
    set_flora_widget_on_mouse_down(next_screen_button, change_screen_to_demo);

    FloraWidget *next_screen_button_text = create_flora_text_widget(state, next_screen_button, FLORA_TRUE,
                                                                    "Next Screen", font,
        (FloraWidgetStyle){.text_colour = FLORA_WHITE, .font_size = 24});
    add_flora_child_widget(next_screen_button, next_screen_button_text);
}

int main(const int argc, char **argv) {
    (void) argc;
    (void) argv;

    FloraConfig config = {
        .app_name = "Flora Engine Demo",
        .window_width = 800,
        .window_height = 600,
        .event_queue_capacity = DEFAULT_EVENT_QUEUE_CAPACITY,
        .use_fonts = FLORA_TRUE,
        .window_table_initial_capacity = DEFAULT_WINDOW_TABLE_CAPACITY,
        .font_table_initial_capacity = DEFAULT_FONT_TABLE_CAPACITY,
    };

    FloraApplicationState *state = create_flora(&config);
    if (!state) {
        fprintf(stderr, "Error: Failed to initialize application\n");
        return FLORA_ENGINE_FATAL;
    }

    FloraWindow *window = init_flora_window(state, "Flora Engine Demo");
    if (!window) {
        fprintf(stderr, "Error: Failed to initialize window\n");
        destroy_flora(state);
        return FLORA_ENGINE_FATAL;
    }

    FloraScreen *base = init_flora_screen(state, window, "base", base_create_screen, destroy_flora_screen);
    FloraScreen *demo = init_flora_screen(state, window, "demo", demo_create_screen, destroy_flora_screen);
    if (!base || !demo) {
        fprintf(stderr, "Error: Failed to initialize screens\n");
        destroy_flora(state);
        return FLORA_ENGINE_FATAL;
    }

    if (!set_current_flora_screen(state, base)) {
        fprintf(stderr, "Error: Failed to set current screen\n");
        destroy_flora(state);
        return FLORA_ENGINE_FATAL;
    }

    resume_flora(state);
    run_flora(state);

    destroy_flora(state);
    return FLORA_ENGINE_SUCCESS;
}
