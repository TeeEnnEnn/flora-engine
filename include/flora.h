#ifndef FLORA_H
#define FLORA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define FLORA_TRUE 1
#define FLORA_FALSE 0
#define FLORA_ENGINE_FATAL 1
#define FLORA_ENGINE_SUCCESS 0

#define DEFAULT_EVENT_QUEUE_CAPACITY 64
#define DEFAULT_WINDOW_TABLE_CAPACITY 4
#define DEFAULT_FONT_TABLE_CAPACITY 4

typedef struct {
	char *app_name;
	int event_queue_capacity;
	int use_fonts;
	int window_table_initial_capacity;
	int font_table_initial_capacity;
} FloraConfig;

typedef struct FloraApplicationState FloraApplicationState;

typedef struct FloraWindow FloraWindow;

typedef struct FloraScreen FloraScreen;

typedef struct FloraWidget FloraWidget;

typedef struct FloraFont FloraFont;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} FloraColour;

typedef enum FloraLayoutDirection { LEFT_TO_RIGHT, TOP_TO_BOTTOM } FloraLayoutDirection;

typedef enum FloraSizingType { FIT, FIXED, GROW } FloraSizingType;

typedef enum FloraWidgetType { FLORA_BOX, FLORA_TEXT } FloraWidgetType;


// TODO: change this to widget, screen
typedef void (*widget_callback)(FloraWidget *widget, FloraApplicationState *state);

// TODO: change this to screen, window
typedef void (*screen_callback)(FloraApplicationState *state, FloraScreen *screen);

// TODO: change this to window, state
typedef void (*window_callback)(FloraApplicationState *state, FloraWindow *window);


typedef struct {
	float left;
	float right;
	float top;
	float bottom;
} FloraPadding;

typedef struct {
	float x;
	float y;
} FloraGap;

typedef struct {
	FloraSizingType type;
	float value;
} FloraDimension;

typedef struct {
	FloraDimension width;
	FloraDimension height;
} FloraSizing;

typedef struct {
	float x;
	float y;
} FloraPosition;

typedef struct {
    /** is not used if not a text widget */
	FloraColour text_colour;
	int font_size;
	FloraColour inner_colour;
	FloraColour border_colour;
	FloraPadding padding;
	FloraGap gap;
	FloraLayoutDirection layout_direction;
	FloraSizing sizing;
	FloraPosition position;
} FloraWidgetStyle;

typedef struct {
    /** Called on every frame */
	widget_callback on_update;
	/** Called when the mouse down event reaches this widget */
	widget_callback on_mouse_down;
	/** Called when the widget is destroyed */
	widget_callback on_destroy;
} FloraWidgetCallbacks;

/* Sizing helpers - build a FloraDimension / FloraSizing / FloraPadding inline. */
#define FLORA_WIDTH_FIXED(v) ((FloraDimension){.type = FIXED, .value = (v)})
#define FLORA_WIDTH_GROW(v) ((FloraDimension){.type = GROW, .value = (v)})
#define FLORA_WIDTH_FIT(v) ((FloraDimension){.type = FIT, .value = (v)})
#define FLORA_HEIGHT_FIXED(v) ((FloraDimension){.type = FIXED, .value = (v)})
#define FLORA_HEIGHT_GROW(v) ((FloraDimension){.type = GROW, .value = (v)})
#define FLORA_HEIGHT_FIT(v) ((FloraDimension){.type = FIT, .value = (v)})
#define PADDING(v) ((FloraPadding){.left = (v), .right = (v), .top = (v), .bottom = (v)})
#define PADDING_X(v) ((FloraPadding){.left = (v), .right = (v), .top = 0, .bottom = 0})
#define PADDING_Y(v) ((FloraPadding){.left = 0, .right = 0, .top = (v), .bottom = (v)})

/**
 * Create a flora application state and initialize it.
 * @param config The configuration for the application (can be NULL for defaults)
 * @return The initialized application state, or NULL on failure
 */
FloraApplicationState *create_flora(FloraConfig *config);

/**
 * Deinitialize a flora application state and release all of its resources.
 * @param state The application state to destroy
 */
void destroy_flora(FloraApplicationState *state);

/**
 * Resume the application loop by setting the running flag to true.
 * @param state The application state
 */
void resume_flora(FloraApplicationState *state);

/**
 * Pause the application loop by setting the running flag to false.
 * @param state The application state
 */
void pause_flora(FloraApplicationState *state);

/**
 * Run the application frame loop until the running flag is cleared
 * (e.g. via the ESC key or a quit event).
 * @param state The application state
 */
void run_flora(FloraApplicationState *state);

/**
 * Get the current window of the application.
 * @param state The flora application state
 * @return The current window, or NULL if none is set
 */
FloraWindow *get_current_flora_window(FloraApplicationState *state);

/**
 * Create a window owned by the application and register it in the state's window table.
 * @param state The flora application state
 * @param window_title The title of the window (also used as the table key)
 * @return The initialized window, or NULL on failure
 */
