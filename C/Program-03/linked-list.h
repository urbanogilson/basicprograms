#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdbool.h>

typedef struct node
{
    int data;
    struct node *next;
} node;

node *create_node(int data);
int get(node *head, int index);
node *insert_at_beginning(node **head, int data);
node *insert_at_end(node **head, int data);
node *insert_at_index(node **head, int index, int data);
node *remove_at_index(node **head, int index);
node *remove_at_beginning(node **head);
node *remove_at_end(node **head);
node *sort(node **head);
node *reverse(node **head);
bool has_cycle(node *head);
bool is_palindrome(node *head);
void free_linkedlist(node *head);
void traverse(node *head);

#endif // LINKEDLIST_H