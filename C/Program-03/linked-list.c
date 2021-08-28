#include "linked-list.h"
#include <stdio.h>
#include <stdlib.h>

node *create_node(int data)
{
    node *new_node = (node *)malloc(sizeof(node));

    if (NULL == new_node)
    {
        return NULL;
    }

    new_node->data = data;
    new_node->next = NULL;

    return new_node;
}

void free_linkedlist(node *head)
{
    while (NULL != head)
    {
        node *n = head;
        head = head->next;
        free(n);
    }
}

void print_linkedlist(node *head)
{
    for (node *n = head; NULL != n; n = n->next)
    {
        printf("%d -> ", n->data);
    }

    printf("NULL\n");
}

node *insert_at_beginning(node *head, int data)
{
    node *new_node = create_node(data);

    if (NULL == new_node)
    {
        return head;
    }

    new_node->next = head;

    head = new_node;

    return head;
}

node *insert_at_end(node *head, int data)
{
    node *new_node = create_node(data);

    if (NULL == new_node)
    {
        return head;
    }

    if (head == NULL)
    {
        head = new_node;
        return head;
    }

    node *temp = head;

    while (NULL != temp->next)
    {
        temp = temp->next;
    }

    temp->next = new_node;

    return head;
}

node *remove_at_beginning(node *head)
{
    if (head == NULL)
    {
        return NULL;
    }

    node *temp = head;
    head = temp->next;
    free(temp);

    return head;
}

node *remove_at_end(node *head)
{
    if (head == NULL)
    {
        return NULL;
    }

    node *temp = head, *new_end;

    if (NULL == temp->next)
    {
        free(temp);
        return NULL;
    }

    while (NULL != temp->next)
    {
        new_end = temp;
        temp = temp->next;
    }

    free(temp);

    new_end->next = NULL;

    return head;
}