/*
Linked List
*/

#include <stdio.h>
#include <stdlib.h>
#include "linked-list.h"

int main(void)
{
    printf("Create an empty linked list\n");
    node *head = NULL;
    list_traverse(head);

    printf("Insert elements at beginning and at the end!\n");
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

    printf("Remove at beginning!\n");
    list_remove_at_beginning(&head);
    list_traverse(head);

    printf("Remove at end!\n");
    list_remove_at_end(&head);
    list_traverse(head);

    printf("Sort list!\n");
    list_sort(&head);
    list_traverse(head);

    printf("Reverse list!\n");
    list_reverse(&head);
    list_traverse(head);

    printf("Value in the index 1: %d\n\n", list_get_at_index(head, 1));

    printf("Remove at index 1!\n");
    list_remove_at_index(&head, 1);
    list_traverse(head);

    printf("Insert at index 1!\n");
    list_insert_at_index(&head, 1, 11);
    list_traverse(head);

    printf("List Is Palindrome (head)? %s\n", list_is_palindrome(head) ? "Yes" : "No");
    list_traverse(head);

    node *palindrome = NULL;
    list_insert_at_end(&palindrome, 1);
    list_insert_at_end(&palindrome, 2);
    list_insert_at_end(&palindrome, 1);

    printf("List Is Palindrome (palindrome)? %s\n", list_is_palindrome(palindrome) ? "Yes" : "No");
    list_traverse(palindrome);

    printf("List Has Cycle (head)? %s\n", list_has_cycle(head) ? "Yes" : "No");

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

    printf("List Has Cycle (cycle)? %s\n", list_has_cycle(cycle) ? "Yes" : "No");

    cycle->next->next->next->next->next->next->next->next = NULL;

    node *found = list_search(head, 5);
    printf("\nFound value: %d\n", found ? found->data : -1);

    found = list_search(head, 10);
    printf("Found value: %d\n", found ? found->data : -1);

    printf("\nList length (palindrome): %zd\n", list_length(palindrome));
    printf("List length (cycle): %zd\n", list_length(cycle));
    printf("List length (head): %zd\n", list_length(head));

    printf("\nMerge sorted lists\n");
    list_sort(&head);
    list_sort(&palindrome);
    list_traverse(head);
    list_traverse(palindrome);
    head = list_sorted_merge(head, palindrome);
    list_traverse(head);

    printf("\nSplit list\n");
    node *a, *b;
    list_traverse(head);
    list_split(head, &a, &b);
    list_traverse(a);
    list_traverse(b);
    head = list_sorted_merge(a, b);

    printf("\nSort using merge sort\n");
    list_reverse(&head);
    list_traverse(head);
    list_merge_sort(&head);
    list_traverse(head);

    list_free(cycle);

    list_free(head);
}
