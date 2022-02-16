#include <stddef.h>

typedef struct {
  char* key;
  char* value;
} ht_item;

typedef struct {
  size_t size;
  size_t count;
  ht_item** items;
} ht_hash_table;

static ht_item* ht_new_item(const char* k, const char* v);
static void ht_del_item(ht_item* item);
ht_hash_table* ht_new();
void ht_del_hash_table(ht_hash_table* hash_table);