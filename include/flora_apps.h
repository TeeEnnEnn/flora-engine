#ifndef FLORA_APPS_H
#define FLORA_APPS_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdint.h>

#include "flora_constants.h"
#include "flora_events.h"
#include "flora_widgets.h"

typedef struct {
    char key[TABLE_KEY_LENGTH];
    void* element;
} TableEntry;

typedef struct {
    uint64_t count;
    uint64_t capacity;
    TableEntry* entries;
} Table;

typedef struct {
    char* app_name;
    int window_width;
    int window_height;
    int event_queue_capacity;
    int use_fonts; /* should use FLORA_TRUE / FLORA_FALSE */
    int window_table_initial_capacity;
    int font_table_initial_capacity;
} FloraConfig;

typedef void (*on_init_screen)(FloraApplicationState *state, FloraScreen *screen);

typedef void (*on_deinit_screen)(FloraApplicationState *state, FloraScreen *screen);

typedef void (*on_init_window)(FloraApplicationState *state, FloraWindow *window);

typedef void (*on_deinit_window)(FloraApplicationState *state, FloraWindow *window);

struct FloraApplicationState {
    /* Application Details */
    char* app_name;
    int window_width;
    int window_height;

    uint64_t last_frame_time;
    double delta_time;
    bool running;
    EventQueue event_queue;

    Table window_table;
    FloraWindow* current_window;

    Table font_table;
};

struct FloraWindow {
    on_init_window on_init_window;
    on_deinit_window on_deinit_window;
    SDL_Window *window;
    SDL_Renderer *renderer;
    Table screens;
    FloraScreen *current_screen;
    char name[TABLE_KEY_LENGTH];
};

struct FloraScreen {
    on_init_screen on_init_screen;
    on_deinit_screen on_deinit_screen;
    FloraWidget **widgets;
    int widget_count;
    int widget_capacity;
    char name[TABLE_KEY_LENGTH];
};

struct FloraFont {
    TTF_Font *font;
    float font_size;
    char font_path[256];
};


/**
 * Initialize the flora application state.
 * @param state The application state to initialize
 * @param config The configuration for the application (can be NULL for defaults)
 * @return FLORA_TRUE on success, FLORA_FALSE otherwise
 */
int init_flora(FloraApplicationState *state, FloraConfig *config);

/**
 * Deinitialize the flora application state and release all resources.
 * @param state The application state to deinitialize
 */
void deinit_flora(FloraApplicationState *state);

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
 * Create a window owned by the application and register it in the state's window table.
 * @param state The flora application state
 * @param window_title The title of the window (also used as the table key)
 * @return The initialized window, or NULL on failure
 */
FloraWindow *init_flora_window(FloraApplicationState *state, char *window_title);

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
                               on_init_screen on_init_screen, on_deinit_screen on_deinit_screen);

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

#endif //FLORA_APPS_H
