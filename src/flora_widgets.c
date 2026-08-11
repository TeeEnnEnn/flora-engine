#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "flora.h"
#include "flora_apps.h"
#include "flora_constants.h"
#include "flora_fonts.h"
#include "flora_screens.h"
#include "flora_widgets.h"
#include "flora_windows.h"
#include "table.h"

static void calculate_grow_sizing_left_to_right(const FloraWidget *widget)
{
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
		const float growWidth = widget->style.layout_direction == LEFT_TO_RIGHT ? remainingWidth / growCount
																				: remainingWidth;
		for (int i = 0; i < widget->child_count; i++) {
			FloraWidget *child = widget->children[i];
			if (child->is_visible && child->style.sizing.width.type == GROW) {
				child->style.sizing.width.value = growWidth;
			}
		}
	}
}

static void calculate_grow_sizing_top_to_bottom(const FloraWidget *widget)
{
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
		const float growHeight = widget->style.layout_direction == TOP_TO_BOTTOM ? remainingHeight / growCount
																				 : remainingHeight;
		for (int i = 0; i < widget->child_count; i++) {
			FloraWidget *child = widget->children[i];
			if (child->is_visible && child->style.sizing.height.type == GROW) {
				child->style.sizing.height.value = growHeight;
			}
		}
	}
}

static void calculate_widget_grow_width(FloraWidget *widget)
{
	for (int i = 0; i < widget->child_count; i++) {
		FloraWidget *child = widget->children[i];
		calculate_widget_grow_width(child);
	}
	calculate_grow_sizing_left_to_right(widget);
}

static void calculate_widget_grow_height(FloraWidget *widget)
{
	for (int i = 0; i < widget->child_count; i++) {
		FloraWidget *child = widget->children[i];
		calculate_widget_grow_height(child);
	}
	calculate_grow_sizing_top_to_bottom(widget);
}

static void calculate_widget_fit_width(FloraWidget *widget)
{
	// if you have no children you keep your intrinsic size
	if (!widget || widget->is_visible == FLORA_FALSE || widget->child_count == 0) {
		return;
	}

	// find the width of your children before we find the width of you
	for (int i = 0; i < widget->child_count; i++) {
		calculate_widget_fit_width(widget->children[i]);
	}

	float total_widget_width = 0.0f;
	int visible_child_count = 0;

	switch (widget->style.layout_direction) {
	// if you lay your children out left to right then we need to add the width of
	// your children to the total width
	case LEFT_TO_RIGHT: {
		for (int i = 0; i < widget->child_count; i++) {
			FloraWidget *child = widget->children[i];
			if (child->is_visible == FLORA_FALSE)
				continue;
			total_widget_width += child->style.sizing.width.value;
			visible_child_count += 1;
		}
		if (visible_child_count > 0) {
			total_widget_width += (visible_child_count - 1) * widget->style.gap.x;
		}
		break;
	}
	// if you lay your children out top to bottom then we need to add the width of
	// your largest child to the total width
	case TOP_TO_BOTTOM: {
		float widest_child_width = 0.0f;
		for (int i = 0; i < widget->child_count; i++) {
			FloraWidget *child = widget->children[i];
			if (child->is_visible == FLORA_FALSE)
				continue;
			if (child->style.sizing.width.value > widest_child_width) {
				widest_child_width = child->style.sizing.width.value;
			}
		}

		total_widget_width = widest_child_width;
		break;
	}
	}

	total_widget_width += widget->style.padding.left;
	total_widget_width += widget->style.padding.right;
	if (widget->style.sizing.width.type == FIT) {
		widget->style.sizing.width.value = total_widget_width;
	}
}

/**
 * Wrap all text widgets based on the width of their containers, thus
 * calculating their heights
 */
