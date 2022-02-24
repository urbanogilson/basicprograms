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

  char* value = ht_search(hash_table, "Gilson");
  printf("Gilson %s\n", value ? value : "Not found");

  value = ht_search(hash_table, "Brazil");
  printf("Brazil -> %s\n", value ? value : "Not found");

  value = ht_search(hash_table, "Age");
  printf("Age: %s\n", value ? value : "Not found");

  value = ht_search(hash_table, "Address");
  printf("Street: %s\n", value ? value : "Not found");

  ht_del_hash_table(hash_table);
}