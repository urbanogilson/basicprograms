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
            insert_at_end(&head, i);
        }
        else
        {
            insert_at_beginning(&head, i);
        }
    }

    traverse(head);

    remove_at_beginning(&head);

    traverse(head);

    remove_at_end(&head);

    sort(&head);

    traverse(head);

    reverse(&head);

    traverse(head);

    printf("Value in the second node: %d\n", get(head, 1));

    free_linkedlist(head);
}