FloraWindow *init_flora_window(FloraApplicationState *state, char *window_title, int window_width, int window_height);

/**
 * Destroy a window and all of its screens.
 * @param state The flora application state
 * @param window The window to destroy
 */
void deinit_flora_window(FloraApplicationState *state, FloraWindow *window);

/**
 * Get a window by its title.
 * @param state The flora application state
 * @param name The title (table key) of the window
 * @return The window, or NULL if not found
 */
FloraWindow *get_flora_window(FloraApplicationState *state, char *name);

/**
 * Set the current window of the application.
 * @param state The flora application state
 * @param window The window to set as current
 * @return FLORA_TRUE on success, FLORA_FALSE otherwise
 */
int set_current_flora_window(FloraApplicationState *state, FloraWindow *window);

/**
 * Create a screen owned by a window and register it in the window's screen table.
 * @param state The flora application state
 * @param window The owning window
 * @param screen_name The name of the screen (also used as the table key)
 * @param on_init_screen Callback invoked when the screen becomes current
 * @param on_deinit_screen Callback invoked when the screen is destroyed
 * @return The initialized screen, or NULL on failure
 */
FloraScreen *init_flora_screen(FloraApplicationState *state, FloraWindow *window, char *screen_name,
							   screen_callback on_init_screen, screen_callback on_deinit_screen);

/**
 * Get a screen from a window by its name.
 * @param window The owning window
 * @param name The name (table key) of the screen
 * @return The screen, or NULL if not found
 */
FloraScreen *get_flora_screen(FloraWindow *window, char *name);

/**
 * Set the current screen of the current window.
 * @param state The flora application state
 * @param screen The screen to set as current
 * @return FLORA_TRUE on success, FLORA_FALSE otherwise
 */
int set_current_flora_screen(FloraApplicationState *state, FloraScreen *screen);

/**
 * Destroy a screen and all of the widgets it owns.
 * Matches the on_deinit_screen callback signature.
 * @param state The flora application state
 * @param screen The screen to destroy
 */
void destroy_flora_screen(FloraApplicationState *state, FloraScreen *screen);

/**
 * Load a font (cached by path) and return an opaque handle for use with text widgets.
 * @param state The flora application state
 * @param path The path to the font file
 * @param point_size The point size to load
 * @param font_name The name of the font - used to identify the font
 * @return The font handle, or NULL on failure
 */
FloraFont *add_flora_font(FloraApplicationState *state, const char *path, float point_size, char *font_name);

/**
 * Create a box widget.
 * @param state The flora application state
 * @param parent The parent widget (NULL for a root widget)
 * @param is_visible FLORA_TRUE / FLORA_FALSE
 * @param style The initial style (use a (FloraWidgetStyle){...} literal or {0} for defaults)
 * @return The created widget, or NULL on failure
 */
FloraWidget *create_flora_box_widget(FloraApplicationState *state, FloraWidget *parent, int is_visible,
									 FloraWidgetStyle style);

/**
 * Create a text widget.
 * The text is copied, so the caller keeps ownership of the string.
 * The widget is auto-sized to the measured text size unless the style overrides "sizing".
 * @param state The flora application state
 * @param parent The parent widget (NULL for a root widget)
 * @param is_visible FLORA_TRUE / FLORA_FALSE
 * @param text The text to render
 * @param font The font handle returned by add_flora_font
 * @param style The initial style (use a (FloraWidgetStyle){...} literal or {0} for defaults)
 * @return The created widget, or NULL on failure
 */
FloraWidget *create_flora_text_widget(FloraApplicationState *state, FloraWindow *window, FloraWidget *parent,
									  const int is_visible, const char *text, const char *font_name,
									  FloraWidgetStyle style);

/**
 * Add a child widget to a parent widget.
 * @param widget The parent widget
 * @param child The child widget
 * @return FLORA_TRUE on success, FLORA_FALSE otherwise
 */
int add_flora_child_widget(FloraWidget *widget, FloraWidget *child);

/* Widget style setters */
void set_flora_widget_style(FloraWidget *widget, FloraWidgetStyle style);
void set_flora_widget_position(FloraWidget *widget, float x, float y);
void set_flora_widget_width(FloraWidget *widget, FloraSizingType type, float value);
void set_flora_widget_height(FloraWidget *widget, FloraSizingType type, float value);
void set_flora_widget_inner_colour(FloraWidget *widget, FloraColour colour);
void set_flora_widget_border_colour(FloraWidget *widget, FloraColour colour);
void set_flora_widget_text_colour(FloraWidget *widget, FloraColour colour);
void set_flora_widget_font_size(FloraWidget *widget, int size);
void set_flora_widget_padding(FloraWidget *widget, float left, float right, float top, float bottom);
void set_flora_widget_gap(FloraWidget *widget, float x, float y);
void set_flora_widget_layout_direction(FloraWidget *widget, FloraLayoutDirection direction);
void set_flora_widget_visible(FloraWidget *widget, int is_visible);

