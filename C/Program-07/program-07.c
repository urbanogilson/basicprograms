/*
Write a hash table in C

Inspired by: https://github.com/jamesroutley/write-a-hash-table
*/

#include "hash_table.h"

int main(void) {
  ht_hash_table* hash_table = ht_new();
  ht_del_hash_table(hash_table);
}