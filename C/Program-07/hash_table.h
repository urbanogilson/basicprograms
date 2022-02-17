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

static ht_item HT_DELETED_ITEM = {NULL, NULL};
static ht_item* ht_new_item(const char* k, const char* v);
static void ht_del_item(ht_item* item);
static size_t ht_hash(const char* string, const int prime,
                      const size_t buckets);
static size_t ht_get_hash(const char* string, const size_t buckets,
                          const size_t attempt);

ht_hash_table* ht_new();
void ht_del_hash_table(ht_hash_table* hash_table);
void ht_insert(ht_hash_table* hash_table, const char* key, const char* value);
char* ht_search(ht_hash_table* hash_table, const char* key);
void ht_delete(ht_hash_table* hash_table, const char* key);
static ht_hash_table* ht_new_sized(const size_t base_size);
static void ht_resize(ht_hash_table* hash_table, const size_t base_size);
static void ht_resize_up(ht_hash_table* hash_table);
static void ht_resize_down(ht_hash_table* hash_table);