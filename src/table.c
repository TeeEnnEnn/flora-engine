#include "table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FNV_OFFSET_BIAS 14695981039346656037ULL
#define FNV_PRIME 1099511628211

bool init_table(Table *table, const uint64_t capacity) {
    table->capacity = capacity;
    table->count = 0;
    table->entries = calloc(table->capacity, sizeof(TableEntry));
    if (table->entries == NULL) {
        fprintf(stderr, "Error: not enough memory for table->elements\n");
        return false;
    }
    return true;
}

void destroy_table(Table *table) {
    table->count = 0;
    table->capacity = 0;
    free(table->entries);
    table->entries = NULL;
}

/**
 * Hashes a string
 * @param key A null terminated string
 * @return The hashed value
 */
static uint64_t hash_key(const char *key) {
    uint64_t hash = FNV_OFFSET_BIAS;
    const size_t len = strlen(key);
    for (uint64_t i = 0; i < len; i++) {
        hash ^= key[i];
        hash *= FNV_PRIME;
    }
    return hash;
}

static bool resize_table(Table *table, const uint64_t current_capacity) {
    const uint64_t new_capacity = current_capacity * 2;
    TableEntry *new_entries = calloc(new_capacity, sizeof(TableEntry));
    if (new_entries == NULL) {
        fprintf(stderr, "Error: not enough memory for new_entries\n");
        return false;
    }

    for (uint64_t i = 0; i < table->capacity; i++) {
        if (table->entries[i].key[0] != '\0') {
            uint64_t hash = hash_key(table->entries[i].key);
            uint64_t index = hash % new_capacity;

            // Loop until we find an empty slot.
            while (new_entries[index].key[0] != '\0') {
                index = (index + 1) % new_capacity;
            }

            new_entries[index] = table->entries[i];
        }
    }

    free(table->entries);
    table->entries = new_entries;
    table->capacity = new_capacity;
    return true;
}

bool set_table(Table *table, void *element, const char *key) {
    if (key[0] == '\0' || !table) return false;

    if (table->count >= (uint64_t) ((double) table->capacity * TABLE_LOAD_FACTOR)) {
        if (!resize_table(table, table->capacity)) {
            return false;
        }
    }

    uint64_t hash = hash_key(key);
    uint64_t index = hash % table->capacity;

    while (table->entries[index].key[0] != '\0') {
        // already occupied - update
        if (strncmp(table->entries[index].key, key, TABLE_KEY_LENGTH) == 0) {
            table->entries[index].element = element;
            return true;
        }
        // Collision: Move to next slot
        index = (index + 1) % table->capacity;
    }

    // Found an empty slot
    strncpy(table->entries[index].key, key, TABLE_KEY_LENGTH - 1);
    table->entries[index].key[TABLE_KEY_LENGTH - 1] = '\0';
    table->entries[index].element = element;

    table->count++;
    return true;
}

bool get_table(Table *table, void **element, const char *key) {
    if (!table || key[0] == '\0') return false;

    const uint64_t hash = hash_key(key);
    uint64_t index = hash % table->capacity;
    const uint64_t start_index = index;

    // Probe
    while (table->entries[index].key[0] != '\0') {
        if (strncmp(table->entries[index].key, key, TABLE_KEY_LENGTH) == 0) {
            *element = table->entries[index].element;
            return true;
        }

        index = (index + 1) % table->capacity;

        // should be unreachable
        if (index == start_index) break;
    }

    return false;
}