static int calculate_widget_wrapping(FloraWidget *widget)
{
	if (!widget || widget->is_visible == FLORA_FALSE) {
		return FLORA_TRUE;
	}

	// at this point we know how much horizontal space the text widget has
	if (widget->type == FLORA_TEXT) {
		// maximum width of the text widget
		int wrap_width = (int)SDL_round(widget->style.sizing.width.value);

		if (!TTF_SetTextWrapWidth(widget->as.text.ttf_text, wrap_width)) {
			fprintf(stderr, "ERROR: Failed to set wrapped text width: %s\n", SDL_GetError());
			return FLORA_FALSE;
		}
		FloraWidgetStyle style = widget->style;

		SDL_Surface *text_surface = TTF_RenderText_Blended_Wrapped(
			widget->as.text.font, widget->as.text.content, widget->as.text.content_length,
			(SDL_Color){style.text_colour.r, style.text_colour.g, style.text_colour.b, style.text_colour.a},
			wrap_width);

		if (text_surface == NULL) {
			fprintf(stderr, "ERROR: Failed to create wrapped text surface");
			return FLORA_FALSE;
		}

		widget->as.text.surface = text_surface;
		const float text_width = (float)text_surface->w;
		const float text_height = (float)text_surface->h;
		// The wrapped surface defines the true height (and width when fitting) of
		// the text, otherwise it gets clipped to a single line.
		widget->style.sizing.height.value = text_height;
		if (widget->style.sizing.width.type == FIT) {
			widget->style.sizing.width.value = text_width;
		}
	}

	for (int i = 0; i < widget->child_count; i++) {
		calculate_widget_wrapping(widget->children[i]);
	}
	return FLORA_TRUE;
}

static void calculate_widget_fit_height(FloraWidget *widget)
{
	// if you have no children we use your intrinsic height
	if (!widget || widget->is_visible == FLORA_FALSE || widget->child_count == 0) {
		return;
	}

	// find the hight of your children before we find the height of you
	for (int i = 0; i < widget->child_count; i++) {
		calculate_widget_fit_height(widget->children[i]);
	}

	float total_widget_height = 0.0f;
	float visible_child_count = 0.0f;

	switch (widget->style.layout_direction) {
	// if you lay out your children top to bottom then we need to add the heights
	// of your children to the total height
	case TOP_TO_BOTTOM: {
		for (int i = 0; i < widget->child_count; i++) {
			FloraWidget *child = widget->children[i];
			if (child->is_visible == FLORA_FALSE)
				continue;
			total_widget_height += child->style.sizing.height.value;
			visible_child_count += 1;
		}
		if (visible_child_count > 0) {
			total_widget_height += (visible_child_count - 1) * widget->style.gap.y;
		}
		break;
	}
	// if you lay out your children left top right then we need to add the height
	// of your tallest child to the total height
	case LEFT_TO_RIGHT: {
		float widest_child_height = 0.0f;
		for (int i = 0; i < widget->child_count; i++) {
			FloraWidget *child = widget->children[i];
			if (child->is_visible == FLORA_FALSE)
				continue;
			if (child->style.sizing.height.value > widest_child_height) {
				widest_child_height = child->style.sizing.height.value;
			}
		}

		total_widget_height = widest_child_height;
		break;
	}
	}

	total_widget_height += widget->style.padding.top;
	total_widget_height += widget->style.padding.bottom;

	if (widget->style.sizing.height.type == FIT) {
		widget->style.sizing.height.value = total_widget_height;
	}
}

static void calculate_child_positions(FloraWidget *widget)
{
	if (!widget || widget->is_visible == FLORA_FALSE) {
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

			calculate_child_positions(child);
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
			calculate_child_positions(child);
		}
	}
}

static int render_widget(FloraWidget *widget, FloraWindow *window)
{
	if (!widget || widget->is_visible == FLORA_FALSE || !window) {
		return FLORA_TRUE;
	}

	switch (widget->type) {
	case FLORA_TEXT: {
		if (!widget->as.text.surface) {
			fprintf(stderr, "ERROR: Attempted to render text to non existent surface.\n");
			return FLORA_FALSE;
		}

		SDL_Texture *texture = SDL_CreateTextureFromSurface(window->renderer, widget->as.text.surface);
		if (!texture) {
			fprintf(stderr, "ERROR: Failed to create texture to render text\n");
			return FLORA_FALSE;
		}
		widget->as.text.texture = texture;
		SDL_RenderTexture(window->renderer, widget->as.text.texture, NULL,
						  &(SDL_FRect){widget->style.position.x, widget->style.position.y,
									   widget->style.sizing.width.value, widget->style.sizing.height.value});
		break;
	}

	case FLORA_BOX: {
		const FloraWidgetStyle style = widget->style;
		const FloraPosition position = widget->style.position;

		SDL_SetRenderDrawColor(window->renderer, style.inner_colour.r, style.inner_colour.g, style.inner_colour.b,
							   style.inner_colour.a);
		const SDL_FRect rect = {position.x, position.y, widget->style.sizing.width.value,
								widget->style.sizing.height.value};
		SDL_RenderFillRect(window->renderer, &rect);
		SDL_SetRenderDrawColor(window->renderer, style.border_colour.r, style.border_colour.g, style.border_colour.b,
							   style.border_colour.a);
		SDL_RenderRect(window->renderer, &rect);

		for (int i = 0; i < widget->child_count; i++) {
			FloraWidget *child = widget->children[i];
			if (child && child->is_visible) {
				render_widget(child, window);
			}
		}
		break;
	}
	}
	return FLORA_TRUE;
}