/* Widget callback setters */
void set_flora_widget_update(FloraWidget *widget, widget_callback update);
void set_flora_widget_on_mouse_down(FloraWidget *widget, widget_callback on_mouse_down);
void set_flora_widget_on_destroy(FloraWidget *widget, widget_callback on_destroy);

/* Base widget callbacks, referenceable by name */
void base_box_widget_on_mouse_down(FloraWidget *widget, FloraApplicationState *state);

/* FLORA COLOUR DEFINITIONS */

#define FLORA_BLACK ((FloraColour){.r = 0, .g = 0, .b = 0, .a = 255})
#define FLORA_WHITE ((FloraColour){.r = 255, .g = 255, .b = 255, .a = 255})

// --- Slate ---
#define FLORA_SLATE_50 ((FloraColour){.r = 248, .g = 250, .b = 252, .a = 255})
#define FLORA_SLATE_100 ((FloraColour){.r = 241, .g = 245, .b = 249, .a = 255})
#define FLORA_SLATE_200 ((FloraColour){.r = 226, .g = 232, .b = 240, .a = 255})
#define FLORA_SLATE_300 ((FloraColour){.r = 203, .g = 213, .b = 225, .a = 255})
#define FLORA_SLATE_400 ((FloraColour){.r = 148, .g = 163, .b = 184, .a = 255})
#define FLORA_SLATE_500 ((FloraColour){.r = 100, .g = 116, .b = 139, .a = 255})
#define FLORA_SLATE_600 ((FloraColour){.r = 71, .g = 85, .b = 105, .a = 255})
#define FLORA_SLATE_700 ((FloraColour){.r = 51, .g = 65, .b = 85, .a = 255})
#define FLORA_SLATE_800 ((FloraColour){.r = 30, .g = 41, .b = 59, .a = 255})
#define FLORA_SLATE_900 ((FloraColour){.r = 15, .g = 23, .b = 42, .a = 255})
#define FLORA_SLATE_950 ((FloraColour){.r = 2, .g = 6, .b = 23, .a = 255})

// --- Gray ---
#define FLORA_GRAY_50 ((FloraColour){.r = 249, .g = 250, .b = 251, .a = 255})
#define FLORA_GRAY_100 ((FloraColour){.r = 243, .g = 244, .b = 246, .a = 255})
#define FLORA_GRAY_200 ((FloraColour){.r = 229, .g = 231, .b = 235, .a = 255})
#define FLORA_GRAY_300 ((FloraColour){.r = 209, .g = 213, .b = 219, .a = 255})
#define FLORA_GRAY_400 ((FloraColour){.r = 156, .g = 163, .b = 175, .a = 255})
#define FLORA_GRAY_500 ((FloraColour){.r = 107, .g = 114, .b = 128, .a = 255})
#define FLORA_GRAY_600 ((FloraColour){.r = 75, .g = 85, .b = 99, .a = 255})
#define FLORA_GRAY_700 ((FloraColour){.r = 55, .g = 65, .b = 81, .a = 255})
#define FLORA_GRAY_800 ((FloraColour){.r = 31, .g = 41, .b = 55, .a = 255})
#define FLORA_GRAY_900 ((FloraColour){.r = 17, .g = 24, .b = 39, .a = 255})
#define FLORA_GRAY_950 ((FloraColour){.r = 3, .g = 7, .b = 18, .a = 255})

// --- Zinc ---
#define FLORA_ZINC_50 ((FloraColour){.r = 250, .g = 250, .b = 250, .a = 255})
#define FLORA_ZINC_100 ((FloraColour){.r = 244, .g = 244, .b = 245, .a = 255})
#define FLORA_ZINC_200 ((FloraColour){.r = 228, .g = 228, .b = 231, .a = 255})
#define FLORA_ZINC_300 ((FloraColour){.r = 212, .g = 212, .b = 216, .a = 255})
#define FLORA_ZINC_400 ((FloraColour){.r = 161, .g = 161, .b = 170, .a = 255})
#define FLORA_ZINC_500 ((FloraColour){.r = 113, .g = 113, .b = 122, .a = 255})
#define FLORA_ZINC_600 ((FloraColour){.r = 82, .g = 82, .b = 91, .a = 255})
#define FLORA_ZINC_700 ((FloraColour){.r = 63, .g = 63, .b = 70, .a = 255})
#define FLORA_ZINC_800 ((FloraColour){.r = 39, .g = 39, .b = 42, .a = 255})
#define FLORA_ZINC_900 ((FloraColour){.r = 24, .g = 24, .b = 27, .a = 255})
#define FLORA_ZINC_950 ((FloraColour){.r = 9, .g = 9, .b = 11, .a = 255})

