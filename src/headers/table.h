#ifndef FLORA_ENGINE_TABLE_H
#define FLORA_ENGINE_TABLE_H

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


int init_table(Table *table, uint64_t capacity);
void deinit_table(Table *table);

int set_table(Table *table, void *element, const char* key);
int get_table(Table *table, void** element, const char* key);

#endif //FLORA_ENGINE_TABLE_H
