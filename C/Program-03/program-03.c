/*
Linked List
*/

#include <stdio.h>
#include <stdlib.h>
#include "linked-list.h"

int main(void)
{
    node *head = NULL;

    traverse(head);

    for (int i = 0; i < 10; i++)
    {
        if (i % 2 == 0)
        {
            head = insert_at_end(head, i);
        }
        else
        {
            head = insert_at_beginning(head, i);
        }
    }

    traverse(head);

    head = remove_at_beginning(head);

    traverse(head);

    head = remove_at_end(head);

    head = sort(head);

    traverse(head);

    head = reverse(head);

    traverse(head);

    free_linkedlist(head);
}
