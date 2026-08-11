#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flora.h"

#define OPEN_SANS_FONT_PATH "assets/fonts/Open_Sans/OpenSans-VariableFont_wdth,wght.ttf"
#define BASIC_FONT_NAME "basic"
#define TITLE_FONT_NAME "title"
#define BASE_FONT_NAME "base"

void navigate_to_screen(FloraApplicationState *state, char *screen_name)
{
	FloraScreen *target = get_flora_screen(get_current_flora_window(state), screen_name);
	if (!target) {
		printf("Log: Error: Could not find screen \"%s\".\n", screen_name);
		return;
	}
	if (!set_current_flora_screen(state, target)) {
		printf("Log: Error: Could not change screen to \"%s\".\n", screen_name);
	}
}

void go_to_base(FloraWidget *widget, FloraApplicationState *state)
{
	(void)widget;
	navigate_to_screen(state, "base");
}

void go_to_demo(FloraWidget *widget, FloraApplicationState *state)
{
	(void)widget;
	navigate_to_screen(state, "demo");
}

void go_to_wrapping(FloraWidget *widget, FloraApplicationState *state)
{
	(void)widget;
	navigate_to_screen(state, "wrapping");
}

#define NAV_BAR_BUTTON_WIDTH 140.0f
#define NAV_BAR_Y 524.0f

// Creates a nav bar pinned to the bottom of the window as its own root widget.
// It is created last in each screen, so it renders on top and gets the first
// chance to handle clicks. Fixed-width buttons avoid depending on GROW layout.
FloraWidget *create_nav_bar(FloraApplicationState *state, FloraWindow *window, const char *back_label,
							widget_on_mouse_down on_back, const char *forward_label, widget_on_mouse_down on_forward)
{
	// The nav bar needs the base font regardless of what the screen registered.
	add_flora_font(state, OPEN_SANS_FONT_PATH, 18, BASIC_FONT_NAME);

	FloraWidget *navBar = create_flora_box_widget(state, NULL, FLORA_TRUE,
												  (FloraWidgetStyle){
													  .inner_colour = FLORA_GRAY_900,
													  .border_colour = FLORA_SLATE_600,
													  .padding = PADDING(10),
													  .gap = {.x = 12.0f, .y = 0.0f},
													  .layout_direction = LEFT_TO_RIGHT,
													  .sizing = {.width = FLORA_WIDTH_FIT(0),
																 .height = FLORA_HEIGHT_FIT(0)},
													  .position = {.x = 0, .y = 0},
												  });

	FloraWidget *backButton = create_flora_box_widget(state, navBar, FLORA_TRUE,
													  (FloraWidgetStyle){
														  .inner_colour = FLORA_SLATE_700,
														  .border_colour = FLORA_SLATE_400,
														  .padding = PADDING(8),
														  .gap = {.x = 0.0f, .y = 0.0f},
														  .layout_direction = LEFT_TO_RIGHT,
														  .sizing = {.width = FLORA_WIDTH_FIXED(NAV_BAR_BUTTON_WIDTH),
																	 .height = FLORA_HEIGHT_FIT(0)},
														  .position = {.x = 0, .y = 0},
													  });
	set_flora_widget_on_mouse_down(backButton, on_back);
	FloraWidget *backText = create_flora_text_widget(state, window, backButton, FLORA_TRUE, back_label, BASIC_FONT_NAME,
													 (FloraWidgetStyle){.text_colour = FLORA_WHITE, .font_size = 18});
	add_flora_child_widget(backButton, backText);
	add_flora_child_widget(navBar, backButton);

	FloraWidget *forwardButton = create_flora_box_widget(
		state, navBar, FLORA_TRUE,
		(FloraWidgetStyle){
			.inner_colour = FLORA_SLATE_700,
			.border_colour = FLORA_SLATE_400,
			.padding = PADDING(8),
			.gap = {.x = 0.0f, .y = 0.0f},
			.layout_direction = LEFT_TO_RIGHT,
			.sizing = {.width = FLORA_WIDTH_FIXED(NAV_BAR_BUTTON_WIDTH), .height = FLORA_HEIGHT_FIT(0)},
			.position = {.x = 0, .y = 0},
		});
	set_flora_widget_on_mouse_down(forwardButton, on_forward);
	FloraWidget *forwardText = create_flora_text_widget(state, window, forwardButton, FLORA_TRUE, forward_label,
														BASIC_FONT_NAME,
														(FloraWidgetStyle){.text_colour = FLORA_WHITE,
																		   .font_size = 18});
	add_flora_child_widget(forwardButton, forwardText);
	add_flora_child_widget(navBar, forwardButton);

	// Center the bar horizontally: 2 buttons + gap + padding.
	const float nav_width = NAV_BAR_BUTTON_WIDTH * 2.0f + 12.0f + 20.0f;
	set_flora_widget_position(navBar, (800.0f - nav_width) / 2.0f, NAV_BAR_Y);
	return navBar;
}

