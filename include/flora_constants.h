#ifndef FLORA_CONSTANTS_H
#define FLORA_CONSTANTS_H

// #define EVENT_LOGS

#define INITIAL_WINDOW_WIDTH 640
#define INITIAL_WINDOW_HEIGHT 480
#define WINDOW_TITLE "Flora Engine"
#define FLORA_ENGINE_FATAL 1
#define FLORA_ENGINE_SUCCESS 0
#define INITIAL_EVENT_QUEUE_CAPACITY 8
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
// TODO: Change this and all occurrences
#define OPEN_SANS_FONT_PATH "D:\\flora-engine\\assets\\fonts\\Open_Sans\\OpenSans-VariableFont_wdth,wght.ttf"
#define TABLE_KEY_LENGTH 16
#define TABLE_LOAD_FACTOR 0.65

#endif // !FLORA_CONSTANTS_H
