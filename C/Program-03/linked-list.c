#include "linked-list.h"
#include <stdio.h>
#include <stdlib.h>

node *list_create_node(int data)
{
    node *new_node = (node *)malloc(sizeof(node));

    if (new_node == NULL)
    {
        return NULL;
    }

    new_node->data = data;
    new_node->next = NULL;

    return new_node;
}

void list_free(node *head)
{
    node *n = head;

    while (head != NULL)
    {
        n = head;
        head = head->next;
        free(n);
    }
}

void list_traverse(node *head)
{
    for (node *current = head; current != NULL; current = current->next)
    {
        printf("%d -> ", current->data);
    }

    printf("NULL\n\n");
}

node *list_insert_at_beginning(node **head, int data)
{
    node *new_node = list_create_node(data);

    if (new_node == NULL)
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

    if (new_node == NULL)
    {
        return *head;
    }

    if (*head == NULL)
    {
        *head = new_node;
        return *head;
    }

    node *temp = *head;

    while (temp->next != NULL)
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

    node *current = *head;
    *head = current->next;
    free(current);

    return *head;
}

node *list_remove_at_end(node **head)
{
    if (*head == NULL)
    {
        return NULL;
    }

    node *current = *head, *previous = *head;

    if (current->next == NULL)
    {
        free(current);
        *head = NULL;
        return *head;
    }

    while (current->next != NULL)
    {
        previous = current;
        current = current->next;
    }

    free(current);

    previous->next = NULL;

    return *head;
}

node *list_sort(node **head)
{
    if (*head == NULL)
    {
        return NULL;
    }

    int temp = 0;

    for (node *i = *head; i != NULL; i = i->next)
    {
        for (node *j = *head; j != NULL; j = j->next)
        {
            if (i->data < j->data)
            {
                temp = i->data;
                i->data = j->data;
                j->data = temp;
            }
        }
    }

    return *head;
}

node *list_reverse(node **head)
{
    if (*head == NULL)
    {
        return NULL;
    }

    node *previous = NULL, *current = *head, *next = NULL;

    while (current != NULL)
    {
        next = current->next;
        current->next = previous;
        previous = current;
        current = next;
    }

    *head = previous;

    return *head;
}

int list_get_at_index(node *head, size_t index)
{
    if (head == NULL)
    {
        return -1;
    }

    node *current = head;
    size_t i = 0;

    while (current != NULL)
    {
        if (i == index)
        {
            return current->data;
        }

        i++;
        current = current->next;
    }

    return -1;
}

node *list_remove_at_index(node **head, size_t index)
{
    if (*head == NULL)
    {
        return *head;
    }

    if (index == 0)
    {
        return list_remove_at_beginning(head);
    }

    node *current = (*head)->next, *previous = *head;

    size_t i = 1;

    while (NULL != current)
    {
        if (i == index)
        {
            previous->next = current->next;
            free(current);
            return *head;
        }

        i++;
        previous = current;
        current = current->next;
    }

    return *head;
}

node *list_insert_at_index(node **head, size_t index, int data)
{
    if (*head == NULL)
    {
        return *head;
    }

    if (index == 0)
    {
        return list_insert_at_beginning(head, data);
    }

    node *current = (*head)->next, *previous = *head;

    size_t i = 1;

    while (current != NULL)
    {
        if (i == index)
        {
            node *new_node = list_create_node(data);
            new_node->next = current;
            previous->next = new_node;
            return *head;
        }

        i++;
        previous = current;
        current = current->next;
    }

    if (i == index)
    {
        return list_insert_at_end(head, data);
    }

    return *head;
}

bool list_has_cycle(node *head)
{
    if (head == NULL)
    {
        return false;
    }

    node *slow = head, *fast = head;

    while (fast->next != NULL && fast->next->next != NULL)
    {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast)
        {
            return true;
        }
    }

    return false;
}

bool list_is_palindrome(node *head)
{
    bool is_palindrome = true;

    if (head == NULL || head->next == NULL)
    {
        return is_palindrome;
    }

    node *slow = head, *fast = head, *reversed = NULL, *tmp = NULL, *first_part = NULL, *last_part;

    while (fast != NULL && fast->next != NULL)
    {
        fast = fast->next->next;
        tmp = slow->next;
        slow->next = reversed;
        reversed = slow;
        slow = tmp;
    }

    first_part = reversed;
    last_part = slow;

    slow = (fast != NULL ? slow->next : slow);

    while (slow != NULL)
    {
        if (reversed->data != slow->data)
        {
            is_palindrome = false;
            break;
        }

        reversed = reversed->next;
        slow = slow->next;
    }

    head = list_reverse(&first_part);

    while (first_part->next != NULL)
    {
        first_part = first_part->next;
    }

    first_part->next = last_part;

    return is_palindrome;
}

node *list_search(node *head, int data)
{
    for (node *curent = head; curent != NULL; curent = curent->next)
    {
        if (curent->data == data)
        {
            return curent;
        }
    }

    return NULL;
}

size_t list_length(node *head)
{
    size_t count = 0;

    for (node *curent = head; curent != NULL; curent = curent->next)
    {
        count++;
    }

    return count;
}

node *list_sorted_merge(node *head_a, node *head_b)
{
    node *result = NULL;
    node **last = &result;

    while (head_a != NULL || head_b != NULL)
    {
        if (head_a != NULL && head_b != NULL && head_a->data <= head_b->data)
        {
            *last = head_a;
            head_a = head_a->next;
        }
        else if (head_a != NULL && head_b != NULL && head_a->data > head_b->data)
        {
            *last = head_b;
            head_b = head_b->next;
        }
        else if (head_a != NULL)
        {
            *last = head_a;
            head_a = head_a->next;
        }
        else if (head_b != NULL)
        {
            *last = head_b;
            head_b = head_b->next;
        }

        last = &((*last)->next);
    }

    return result;
}

void list_split(node *head, node **init, node **end)
{
    node *fast, *slow;

    if (head == NULL || head->next == NULL)
    {
        *init = head;
        *end = NULL;
    }
    else
    {
        slow = head;
        fast = head->next;

        while (fast != NULL)
        {
            fast = fast->next;
            if (fast != NULL)
            {
                slow = slow->next;
                fast = fast->next;
            }
        }

        *init = head;
        *end = slow->next;
        slow->next = NULL;
    }
}

void list_merge_sort(node **head)
{
    struct node *a, *b;

    if ((*head == NULL) || ((*head)->next == NULL))
    {
        return;
    }

    list_split(*head, &a, &b);
    list_merge_sort(&a);
    list_merge_sort(&b);

    *head = list_sorted_merge(a, b);
}