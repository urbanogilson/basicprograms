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

    printf("NULL\n\n");
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

int list_get_at_index(node *head, int index)
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

node *list_remove_at_index(node **head, int index)
{
    if (NULL == *head || index < 0)
    {
        return *head;
    }

    if (index == 0)
    {
        return list_remove_at_beginning(head);
    }

    node *current = (*head)->next, *previous = *head;

    int i = 1;

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

node *list_insert_at_index(node **head, int index, int data)
{
    if (NULL == *head || index < 0)
    {
        return *head;
    }

    if (index == 0)
    {
        return list_insert_at_beginning(head, data);
    }

    node *current = (*head)->next, *previous = *head;

    int i = 1;

    while (NULL != current)
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
    if (NULL == head)
    {
        return false;
    }

    node *walker = head, *runner = head;

    while (NULL != runner->next && NULL != runner->next->next)
    {
        walker = walker->next;
        runner = runner->next->next;

        if (walker == runner)
        {
            return true;
        }
    }

    return false;
}

bool list_is_palindrome(node *head)
{
    bool is_palindrome = true;

    if (NULL == head || NULL == head->next)
    {
        return is_palindrome;
    }

    node *slow = head, *faster = head, *reversed = NULL, *tmp = NULL, *first_part = NULL, *last_part;

    while (NULL != faster && NULL != faster->next)
    {
        faster = faster->next->next;
        tmp = slow->next;
        slow->next = reversed;
        reversed = slow;
        slow = tmp;
    }

    first_part = reversed;
    last_part = slow;

    slow = (NULL != faster ? slow->next : slow);

    while (NULL != slow)
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

    while (NULL != first_part->next)
    {
        first_part = first_part->next;
    }

    first_part->next = last_part;

    return is_palindrome;
}

node *list_search(node *head, int data)
{
    for (node *curent = head; NULL != curent; curent = curent->next)
    {
        if (curent->data == data)
        {
            return curent;
        }
    }

    return NULL;
}

int list_length(node *head)
{
    int count = 0;

    for (node *curent = head; NULL != curent; curent = curent->next)
    {
        count++;
    }

    return count;
}

node *list_sorted_merge(node *a, node *b)
{
    node *result = NULL;
    node **last = &result;

    while (NULL != a || NULL != b)
    {
        if (NULL != a && NULL != b && a->data <= b->data)
        {
            *last = a;
            a = a->next;
        }
        else if (NULL != a && NULL != b && a->data > b->data)
        {
            *last = b;
            b = b->next;
        }
        else if (NULL != a)
        {
            *last = a;
            a = a->next;
        }
        else if (NULL != b)
        {
            *last = b;
            b = b->next;
        }

        last = &((*last)->next);
    }

    return result;
}

void list_split(struct node *source, struct node **init, struct node **end)
{
    node *fast, *slow;

    if (source == NULL || source->next == NULL)
    {
        *init = source;
        *end = NULL;
    }
    else
    {
        slow = source;
        fast = source->next;

        while (NULL != fast)
        {
            fast = fast->next;
            if (NULL != fast)
            {
                slow = slow->next;
                fast = fast->next;
            }
        }

        *init = source;
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