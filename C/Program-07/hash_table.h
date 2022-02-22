#include <stddef.h>

#define HT_PRIME_1 157
#define HT_PRIME_2 113
#define HT_INITIAL_BASE_SIZE 50

typedef struct {
  char* key;
  char* value;
} ht_item;

typedef struct {
  size_t base_size;
  size_t size;
  size_t count;
  ht_item** items;
} ht_hash_table;

ht_hash_table* ht_new();
void ht_del_hash_table(ht_hash_table* hash_table);
void ht_insert(ht_hash_table* hash_table, const char* key, const char* value);
char* ht_search(ht_hash_table* hash_table, const char* key);
void ht_delete(ht_hash_table* hash_table, const char* key);