// --- Neutral ---
#define FLORA_NEUTRAL_50 ((FloraColour){.r = 250, .g = 250, .b = 250, .a = 255})
#define FLORA_NEUTRAL_100 ((FloraColour){.r = 245, .g = 245, .b = 245, .a = 255})
#define FLORA_NEUTRAL_200 ((FloraColour){.r = 229, .g = 229, .b = 229, .a = 255})
#define FLORA_NEUTRAL_300 ((FloraColour){.r = 212, .g = 212, .b = 212, .a = 255})
#define FLORA_NEUTRAL_400 ((FloraColour){.r = 163, .g = 163, .b = 163, .a = 255})
#define FLORA_NEUTRAL_500 ((FloraColour){.r = 115, .g = 115, .b = 115, .a = 255})
#define FLORA_NEUTRAL_600 ((FloraColour){.r = 82, .g = 82, .b = 82, .a = 255})
#define FLORA_NEUTRAL_700 ((FloraColour){.r = 64, .g = 64, .b = 64, .a = 255})
#define FLORA_NEUTRAL_800 ((FloraColour){.r = 38, .g = 38, .b = 38, .a = 255})
#define FLORA_NEUTRAL_900 ((FloraColour){.r = 23, .g = 23, .b = 23, .a = 255})
#define FLORA_NEUTRAL_950 ((FloraColour){.r = 10, .g = 10, .b = 10, .a = 255})

// --- Stone ---
#define FLORA_STONE_50 ((FloraColour){.r = 250, .g = 250, .b = 249, .a = 255})
#define FLORA_STONE_100 ((FloraColour){.r = 245, .g = 245, .b = 244, .a = 255})
#define FLORA_STONE_200 ((FloraColour){.r = 231, .g = 229, .b = 228, .a = 255})
#define FLORA_STONE_300 ((FloraColour){.r = 214, .g = 211, .b = 209, .a = 255})
#define FLORA_STONE_400 ((FloraColour){.r = 168, .g = 162, .b = 158, .a = 255})
#define FLORA_STONE_500 ((FloraColour){.r = 120, .g = 113, .b = 108, .a = 255})
#define FLORA_STONE_600 ((FloraColour){.r = 87, .g = 83, .b = 78, .a = 255})
#define FLORA_STONE_700 ((FloraColour){.r = 68, .g = 64, .b = 60, .a = 255})
#define FLORA_STONE_800 ((FloraColour){.r = 41, .g = 37, .b = 36, .a = 255})
#define FLORA_STONE_900 ((FloraColour){.r = 28, .g = 25, .b = 23, .a = 255})
#define FLORA_STONE_950 ((FloraColour){.r = 12, .g = 10, .b = 9, .a = 255})

// --- Red ---
#define FLORA_RED_50 ((FloraColour){.r = 254, .g = 242, .b = 242, .a = 255})
#define FLORA_RED_100 ((FloraColour){.r = 254, .g = 226, .b = 226, .a = 255})
#define FLORA_RED_200 ((FloraColour){.r = 254, .g = 202, .b = 202, .a = 255})
#define FLORA_RED_300 ((FloraColour){.r = 252, .g = 165, .b = 165, .a = 255})
#define FLORA_RED_400 ((FloraColour){.r = 248, .g = 113, .b = 113, .a = 255})
#define FLORA_RED_500 ((FloraColour){.r = 239, .g = 68, .b = 68, .a = 255})
#define FLORA_RED_600 ((FloraColour){.r = 220, .g = 38, .b = 38, .a = 255})
#define FLORA_RED_700 ((FloraColour){.r = 185, .g = 28, .b = 28, .a = 255})
#define FLORA_RED_800 ((FloraColour){.r = 153, .g = 27, .b = 27, .a = 255})
#define FLORA_RED_900 ((FloraColour){.r = 127, .g = 29, .b = 29, .a = 255})
#define FLORA_RED_950 ((FloraColour){.r = 69, .g = 10, .b = 10, .a = 255})

// --- Orange ---
#define FLORA_ORANGE_50 ((FloraColour){.r = 255, .g = 247, .b = 237, .a = 255})
#define FLORA_ORANGE_100 ((FloraColour){.r = 255, .g = 237, .b = 213, .a = 255})
#define FLORA_ORANGE_200 ((FloraColour){.r = 254, .g = 215, .b = 171, .a = 255})
#define FLORA_ORANGE_300 ((FloraColour){.r = 253, .g = 186, .b = 116, .a = 255})
#define FLORA_ORANGE_400 ((FloraColour){.r = 251, .g = 146, .b = 60, .a = 255})
#define FLORA_ORANGE_500 ((FloraColour){.r = 249, .g = 115, .b = 22, .a = 255})
#define FLORA_ORANGE_600 ((FloraColour){.r = 234, .g = 88, .b = 12, .a = 255})
#define FLORA_ORANGE_700 ((FloraColour){.r = 194, .g = 65, .b = 12, .a = 255})
#define FLORA_ORANGE_800 ((FloraColour){.r = 154, .g = 52, .b = 18, .a = 255})
#define FLORA_ORANGE_900 ((FloraColour){.r = 124, .g = 45, .b = 18, .a = 255})
#define FLORA_ORANGE_950 ((FloraColour){.r = 66, .g = 21, .b = 5, .a = 255})

