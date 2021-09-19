#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct node
{
    int data;
    struct node *next;
} node;

node *list_create_node(int data);
void list_free(node *head);
void list_traverse(node *head);
size_t list_length(node *head);
node *list_insert_at_beginning(node **head, int data);
node *list_remove_at_beginning(node **head);
node *list_insert_at_end(node **head, int data);
node *list_remove_at_end(node **head);
node *list_search(node *head, int data);
node *list_sort(node **head);
node *list_reverse(node **head);
bool list_has_cycle(node *head);
bool list_is_palindrome(node *head);
int list_get_at_index(node *head, size_t index);
node *list_insert_at_index(node **head, size_t index, int data);
node *list_remove_at_index(node **head, size_t index);
node *list_sorted_merge(node *head_a, node *head_b);
void list_split(node *head, node **init, node **end);
void list_merge_sort(node **head);
#endif // LINKEDLIST_H