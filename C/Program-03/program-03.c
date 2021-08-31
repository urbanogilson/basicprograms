/*
Linked List
*/

#include <stdio.h>
#include <stdlib.h>
#include "linked-list.h"

int main(void)
{
    node *head = NULL;

    list_traverse(head);

    for (int i = 0; i < 10; i++)
    {
        if (i % 2 == 0)
        {
            list_insert_at_end(&head, i);
        }
        else
        {
            list_insert_at_beginning(&head, i);
        }
    }

    list_traverse(head);

    list_remove_at_beginning(&head);

    list_traverse(head);

    list_remove_at_end(&head);

    list_sort(&head);

    list_traverse(head);

    list_sort(&head);

    list_traverse(head);

    list_reverse(&head);

    list_traverse(head);

    printf("Value in the second node: %d\n", list_get_at_index(head, 1));

    list_remove_at_index(&head, 1);

    list_traverse(head);

    list_free(head);
}