// --- Amber ---
#define FLORA_AMBER_50 ((FloraColour){.r = 255, .g = 251, .b = 235, .a = 255})
#define FLORA_AMBER_100 ((FloraColour){.r = 254, .g = 243, .b = 199, .a = 255})
#define FLORA_AMBER_200 ((FloraColour){.r = 253, .g = 230, .b = 138, .a = 255})
#define FLORA_AMBER_300 ((FloraColour){.r = 252, .g = 211, .b = 77, .a = 255})
#define FLORA_AMBER_400 ((FloraColour){.r = 251, .g = 191, .b = 36, .a = 255})
#define FLORA_AMBER_500 ((FloraColour){.r = 245, .g = 158, .b = 11, .a = 255})
#define FLORA_AMBER_600 ((FloraColour){.r = 217, .g = 119, .b = 6, .a = 255})
#define FLORA_AMBER_700 ((FloraColour){.r = 180, .g = 83, .b = 9, .a = 255})
#define FLORA_AMBER_800 ((FloraColour){.r = 146, .g = 64, .b = 14, .a = 255})
#define FLORA_AMBER_900 ((FloraColour){.r = 120, .g = 53, .b = 15, .a = 255})
#define FLORA_AMBER_950 ((FloraColour){.r = 65, .g = 28, .b = 5, .a = 255})

// --- Yellow ---
#define FLORA_YELLOW_50 ((FloraColour){.r = 254, .g = 252, .b = 232, .a = 255})
#define FLORA_YELLOW_100 ((FloraColour){.r = 254, .g = 249, .b = 195, .a = 255})
#define FLORA_YELLOW_200 ((FloraColour){.r = 254, .g = 240, .b = 138, .a = 255})
#define FLORA_YELLOW_300 ((FloraColour){.r = 253, .g = 224, .b = 71, .a = 255})
#define FLORA_YELLOW_400 ((FloraColour){.r = 250, .g = 204, .b = 21, .a = 255})
#define FLORA_YELLOW_500 ((FloraColour){.r = 234, .g = 179, .b = 8, .a = 255})
#define FLORA_YELLOW_600 ((FloraColour){.r = 202, .g = 138, .b = 4, .a = 255})
#define FLORA_YELLOW_700 ((FloraColour){.r = 161, .g = 98, .b = 7, .a = 255})
#define FLORA_YELLOW_800 ((FloraColour){.r = 133, .g = 77, .b = 14, .a = 255})
#define FLORA_YELLOW_900 ((FloraColour){.r = 113, .g = 63, .b = 18, .a = 255})
#define FLORA_YELLOW_950 ((FloraColour){.r = 66, .g = 32, .b = 6, .a = 255})

// --- Lime ---
#define FLORA_LIME_50 ((FloraColour){.r = 247, .g = 254, .b = 231, .a = 255})
#define FLORA_LIME_100 ((FloraColour){.r = 236, .g = 252, .b = 203, .a = 255})
#define FLORA_LIME_200 ((FloraColour){.r = 217, .g = 249, .b = 157, .a = 255})
#define FLORA_LIME_300 ((FloraColour){.r = 190, .g = 242, .b = 100, .a = 255})
#define FLORA_LIME_400 ((FloraColour){.r = 163, .g = 230, .b = 53, .a = 255})
#define FLORA_LIME_500 ((FloraColour){.r = 132, .g = 204, .b = 22, .a = 255})
#define FLORA_LIME_600 ((FloraColour){.r = 101, .g = 163, .b = 13, .a = 255})
#define FLORA_LIME_700 ((FloraColour){.r = 77, .g = 124, .b = 15, .a = 255})
#define FLORA_LIME_800 ((FloraColour){.r = 63, .g = 98, .b = 18, .a = 255})
#define FLORA_LIME_900 ((FloraColour){.r = 54, .g = 83, .b = 20, .a = 255})
#define FLORA_LIME_950 ((FloraColour){.r = 25, .g = 39, .b = 7, .a = 255})

// --- Green ---
#define FLORA_GREEN_50 ((FloraColour){.r = 240, .g = 253, .b = 244, .a = 255})
#define FLORA_GREEN_100 ((FloraColour){.r = 220, .g = 252, .b = 231, .a = 255})
#define FLORA_GREEN_200 ((FloraColour){.r = 187, .g = 247, .b = 208, .a = 255})
#define FLORA_GREEN_300 ((FloraColour){.r = 134, .g = 239, .b = 172, .a = 255})
#define FLORA_GREEN_400 ((FloraColour){.r = 74, .g = 222, .b = 128, .a = 255})
#define FLORA_GREEN_500 ((FloraColour){.r = 34, .g = 197, .b = 94, .a = 255})
#define FLORA_GREEN_600 ((FloraColour){.r = 22, .g = 163, .b = 74, .a = 255})
#define FLORA_GREEN_700 ((FloraColour){.r = 21, .g = 128, .b = 61, .a = 255})
#define FLORA_GREEN_800 ((FloraColour){.r = 22, .g = 101, .b = 52, .a = 255})
#define FLORA_GREEN_900 ((FloraColour){.r = 20, .g = 83, .b = 45, .a = 255})
#define FLORA_GREEN_950 ((FloraColour){.r = 5, .g = 46, .b = 22, .a = 255})

