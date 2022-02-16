#include "hash_table.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static ht_item* ht_new_item(const char* key, const char* value) {
  ht_item* item = (ht_item*)malloc(sizeof(ht_item));

  if (item == NULL) {
    return NULL;
  }

  item->key = strdup(key);
  item->value = strdup(value);

  if (item->key == NULL) {  // ! free(item)
    return NULL;
  }

  if (item->value == NULL) {  // ! free(item) and free key
    return NULL;
  }

  return item;
}

static void ht_del_item(ht_item* item) {
  free(item->key);
  free(item->value);
  free(item);
}

ht_hash_table* ht_new() {
  ht_hash_table* hash_table = (ht_hash_table*)malloc(sizeof(ht_hash_table));

  if (hash_table == NULL) {
    return NULL;
  }

  hash_table->size = 53;
  hash_table->items = 0;
  hash_table->items = (ht_item**)calloc(hash_table->size, sizeof(ht_item*));

  if (hash_table->items == NULL) {  // ! free(hash_table)
    return NULL;
  }

  return hash_table;
}

void ht_del_hash_table(ht_hash_table* hash_table) {
  for (size_t i = 0; i < hash_table->size; i++) {
    ht_item* item = hash_table->items[i];
    if (item != NULL) {
      ht_del_item(item);
    }
  }
  free(hash_table->items);
  free(hash_table);
}