void base_text_widget_render(FloraWidget *widget, FloraWindow *window)
{
	if (!widget || !widget->is_visible) {
		return;
	}
	render_widget(widget, window);
}

FloraWidget *create_flora_box_widget(FloraApplicationState *state, FloraWidget *parent, const int is_visible,
									 FloraWidgetStyle style)
{
	if (!state) {
		fprintf(stderr, "Error: Widget create got invalid application state.");
		return NULL;
	}
	if (!state->current_window) {
		fprintf(stderr, "Error: No valid window found to add widgets to.");
		return NULL;
	}
	FloraScreen *screen = state->current_window->current_screen;
	if (!screen) {
		fprintf(stderr, "Error: No valid screen found to add widgets to.");
		return NULL;
	}

	if (screen->widget_count == screen->widget_capacity) {
		const int new_capacity = (screen->widget_capacity == 0) ? INITIAL_WIDGET_CAPACITY
																: screen->widget_capacity * GROWTH_FACTOR;
		FloraWidget **new_widgets = realloc(screen->widgets, new_capacity * sizeof(FloraWidget *));
		if (!new_widgets) {
			fprintf(stderr, "Error: Could not reallocate widgets.");
			return NULL;
		}
		screen->widgets = new_widgets;
		screen->widget_capacity = new_capacity;
	}

	FloraWidget *new_widget = calloc(1, sizeof(FloraWidget));
	if (!new_widget) {
		fprintf(stderr, "Error: Could not allocate widget.\n");
		return NULL;
	}
	new_widget->id = screen->widget_count++;
	new_widget->is_visible = is_visible ? FLORA_TRUE : FLORA_FALSE;
	new_widget->style = style;
	new_widget->callbacks.render = base_box_widget_render;
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
FloraWidget *create_flora_text_widget(FloraApplicationState *state, FloraWindow *window, FloraWidget *parent,
									  const int is_visible, const char *text, const char *font_name,
									  FloraWidgetStyle style)
{
	if (window == NULL) {
		fprintf(stderr, "ERROR: NULL window attempted to create a text widget.\n");
		return NULL;
	}
	FloraScreen *screen = window->current_screen;
	if (!screen) {
		fprintf(stderr, "Error: No valid screen found to add widgets to.\n");
		return NULL;
	}
	FloraFont *font = NULL;
	get_table(&state->font_table, (void **)&font, font_name);
	if (font == NULL) {
		fprintf(stderr, "ERROR: Failed to get font with name: %s.\n", font_name);
		return FLORA_FALSE;
	}

	if (screen->widget_count == screen->widget_capacity) {
		const int new_capacity = (screen->widget_capacity == 0) ? INITIAL_WIDGET_CAPACITY
																: screen->widget_capacity * GROWTH_FACTOR;
		FloraWidget **new_widgets = realloc(screen->widgets, new_capacity * sizeof(FloraWidget *));
		if (!new_widgets) {
			fprintf(stderr, "Error: Could not reallocate widgets.\n");
			return NULL;
		}
		screen->widgets = new_widgets;
		screen->widget_capacity = new_capacity;
	}
	if (!text) {
		fprintf(stderr, "Error: Text widget created with a null string.\n");
		return NULL;
	}
	if (!font || !font->font) {
		fprintf(stderr, "Error: Text font widget created with a null font.\n");
		return NULL;
	}

	// take ownership of string content
	const int length = (int)strlen(text);
	char *content = malloc((size_t)length + 1);
	if (!content) {
		fprintf(stderr, "Error: Could not allocate memory for string.\n");
		return NULL;
	}
	memcpy(content, text, (size_t)length + 1);

	TTF_Text *ttf_text = TTF_CreateText(window->text_engine, font->font, content, length);
	if (!ttf_text) {
		fprintf(stderr, "Error: TTF_CreateText failed: %s\n", SDL_GetError());
		free(content);
		return NULL;
	}

	// Measure the text so layout sees the correct dimensions.
	SDL_Surface *surface = TTF_RenderText_Blended(font->font, content, length, (SDL_Color){0, 0, 0, 255});
	if (!surface) {
		fprintf(stderr, "Error: TTF_RenderText_Blended failed.\n");
		free(content);
		return NULL;
	}
	const float text_width = (float)surface->w;
	const float text_height = (float)surface->h;
	SDL_DestroySurface(surface);

	FloraWidget *new_widget = calloc(1, sizeof(FloraWidget));
	if (!new_widget) {
		free(content);
		fprintf(stderr, "Error: Could not allocate widget.\n");
		return NULL;
	}
	new_widget->id = screen->widget_count++;
	new_widget->is_visible = is_visible ? FLORA_TRUE : FLORA_FALSE;
	new_widget->style = style;
	/* Auto-size to the measured text unless the caller provided explicit sizing.
	 */
	if (style.sizing.width.type == FIT && style.sizing.width.value == 0.0f && style.sizing.height.type == FIT &&
		style.sizing.height.value == 0.0f) {
		new_widget->style.sizing.width = (FloraDimension){FIXED, text_width};
		new_widget->style.sizing.height = (FloraDimension){FIXED, text_height};
	}
	new_widget->callbacks.render = base_text_widget_render;
	new_widget->child_capacity = INITIAL_CHILD_WIDGET_CAPACITY;
	new_widget->parent = parent;
	new_widget->children = NULL;
	new_widget->child_count = 0;
	new_widget->type = FLORA_TEXT;

	new_widget->as.text.content = content;
	new_widget->as.text.content_length = length;
	new_widget->as.text.font = font->font;
	new_widget->as.text.ttf_text = ttf_text;
	new_widget->as.text.surface = NULL;
	new_widget->as.text.texture = NULL;

	screen->widgets[screen->widget_count - 1] = new_widget;
	return new_widget;
}

int add_flora_child_widget(FloraWidget *widget, FloraWidget *child)
{
	if (!widget) {
		fprintf(stderr, "Error: Widget add got invalid parent widget.\n");
		return FLORA_FALSE;
	}
	if (!child) {
		fprintf(stderr, "Error: Child widget add got invalid child widget.\n");
		return FLORA_FALSE;
	}
	if (!widget->children) {
		widget->child_capacity = INITIAL_CHILD_WIDGET_CAPACITY;
		widget->child_count = 0;
		widget->children = calloc(widget->child_capacity, sizeof(FloraWidget *));
		if (!widget->children) {
			fprintf(stderr, "Error: Could not allocate widgets.\n");
			return FLORA_FALSE;
		}
	}
	if (widget->child_count == widget->child_capacity && widget->children) {
		FloraWidget **new_widgets = realloc(widget->children,
											widget->child_capacity * GROWTH_FACTOR * sizeof(FloraWidget *));
		if (!new_widgets) {
			fprintf(stderr, "Error: Could not reallocate widgets.\n");
			return FLORA_FALSE;
		}
		widget->children = new_widgets;
		widget->child_capacity = widget->child_capacity * GROWTH_FACTOR;
	}
	widget->children[widget->child_count++] = child;
	child->parent = widget;
	return FLORA_TRUE;
}

void set_flora_widget_style(FloraWidget *widget, FloraWidgetStyle style)
{
	if (!widget) {
		return;
	}
	const FloraColour old_text_colour = widget->style.text_colour;
	widget->style = style;
	if (widget->type == FLORA_TEXT &&
		(old_text_colour.r != style.text_colour.r || old_text_colour.g != style.text_colour.g ||
		 old_text_colour.b != style.text_colour.b || old_text_colour.a != style.text_colour.a) &&
		widget->as.text.texture) {
		SDL_DestroyTexture(widget->as.text.texture);
		widget->as.text.texture = NULL;
	}
}

void set_flora_widget_position(FloraWidget *widget, float x, float y)
{
	if (!widget) {
		return;
	}
	widget->style.position.x = x;
	widget->style.position.y = y;
}

void set_flora_widget_width(FloraWidget *widget, FloraSizingType type, float value)
{
	if (!widget) {
		return;
	}
	widget->style.sizing.width.type = type;
	widget->style.sizing.width.value = value;
}

void set_flora_widget_height(FloraWidget *widget, FloraSizingType type, float value)
{
	if (!widget) {
		return;
	}
	widget->style.sizing.height.type = type;
	widget->style.sizing.height.value = value;
}

void set_flora_widget_inner_colour(FloraWidget *widget, FloraColour colour)
{
	if (!widget) {
		return;
	}
	widget->style.inner_colour = colour;
}

void set_flora_widget_border_colour(FloraWidget *widget, FloraColour colour)
{
	if (!widget) {
		return;
	}
	widget->style.border_colour = colour;
}

void set_flora_widget_text_colour(FloraWidget *widget, FloraColour colour)
{
	if (!widget) {
		return;
	}
	widget->style.text_colour = colour;
	if (widget->type == FLORA_TEXT && widget->as.text.texture) {
		SDL_DestroyTexture(widget->as.text.texture);
		widget->as.text.texture = NULL;
	}
}

void set_flora_widget_font_size(FloraWidget *widget, int size)
{
	if (!widget) {
		return;
	}
	widget->style.font_size = size;
}

void set_flora_widget_padding(FloraWidget *widget, float left, float right, float top, float bottom)
{
	if (!widget) {
		return;
	}
	widget->style.padding = (FloraPadding){left, right, top, bottom};
}

void set_flora_widget_gap(FloraWidget *widget, float x, float y)
{
	if (!widget) {
		return;
	}
	widget->style.gap = (FloraGap){x, y};
}

void set_flora_widget_layout_direction(FloraWidget *widget, FloraLayoutDirection direction)
{
	if (!widget) {
		return;
	}
	widget->style.layout_direction = direction;
}

void set_flora_widget_visible(FloraWidget *widget, int is_visible)
{
	if (!widget) {
		return;
	}
	widget->is_visible = is_visible ? FLORA_TRUE : FLORA_FALSE;
}

void set_flora_widget_update(FloraWidget *widget, widget_update update)
{
	if (!widget) {
		return;
	}
	widget->callbacks.update = update;
}

void set_flora_widget_render(FloraWidget *widget, widget_render render)
{
	if (!widget) {
		return;
	}
	widget->callbacks.render = render;
}

void set_flora_widget_on_mouse_down(FloraWidget *widget, widget_on_mouse_down on_mouse_down)
{
	if (!widget) {
		return;
	}
	widget->callbacks.on_mouse_down = on_mouse_down;
}

void set_flora_widget_on_destroy(FloraWidget *widget, widget_on_destroy on_destroy)
{
	if (!widget) {
		return;
	}
	widget->callbacks.on_destroy = on_destroy;
}

void base_box_widget_render(FloraWidget *widget, FloraWindow *window)
{
	if (!widget || !widget->is_visible) {
		return;
	}

	calculate_widget_fit_width(widget);
	calculate_widget_grow_width(widget);
	calculate_widget_wrapping(widget);
	calculate_widget_fit_height(widget);
	calculate_widget_grow_height(widget);
	calculate_child_positions(widget);
	render_widget(widget, window);
}

void base_box_widget_on_mouse_down(FloraWidget *widget, FloraApplicationState *state)
{
	(void)state;
	widget->style.inner_colour =
		(FloraColour){.r = rand() % 255, .g = rand() % 255, .b = rand() % 255, .a = rand() % 255};
	widget->style.border_colour =
		(FloraColour){.r = rand() % 255, .g = rand() % 255, .b = rand() % 255, .a = rand() % 255};
}

int widget_contains_point(FloraWidget *widget, const int x, const int y)
{
	if (!widget || !widget->is_visible) {
		return FLORA_FALSE;
	}

	return x >= widget->style.position.x && x < widget->style.position.x + widget->style.sizing.width.value &&
		   y >= widget->style.position.y && y < widget->style.position.y + widget->style.sizing.height.value;
}

int cleanup_widget(FloraWidget *widget)
{
	if (!widget) {
		return FLORA_FALSE;
	}

	switch (widget->type) {
	case FLORA_TEXT:
		if (widget->as.text.ttf_text) {
			TTF_DestroyText(widget->as.text.ttf_text);
			widget->as.text.ttf_text = NULL;
		}
		if (widget->as.text.content) {
			free(widget->as.text.content);
			widget->as.text.content = NULL;
		}
		if (widget->as.text.texture) {
			SDL_DestroyTexture(widget->as.text.texture);
			widget->as.text.texture = NULL;
		}
		if (widget->as.text.surface) {
			SDL_DestroySurface(widget->as.text.surface);
			widget->as.text.surface = NULL;
		}
		break;
	case FLORA_BOX:
		break;
	}

	if (widget->children) {
		free(widget->children);
		widget->children = NULL;
	}
	return FLORA_TRUE;
}
