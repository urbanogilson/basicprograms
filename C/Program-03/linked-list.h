#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct node
{
    int data;
    struct node *next;
} node;

node *create_node(int data);
node *insert_at_beginning(node *head, int data);
node *insert_at_end(node *head, int data);
node *remove_at_beginning(node *head);
node *remove_at_end(node *head);
void free_linkedlist(node *head);
void print_linkedlist(node *head);

#endif // LINKEDLIST_H