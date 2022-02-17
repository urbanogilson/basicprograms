/*
Write a hash table in C

Inspired by: https://github.com/jamesroutley/write-a-hash-table
*/

#include <stdio.h>

#include "hash_table.h"

int main(void) {
  ht_hash_table* hash_table = ht_new();
  ht_insert(hash_table, "Gilson", "Urbano");
  ht_insert(hash_table, "Brazil", "Denmark");
  ht_insert(hash_table, "Age", "26");

  printf("Gilson %s\n", ht_search(hash_table, "Gilson") ?: "Not found");
  printf("Brazil -> %s\n", ht_search(hash_table, "Brazil" ?: "Not found"));
  printf("Age: %s\n", ht_search(hash_table, "Age" ?: "Not found"));
  printf("Address: %s\n", ht_search(hash_table, "Address") ?: "Not found");

  ht_del_hash_table(hash_table);
}