void demo_create_screen(FloraApplicationState *state, FloraScreen *screen)
{
	(void)screen;
	add_flora_font(state, OPEN_SANS_FONT_PATH, 18, BASIC_FONT_NAME);
	add_flora_font(state, OPEN_SANS_FONT_PATH, 32, TITLE_FONT_NAME);
	FloraWindow *window = get_current_flora_window(state);

	// Main container - full window with dark background
	FloraWidget *mainContainer = create_flora_box_widget(state, NULL, FLORA_TRUE,
														 (FloraWidgetStyle){
															 .inner_colour = FLORA_SLATE_900,
															 .border_colour = FLORA_SLATE_700,
															 .padding = PADDING(20),
															 .gap = {.x = 0.0f, .y = 20.0f},
															 .layout_direction = TOP_TO_BOTTOM,
															 .sizing = {.width = FLORA_WIDTH_FIXED(760),
																		.height = FLORA_HEIGHT_FIT(0)},
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
													  .sizing = {.width = FLORA_WIDTH_GROW(1),
																 .height = FLORA_HEIGHT_FIT(0)},
													  .position = {.x = 0, .y = 0},
												  });
	set_flora_widget_on_mouse_down(header, base_box_widget_on_mouse_down);

	FloraWidget *headerTitle = create_flora_text_widget(state, window, header, FLORA_TRUE, "Flora Engine Demo",
														TITLE_FONT_NAME,
														(FloraWidgetStyle){.text_colour = FLORA_WHITE,
																		   .font_size = 32});

	FloraWidget *headerSubtitle = create_flora_text_widget(state, window, header, FLORA_TRUE,
														   "Click widgets to randomize colors", BASIC_FONT_NAME,
														   (FloraWidgetStyle){.text_colour = FLORA_INDIGO_200,
																			  .font_size = 18});

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
														 .sizing = {.width = FLORA_WIDTH_GROW(1),
																	.height = FLORA_HEIGHT_FIT(0)},
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
														  .sizing = {.width = FLORA_WIDTH_GROW(1),
																	 .height = FLORA_HEIGHT_FIT(0)},
														  .position = {.x = 0, .y = 0},
													  });
		set_flora_widget_on_mouse_down(button, base_box_widget_on_mouse_down);

		FloraWidget *buttonText = create_flora_text_widget(state, window, button, FLORA_TRUE, button_labels[i],
														   BASIC_FONT_NAME,
														   (FloraWidgetStyle){.text_colour = FLORA_WHITE,
																			  .font_size = 18});

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
														  .sizing = {.width = FLORA_WIDTH_GROW(1),
																	 .height = FLORA_HEIGHT_FIXED(140)},
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
														  .sizing = {.width = FLORA_WIDTH_GROW(1),
																	 .height = FLORA_HEIGHT_GROW(1)},
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
														.sizing = {.width = FLORA_WIDTH_GROW(1),
																   .height = FLORA_HEIGHT_GROW(1)},
														.position = {.x = 0, .y = 0},
													});
		set_flora_widget_on_mouse_down(card, base_box_widget_on_mouse_down);

		FloraWidget *cardText = create_flora_text_widget(state, window, card, FLORA_TRUE, card_texts[i],
														 BASIC_FONT_NAME,
														 (FloraWidgetStyle){.text_colour = FLORA_WHITE,
																			.font_size = 18});

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
														   .sizing = {.width = FLORA_WIDTH_GROW(1),
																	  .height = FLORA_HEIGHT_GROW(1)},
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
													  .sizing = {.width = FLORA_WIDTH_GROW(1),
																 .height = FLORA_HEIGHT_GROW(2)},
													  .position = {.x = 0, .y = 0},
												  });
	set_flora_widget_on_mouse_down(topBox, base_box_widget_on_mouse_down);

	FloraWidget *topBoxText = create_flora_text_widget(state, window, topBox, FLORA_TRUE, "Nested Layout",
													   BASIC_FONT_NAME,
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
														 .sizing = {.width = FLORA_WIDTH_GROW(1),
																	.height = FLORA_HEIGHT_GROW(1)},
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
															.sizing = {.width = FLORA_WIDTH_GROW(1),
																	   .height = FLORA_HEIGHT_GROW(1)},
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
													  .sizing = {.width = FLORA_WIDTH_GROW(1),
																 .height = FLORA_HEIGHT_FIT(0)},
													  .position = {.x = 0, .y = 0},
												  });
	set_flora_widget_on_mouse_down(footer, base_box_widget_on_mouse_down);

	FloraWidget *footerText = create_flora_text_widget(state, window, footer, FLORA_TRUE, "Press ESC to quit",
													   BASIC_FONT_NAME,
													   (FloraWidgetStyle){.text_colour = FLORA_SLATE_300,
																		  .font_size = 18});

	add_flora_child_widget(footer, footerText);
	add_flora_child_widget(mainContainer, footer);

	// Navigation bar
	create_nav_bar(state, window, "Back", go_to_base, "Next", go_to_wrapping);
}

