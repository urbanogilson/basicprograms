#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdbool.h>

typedef struct node
{
    int data;
    struct node *next;
} node;

node *list_create_node(int data);
void list_free(node *head);
void list_traverse(node *head);
int list_length(node *head);
node *list_insert_at_beginning(node **head, int data);
node *list_remove_at_beginning(node **head);
node *list_insert_at_end(node **head, int data);
node *list_remove_at_end(node **head);
node *list_search(node *head, int data);
node *list_sort(node **head);
node *list_reverse(node **head);
node *list_reverse_recursive(node **head);
bool list_has_cycle(node *head);
bool list_is_palindrome(node *head);
int list_get_at_index(node *head, int index);
node *list_insert_at_index(node **head, int index, int data);
node *list_remove_at_index(node **head, int index);
node *list_sorted_merge(node *a, node *b);
void list_split(struct node *source, struct node **init, struct node **end);
void list_merge_sort(node **head);
#endif // LINKEDLIST_H