#include "linked-list.h"
#include <stdio.h>
#include <stdlib.h>

node *list_create_node(int data)
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

void list_free(node *head)
{
    while (NULL != head)
    {
        node *n = head;
        head = head->next;
        free(n);
    }
}

void list_traverse(node *head)
{
    for (node *n = head; NULL != n; n = n->next)
    {
        printf("%d -> ", n->data);
    }

    printf("NULL\n");
}

node *list_insert_at_beginning(node **head, int data)
{
    node *new_node = list_create_node(data);

    if (NULL == new_node)
    {
        return *head;
    }

    new_node->next = *head;
    *head = new_node;

    return *head;
}

node *list_insert_at_end(node **head, int data)
{
    node *new_node = list_create_node(data);

    if (NULL == new_node)
    {
        return *head;
    }

    if (*head == NULL)
    {
        *head = new_node;
        return *head;
    }

    node *temp = *head;

    while (NULL != temp->next)
    {
        temp = temp->next;
    }

    temp->next = new_node;

    return *head;
}

node *list_remove_at_beginning(node **head)
{
    if (*head == NULL)
    {
        return NULL;
    }

    node *temp = *head;
    *head = temp->next;
    free(temp);

    return *head;
}

node *list_remove_at_end(node **head)
{
    if (*head == NULL)
    {
        return NULL;
    }

    node *temp = *head, *new_end = temp;

    if (NULL == temp->next)
    {
        free(temp);
        *head = NULL;
        return *head;
    }

    while (NULL != temp->next)
    {
        new_end = temp;
        temp = temp->next;
    }

    free(temp);

    new_end->next = NULL;

    return *head;
}

node *list_sort(node **head)
{
    if (NULL == *head)
    {
        return NULL;
    }

    for (node *i = *head; NULL != i; i = i->next)
    {
        for (node *j = *head; NULL != j; j = j->next)
        {
            if (i->data < j->data)
            {
                int temp = i->data;
                i->data = j->data;
                j->data = temp;
            }
        }
    }

    return *head;
}

node *list_reverse(node **head)
{
    if (NULL == *head)
    {
        return NULL;
    }

    node *previous = NULL, *current = *head, *next = NULL;

    while (NULL != current)
    {
        next = current->next;
        current->next = previous;
        previous = current;
        current = next;
    }

    *head = previous;

    return *head;
}

int list_geti(node *head, int index)
{
    if (NULL == head || index < 0)
    {
        return -1;
    }

    node *temp = head;
    int i = 0;

    while (NULL != temp)
    {
        if (i == index)
        {
            return temp->data;
        }

        i++;
        temp = temp->next;
    }

    return -1;
}