void base_create_screen(FloraApplicationState *state, FloraScreen *screen)
{
	(void)screen;
	add_flora_font(state, OPEN_SANS_FONT_PATH, 24, BASE_FONT_NAME);
	FloraWindow *window = get_current_flora_window(state);

	FloraWidget *baseWidget = create_flora_box_widget(state, NULL, FLORA_TRUE,
													  (FloraWidgetStyle){
														  .inner_colour = FLORA_SLATE_500,
														  .border_colour = FLORA_WHITE,
														  .padding = {20.0f, 20.0f, 10.0f, 10.0f},
														  .gap = {.x = 25.0f, .y = 0.0f},
														  .layout_direction = LEFT_TO_RIGHT,
														  .sizing = {.width = FLORA_WIDTH_FIXED(460),
																	 .height = FLORA_HEIGHT_FIT(0)},
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
													  .sizing = {.width = FLORA_WIDTH_GROW(125),
																 .height = FLORA_HEIGHT_GROW(50)},
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
													  .sizing = {.width = FLORA_WIDTH_GROW(100),
																 .height = FLORA_HEIGHT_FIT(100)},
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
													  .sizing = {.width = FLORA_WIDTH_GROW(50),
																 .height = FLORA_HEIGHT_GROW(50)},
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
													  .sizing = {.width = FLORA_WIDTH_GROW(90),
																 .height = FLORA_HEIGHT_GROW(60)},
													  .position = {.x = 50, .y = 50},
												  });
	set_flora_widget_on_mouse_down(child4, base_box_widget_on_mouse_down);

	add_flora_child_widget(child2, child3);
	add_flora_child_widget(child2, child4);
	add_flora_child_widget(baseWidget, child1);
	add_flora_child_widget(baseWidget, child2);

	// Auto-sized so the text renders on a single line instead of wrapping early
	FloraWidget *child5 = create_flora_text_widget(state, window, child1, FLORA_TRUE, "Hello World.", BASE_FONT_NAME,
												   (FloraWidgetStyle){.text_colour = FLORA_WHITE,
																	  .font_size = 24,
																	  .position = {.x = 50, .y = 50}});
	add_flora_child_widget(child1, child5);

	// Navigation bar
	create_nav_bar(state, window, "Back", go_to_wrapping, "Next", go_to_demo);
}

