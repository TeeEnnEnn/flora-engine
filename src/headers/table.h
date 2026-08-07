#ifndef FLORA_ENGINE_TABLE_H
#define FLORA_ENGINE_TABLE_H

#include <stdbool.h>
#include <stdint.h>

#include "flora_apps.h"

bool init_table(Table *table, uint64_t capacity);
void deinit_table(Table *table);

bool set_table(Table *table, void *element, const char* key);
bool get_table(Table *table, void** element, const char* key);

#endif //FLORA_ENGINE_TABLE_H