// --- Emerald ---
#define FLORA_EMERALD_50 ((FloraColour){.r = 236, .g = 253, .b = 245, .a = 255})
#define FLORA_EMERALD_100 ((FloraColour){.r = 209, .g = 250, .b = 229, .a = 255})
#define FLORA_EMERALD_200 ((FloraColour){.r = 167, .g = 243, .b = 208, .a = 255})
#define FLORA_EMERALD_300 ((FloraColour){.r = 110, .g = 231, .b = 183, .a = 255})
#define FLORA_EMERALD_400 ((FloraColour){.r = 52, .g = 211, .b = 153, .a = 255})
#define FLORA_EMERALD_500 ((FloraColour){.r = 16, .g = 185, .b = 129, .a = 255})
#define FLORA_EMERALD_600 ((FloraColour){.r = 5, .g = 150, .b = 105, .a = 255})
#define FLORA_EMERALD_700 ((FloraColour){.r = 4, .g = 120, .b = 87, .a = 255})
#define FLORA_EMERALD_800 ((FloraColour){.r = 6, .g = 95, .b = 70, .a = 255})
#define FLORA_EMERALD_900 ((FloraColour){.r = 6, .g = 78, .b = 59, .a = 255})
#define FLORA_EMERALD_950 ((FloraColour){.r = 2, .g = 44, .b = 34, .a = 255})

// --- Teal ---
#define FLORA_TEAL_50 ((FloraColour){.r = 240, .g = 253, .b = 250, .a = 255})
#define FLORA_TEAL_100 ((FloraColour){.r = 204, .g = 251, .b = 241, .a = 255})
#define FLORA_TEAL_200 ((FloraColour){.r = 153, .g = 246, .b = 228, .a = 255})
#define FLORA_TEAL_300 ((FloraColour){.r = 94, .g = 234, .b = 212, .a = 255})
#define FLORA_TEAL_400 ((FloraColour){.r = 45, .g = 212, .b = 191, .a = 255})
#define FLORA_TEAL_500 ((FloraColour){.r = 20, .g = 184, .b = 166, .a = 255})
#define FLORA_TEAL_600 ((FloraColour){.r = 13, .g = 148, .b = 136, .a = 255})
#define FLORA_TEAL_700 ((FloraColour){.r = 15, .g = 118, .b = 110, .a = 255})
#define FLORA_TEAL_800 ((FloraColour){.r = 17, .g = 94, .b = 89, .a = 255})
#define FLORA_TEAL_900 ((FloraColour){.r = 19, .g = 78, .b = 74, .a = 255})
#define FLORA_TEAL_950 ((FloraColour){.r = 4, .g = 47, .b = 46, .a = 255})

// --- Cyan ---
#define FLORA_CYAN_50 ((FloraColour){.r = 236, .g = 254, .b = 255, .a = 255})
#define FLORA_CYAN_100 ((FloraColour){.r = 207, .g = 250, .b = 254, .a = 255})
#define FLORA_CYAN_200 ((FloraColour){.r = 165, .g = 242, .b = 251, .a = 255})
#define FLORA_CYAN_300 ((FloraColour){.r = 103, .g = 232, .b = 249, .a = 255})
#define FLORA_CYAN_400 ((FloraColour){.r = 34, .g = 211, .b = 238, .a = 255})
#define FLORA_CYAN_500 ((FloraColour){.r = 6, .g = 182, .b = 212, .a = 255})
#define FLORA_CYAN_600 ((FloraColour){.r = 8, .g = 145, .b = 178, .a = 255})
#define FLORA_CYAN_700 ((FloraColour){.r = 14, .g = 116, .b = 144, .a = 255})
#define FLORA_CYAN_800 ((FloraColour){.r = 21, .g = 94, .b = 117, .a = 255})
#define FLORA_CYAN_900 ((FloraColour){.r = 22, .g = 78, .b = 99, .a = 255})
#define FLORA_CYAN_950 ((FloraColour){.r = 8, .g = 47, .b = 73, .a = 255})

// --- Sky ---
#define FLORA_SKY_50 ((FloraColour){.r = 240, .g = 249, .b = 255, .a = 255})
#define FLORA_SKY_100 ((FloraColour){.r = 224, .g = 242, .b = 254, .a = 255})
#define FLORA_SKY_200 ((FloraColour){.r = 186, .g = 230, .b = 253, .a = 255})
#define FLORA_SKY_300 ((FloraColour){.r = 125, .g = 211, .b = 252, .a = 255})
#define FLORA_SKY_400 ((FloraColour){.r = 59, .g = 180, .b = 255, .a = 255})
#define FLORA_SKY_500 ((FloraColour){.r = 14, .g = 165, .b = 233, .a = 255})
#define FLORA_SKY_600 ((FloraColour){.r = 2, .g = 132, .b = 199, .a = 255})
#define FLORA_SKY_700 ((FloraColour){.r = 3, .g = 105, .b = 161, .a = 255})
#define FLORA_SKY_800 ((FloraColour){.r = 7, .g = 89, .b = 133, .a = 255})
#define FLORA_SKY_900 ((FloraColour){.r = 12, .g = 74, .b = 110, .a = 255})
#define FLORA_SKY_950 ((FloraColour){.r = 8, .g = 47, .b = 73, .a = 255})

