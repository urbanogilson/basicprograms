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

    list_insert_at_index(&head, 1, 11);

    list_traverse(head);

    printf("List Has Cycle: %s\n", list_has_cycle(head) ? "Yes" : "No");

    node *cycle = NULL;
    cycle = list_create_node(1);
    cycle->next = list_create_node(2);
    cycle->next->next = list_create_node(3);
    cycle->next->next->next = list_create_node(4);
    cycle->next->next->next->next = list_create_node(5);
    cycle->next->next->next->next->next = list_create_node(6);
    cycle->next->next->next->next->next->next = list_create_node(7);
    cycle->next->next->next->next->next->next->next = list_create_node(8);
    cycle->next->next->next->next->next->next->next->next = cycle->next->next->next; // 8 -> 4

    printf("List Has Cycle: %s\n", list_has_cycle(cycle) ? "Yes" : "No");

    cycle->next->next->next->next->next->next->next->next = NULL;

    list_free(cycle);

    list_free(head);
}