void wrapping_create_screen(FloraApplicationState *state, FloraScreen *screen)
{
	(void)screen;
	add_flora_font(state, OPEN_SANS_FONT_PATH, 18, BASIC_FONT_NAME);
	add_flora_font(state, OPEN_SANS_FONT_PATH, 32, TITLE_FONT_NAME);
	FloraWindow *window = get_current_flora_window(state);

	// Main container - full window with dark background
	FloraWidget *mainContainer = create_flora_box_widget(state, NULL, FLORA_TRUE,
														 (FloraWidgetStyle){
															 .inner_colour = FLORA_SLATE_900,
															 .border_colour = FLORA_SLATE_700,
															 .padding = PADDING(15),
															 .gap = {.x = 0.0f, .y = 10.0f},
															 .layout_direction = TOP_TO_BOTTOM,
															 .sizing = {.width = FLORA_WIDTH_FIXED(760),
																		.height = FLORA_HEIGHT_FIT(0)},
															 .position = {.x = 20, .y = 20},
														 });

	FloraWidget *title = create_flora_text_widget(state, window, mainContainer, FLORA_TRUE, "Text Wrapping",
												  TITLE_FONT_NAME,
												  (FloraWidgetStyle){.text_colour = FLORA_WHITE, .font_size = 32});

	FloraWidget *subtitle =
		create_flora_text_widget(state, window, mainContainer, FLORA_TRUE,
								 "Long text wraps at the width of its box; the box grows to fit the "
								 "lines.",
								 BASIC_FONT_NAME, (FloraWidgetStyle){.text_colour = FLORA_SLATE_300, .font_size = 18});

	add_flora_child_widget(mainContainer, title);
	add_flora_child_widget(mainContainer, subtitle);

	const FloraWidgetStyle wrap_style_300 = (FloraWidgetStyle){
		.sizing = {.width = FLORA_WIDTH_FIXED(300), .height = FLORA_HEIGHT_FIT(0)}};
	const FloraWidgetStyle wrap_style_500 = (FloraWidgetStyle){
		.sizing = {.width = FLORA_WIDTH_FIXED(500), .height = FLORA_HEIGHT_FIT(0)}};
	const FloraWidgetStyle wrap_style_grow = (FloraWidgetStyle){
		.sizing = {.width = FLORA_WIDTH_GROW(1), .height = FLORA_HEIGHT_FIT(0)}};
	const char *long_text = "Text wraps at the width given to it, and the box grows to fit all the "
							"wrapped lines.";

	const struct {
		float box_width;
		FloraWidgetStyle text_style;
		const char *label;
		FloraColour box_colour;
	} examples[] = {
		{340.0f, wrap_style_300, "Wrapped at 300px", FLORA_INDIGO_600},
		{540.0f, wrap_style_500, "Wrapped at 500px", FLORA_PURPLE_600},
		{0.0f, wrap_style_grow, "Wrapped at container width", FLORA_CYAN_700},
	};

	for (int i = 0; i < 3; i++) {
		const float box_width = examples[i].box_width == 0.0f ? 0.0f : examples[i].box_width;

		FloraWidget *box = create_flora_box_widget(
			state, mainContainer, FLORA_TRUE,
			(FloraWidgetStyle){
				.inner_colour = examples[i].box_colour,
				.border_colour = FLORA_WHITE,
				.padding = PADDING(8),
				.gap = {.x = 0.0f, .y = 6.0f},
				.layout_direction = TOP_TO_BOTTOM,
				.sizing = {.width = box_width == 0.0f ? FLORA_WIDTH_GROW(1) : FLORA_WIDTH_FIXED(box_width),
						   .height = FLORA_HEIGHT_FIT(0)},
				.position = {.x = 0, .y = 0},
			});

		FloraWidget *label = create_flora_text_widget(state, window, box, FLORA_TRUE, examples[i].label,
													  BASIC_FONT_NAME,
													  (FloraWidgetStyle){.text_colour = FLORA_WHITE, .font_size = 18});

		FloraWidget *text = create_flora_text_widget(state, window, box, FLORA_TRUE, long_text, BASIC_FONT_NAME,
													 (FloraWidgetStyle){.text_colour = FLORA_WHITE,
																		.font_size = 18,
																		.sizing = examples[i].text_style.sizing});

		add_flora_child_widget(box, label);
		add_flora_child_widget(box, text);
		add_flora_child_widget(mainContainer, box);
	}

	// Navigation bar
	create_nav_bar(state, window, "Back", go_to_demo, "Next", go_to_base);
}

int main(const int argc, char **argv)
{
	(void)argc;
	(void)argv;

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
	FloraScreen *wrapping = init_flora_screen(state, window, "wrapping", wrapping_create_screen, destroy_flora_screen);
	if (!base || !demo || !wrapping) {
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
