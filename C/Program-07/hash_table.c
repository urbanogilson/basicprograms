#include "hash_table.h"

#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "prime.h"

static ht_item* ht_new_item(const char* key, const char* value) {
  ht_item* item = (ht_item*)malloc(sizeof(ht_item));

  if (item == NULL) {
    return NULL;
  }

  item->key = strdup(key);
  item->value = strdup(value);

  if (item->key == NULL) {
    return NULL;
  }

  if (item->value == NULL) {
    return NULL;
  }

  return item;
}

static void ht_del_item(ht_item* item) {
  free(item->key);
  free(item->value);
  free(item);
}

ht_hash_table* ht_new() { return ht_new_sized(HT_INITIAL_BASE_SIZE); }

void ht_del_hash_table(ht_hash_table* hash_table) {
  for (size_t i = 0; i < hash_table->size; i++) {
    ht_item* item = hash_table->items[i];
    if (item != NULL && item != &HT_DELETED_ITEM) {
      ht_del_item(item);
    }
  }
  free(hash_table->items);
  free(hash_table);
}

static size_t ht_hash(const char* string, const int prime,
                      const size_t buckets) {
  size_t hash = 0;
  const size_t string_len = strlen(string);
  for (size_t i = 0; i < string_len; i++) {
    hash += (size_t)pow(prime, (string_len - (i + 1))) * string[i];
    hash = hash % prime;
  }
  // hash = hash % prime; // maybe here
  return hash;
}

static size_t ht_get_hash(const char* string, const size_t buckets,
                          const size_t attempt) {
  const size_t hash_a = ht_hash(string, HT_PRIME_1, buckets);
  size_t hash_b = ht_hash(string, HT_PRIME_2, buckets);

  // https://github.com/jamesroutley/write-a-hash-table/issues/35
  if (hash_b % buckets == 0) {
    hash_b = 1;
  }

  return (hash_a + (attempt * (hash_b + 1))) % buckets;
}

void ht_insert(ht_hash_table* hash_table, const char* key, const char* value) {
  const size_t load = hash_table->count * 100 / hash_table->size;
  if (load > 70) {
    ht_resize_up(hash_table);
  }

  ht_item* item = ht_new_item(key, value);

  if (item == NULL) {
    return;
  }

  size_t index = ht_get_hash(item->key, hash_table->size, 0);
  ht_item* current_item = hash_table->items[index];

  for (size_t i = 1; current_item != NULL && current_item != &HT_DELETED_ITEM;
       i++) {
    if (strcmp(current_item->key, key) == 0) {
      ht_del_item(current_item);
      hash_table->items[index] = item;
      return;
    }
    index = ht_get_hash(item->key, hash_table->size, i);
    current_item = hash_table->items[index];
  }

  hash_table->items[index] = item;
  hash_table->count++;
}

char* ht_search(ht_hash_table* hash_table, const char* key) {
  size_t index = ht_get_hash(key, hash_table->size, 0);
  ht_item* item = hash_table->items[index];

  for (size_t i = 1; item != NULL; i++) {
    if (item != &HT_DELETED_ITEM) {
      if (strcmp(item->key, key) == 0) {
        return item->value;
      }
    }
    index = ht_get_hash(key, hash_table->size, i);
    item = hash_table->items[index];
  }

  return NULL;
}

void ht_delete(ht_hash_table* hash_table, const char* key) {
  const size_t load = hash_table->count * 100 / hash_table->size;
  if (load < 10) {
    ht_resize_down(hash_table);
  }

  size_t index = ht_get_hash(key, hash_table->size, 0);
  ht_item* item = hash_table->items[index];

  for (size_t i = 1; item != NULL; i++) {
    if (item != &HT_DELETED_ITEM) {
      if (strcmp(item->key, key) == 0) {
        ht_del_item(item);
        hash_table->items[index] = &HT_DELETED_ITEM;
        hash_table->count--;
        return;
      }
    }
    index = ht_get_hash(key, hash_table->size, i);
    item = hash_table->items[index];
  }
}

static ht_hash_table* ht_new_sized(const size_t base_size) {
  ht_hash_table* hash_table = (ht_hash_table*)malloc(sizeof(ht_hash_table));

  if (hash_table == NULL) {
    return NULL;
  }

  hash_table->base_size = base_size;
  hash_table->size = next_prime(base_size);
  hash_table->items = 0;
  hash_table->items = (ht_item**)calloc(hash_table->size, sizeof(ht_item*));

  if (hash_table->items == NULL) {
    return NULL;
  }

  return hash_table;
}

static void ht_resize(ht_hash_table* hash_table, const size_t base_size) {
  if (base_size < HT_INITIAL_BASE_SIZE) {
    return;
  }

  ht_hash_table* new_hash_table = ht_new_sized(base_size);

  for (int i = 0; i < hash_table->size; i++) {
    ht_item* item = hash_table->items[i];
    if (item != NULL && item != &HT_DELETED_ITEM) {
      ht_insert(new_hash_table, item->key, item->value);
    }
  }

  hash_table->base_size = new_hash_table->base_size;
  hash_table->count = new_hash_table->count;

  const size_t tmp_size = hash_table->size;
  hash_table->size = new_hash_table->size;
  new_hash_table->size = tmp_size;

  ht_item** tmp_items = hash_table->items;
  hash_table->items = new_hash_table->items;
  new_hash_table->items = tmp_items;

  ht_del_hash_table(new_hash_table);
}

static void ht_resize_up(ht_hash_table* hash_table) {
  const size_t new_size = hash_table->base_size * 2;
  ht_resize(hash_table, new_size);
}

static void ht_resize_down(ht_hash_table* hash_table) {
  const size_t new_size = hash_table->base_size / 2;
  ht_resize(hash_table, new_size);
}