// --- Blue ---
#define FLORA_BLUE_50 ((FloraColour){.r = 239, .g = 246, .b = 255, .a = 255})
#define FLORA_BLUE_100 ((FloraColour){.r = 219, .g = 234, .b = 254, .a = 255})
#define FLORA_BLUE_200 ((FloraColour){.r = 191, .g = 219, .b = 254, .a = 255})
#define FLORA_BLUE_300 ((FloraColour){.r = 147, .g = 197, .b = 253, .a = 255})
#define FLORA_BLUE_400 ((FloraColour){.r = 96, .g = 165, .b = 250, .a = 255})
#define FLORA_BLUE_500 ((FloraColour){.r = 59, .g = 130, .b = 246, .a = 255})
#define FLORA_BLUE_600 ((FloraColour){.r = 37, .g = 99, .b = 235, .a = 255})
#define FLORA_BLUE_700 ((FloraColour){.r = 29, .g = 78, .b = 216, .a = 255})
#define FLORA_BLUE_800 ((FloraColour){.r = 30, .g = 64, .b = 175, .a = 255})
#define FLORA_BLUE_900 ((FloraColour){.r = 30, .g = 58, .b = 138, .a = 255})
#define FLORA_BLUE_950 ((FloraColour){.r = 17, .g = 24, .b = 39, .a = 255})

// --- Indigo ---
#define FLORA_INDIGO_50 ((FloraColour){.r = 238, .g = 242, .b = 255, .a = 255})
#define FLORA_INDIGO_100 ((FloraColour){.r = 224, .g = 231, .b = 255, .a = 255})
#define FLORA_INDIGO_200 ((FloraColour){.r = 199, .g = 210, .b = 254, .a = 255})
#define FLORA_INDIGO_300 ((FloraColour){.r = 165, .g = 180, .b = 252, .a = 255})
#define FLORA_INDIGO_400 ((FloraColour){.r = 129, .g = 140, .b = 248, .a = 255})
#define FLORA_INDIGO_500 ((FloraColour){.r = 99, .g = 102, .b = 241, .a = 255})
#define FLORA_INDIGO_600 ((FloraColour){.r = 79, .g = 70, .b = 229, .a = 255})
#define FLORA_INDIGO_700 ((FloraColour){.r = 67, .g = 56, .b = 202, .a = 255})
#define FLORA_INDIGO_800 ((FloraColour){.r = 55, .g = 48, .b = 163, .a = 255})
#define FLORA_INDIGO_900 ((FloraColour){.r = 49, .g = 46, .b = 129, .a = 255})
#define FLORA_INDIGO_950 ((FloraColour){.r = 26, .g = 24, .b = 58, .a = 255})

// --- Violet ---
#define FLORA_VIOLET_50 ((FloraColour){.r = 245, .g = 243, .b = 255, .a = 255})
#define FLORA_VIOLET_100 ((FloraColour){.r = 237, .g = 233, .b = 254, .a = 255})
#define FLORA_VIOLET_200 ((FloraColour){.r = 221, .g = 214, .b = 254, .a = 255})
#define FLORA_VIOLET_300 ((FloraColour){.r = 196, .g = 181, .b = 253, .a = 255})
#define FLORA_VIOLET_400 ((FloraColour){.r = 167, .g = 139, .b = 250, .a = 255})
#define FLORA_VIOLET_500 ((FloraColour){.r = 139, .g = 92, .b = 246, .a = 255})
#define FLORA_VIOLET_600 ((FloraColour){.r = 124, .g = 58, .b = 237, .a = 255})
#define FLORA_VIOLET_700 ((FloraColour){.r = 109, .g = 40, .b = 217, .a = 255})
#define FLORA_VIOLET_800 ((FloraColour){.r = 91, .g = 33, .b = 182, .a = 255})
#define FLORA_VIOLET_900 ((FloraColour){.r = 76, .g = 29, .b = 149, .a = 255})
#define FLORA_VIOLET_950 ((FloraColour){.r = 39, .g = 15, .b = 85, .a = 255})

