#ifndef FLORA_CONSTANTS_H
#define FLORA_CONSTANTS_H

#define DEFAULT_WINDOW_WIDTH 640
#define DEFAULT_WINDOW_HEIGHT 480
#define DEFAULT_APP_NAME "Flora App"
#define DEFAULT_USE_FONTS FLORA_TRUE
#define DEFAULT_SCREEN_TABLE_CAPACITY 4

#define INITIAL_WIDGET_CAPACITY 8
#define GROWTH_FACTOR 2
#define INITIAL_CHILD_WIDGET_CAPACITY 4
#define INITIAL_FONT_CAPACITY 4
#define INITIAL_TABLE_CAPACITY 4
#define INITIAL_SCREEN_NAME "base"
#define BASE_TEXT_SIZE 24
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define RAND(a, b) ((a) + rand() % ((b) - (a) + 1))
#define TABLE_KEY_LENGTH 128
#define TABLE_LOAD_FACTOR 0.65

#endif // !FLORA_CONSTANTS_H
