#ifndef FLORA_ENGINE_TABLE_H
#define FLORA_ENGINE_TABLE_H

#include <stdbool.h>
#include <stdint.h>

#include "flora_constants.h"

typedef struct {
    char key[TABLE_KEY_LENGTH];
    void* element;
} TableEntry;

typedef struct {
    uint64_t count;
    uint64_t capacity;
    TableEntry* entries;
} Table;

bool init_table(Table *table, uint64_t capacity);
void destroy_table(Table *table);

bool set_table(Table *table, void *element, const char* key);
bool get_table(Table *table, void** element, const char* key);

#endif //FLORA_ENGINE_TABLE_H