// --- Purple ---
#define FLORA_PURPLE_50 ((FloraColour){.r = 250, .g = 245, .b = 255, .a = 255})
#define FLORA_PURPLE_100 ((FloraColour){.r = 243, .g = 232, .b = 255, .a = 255})
#define FLORA_PURPLE_200 ((FloraColour){.r = 233, .g = 213, .b = 255, .a = 255})
#define FLORA_PURPLE_300 ((FloraColour){.r = 216, .g = 180, .b = 254, .a = 255})
#define FLORA_PURPLE_400 ((FloraColour){.r = 192, .g = 132, .b = 252, .a = 255})
#define FLORA_PURPLE_500 ((FloraColour){.r = 168, .g = 85, .b = 247, .a = 255})
#define FLORA_PURPLE_600 ((FloraColour){.r = 147, .g = 51, .b = 234, .a = 255})
#define FLORA_PURPLE_700 ((FloraColour){.r = 126, .g = 34, .b = 206, .a = 255})
#define FLORA_PURPLE_800 ((FloraColour){.r = 107, .g = 33, .b = 168, .a = 255})
#define FLORA_PURPLE_900 ((FloraColour){.r = 88, .g = 28, .b = 135, .a = 255})
#define FLORA_PURPLE_950 ((FloraColour){.r = 49, .g = 19, .b = 81, .a = 255})

// --- Fuchsia ---
#define FLORA_FUCHSIA_50 ((FloraColour){.r = 253, .g = 244, .b = 255, .a = 255})
#define FLORA_FUCHSIA_100 ((FloraColour){.r = 250, .g = 232, .b = 255, .a = 255})
#define FLORA_FUCHSIA_200 ((FloraColour){.r = 245, .g = 208, .b = 254, .a = 255})
#define FLORA_FUCHSIA_300 ((FloraColour){.r = 240, .g = 171, .b = 252, .a = 255})
#define FLORA_FUCHSIA_400 ((FloraColour){.r = 232, .g = 121, .b = 249, .a = 255})
#define FLORA_FUCHSIA_500 ((FloraColour){.r = 217, .g = 70, .b = 239, .a = 255})
#define FLORA_FUCHSIA_600 ((FloraColour){.r = 192, .g = 38, .b = 211, .a = 255})
#define FLORA_FUCHSIA_700 ((FloraColour){.r = 162, .g = 28, .b = 175, .a = 255})
#define FLORA_FUCHSIA_800 ((FloraColour){.r = 134, .g = 25, .b = 143, .a = 255})
#define FLORA_FUCHSIA_900 ((FloraColour){.r = 112, .g = 26, .b = 117, .a = 255})
#define FLORA_FUCHSIA_950 ((FloraColour){.r = 66, .g = 15, .b = 70, .a = 255})

// --- Pink ---
#define FLORA_PINK_50 ((FloraColour){.r = 253, .g = 242, .b = 248, .a = 255})
#define FLORA_PINK_100 ((FloraColour){.r = 252, .g = 231, .b = 243, .a = 255})
#define FLORA_PINK_200 ((FloraColour){.r = 251, .g = 207, .b = 232, .a = 255})
#define FLORA_PINK_300 ((FloraColour){.r = 249, .g = 168, .b = 212, .a = 255})
#define FLORA_PINK_400 ((FloraColour){.r = 244, .g = 114, .b = 182, .a = 255})
#define FLORA_PINK_500 ((FloraColour){.r = 236, .g = 72, .b = 153, .a = 255})
#define FLORA_PINK_600 ((FloraColour){.r = 219, .g = 39, .b = 119, .a = 255})
#define FLORA_PINK_700 ((FloraColour){.r = 190, .g = 24, .b = 93, .a = 255})
#define FLORA_PINK_800 ((FloraColour){.r = 157, .g = 23, .b = 77, .a = 255})
#define FLORA_PINK_900 ((FloraColour){.r = 131, .g = 24, .b = 67, .a = 255})
#define FLORA_PINK_950 ((FloraColour){.r = 75, .g = 9, .b = 36, .a = 255})

// --- Rose ---
#define FLORA_ROSE_50 ((FloraColour){.r = 255, .g = 241, .b = 242, .a = 255})
#define FLORA_ROSE_100 ((FloraColour){.r = 255, .g = 228, .b = 230, .a = 255})
#define FLORA_ROSE_200 ((FloraColour){.r = 254, .g = 205, .b = 211, .a = 255})
#define FLORA_ROSE_300 ((FloraColour){.r = 253, .g = 164, .b = 175, .a = 255})
#define FLORA_ROSE_400 ((FloraColour){.r = 251, .g = 113, .b = 133, .a = 255})
#define FLORA_ROSE_500 ((FloraColour){.r = 244, .g = 63, .b = 94, .a = 255})
#define FLORA_ROSE_600 ((FloraColour){.r = 225, .g = 29, .b = 72, .a = 255})
#define FLORA_ROSE_700 ((FloraColour){.r = 190, .g = 18, .b = 60, .a = 255})
#define FLORA_ROSE_800 ((FloraColour){.r = 159, .g = 18, .b = 57, .a = 255})
#define FLORA_ROSE_900 ((FloraColour){.r = 136, .g = 19, .b = 55, .a = 255})
#define FLORA_ROSE_950 ((FloraColour){.r = 76, .g = 5, .b = 25, .a = 255})

#ifdef __cplusplus
}
#endif

#endif // FLORA_H
