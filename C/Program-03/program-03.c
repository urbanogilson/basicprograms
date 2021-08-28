/*
Linked List
*/

#include <stdio.h>
#include <stdlib.h>
#include "linked-list.h"

int main(void)
{
    node *head = NULL;

    print_linkedlist(head);

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

    print_linkedlist(head);

    head = remove_at_beginning(head);

    print_linkedlist(head);

    head = remove_at_end(head);

    print_linkedlist(head);

    free_linkedlist(head);
}
