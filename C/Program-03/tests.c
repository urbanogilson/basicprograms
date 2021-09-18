#include "unittest.h"
#include <stdio.h>
#include "linked-list.h"

int main(void)
{
    // NULL
    node *head = NULL;
    TEST(list_length(head) == 0);

    // 10 -> NULL
    head = list_create_node(10);
    TEST(head != NULL && head->data == 10 && head->next == NULL);
    TEST(list_length(head) == 1);

    // 10 -> 20 -> NULL
    head->next = list_create_node(20);
    TEST(head->next != NULL && head->next->data == 20 && head->next->next == NULL);
    TEST(list_length(head) == 2);

    // 30 -> 10 -> 20 -> NULL
    list_insert_at_beginning(&head, 30);
    TEST(head != NULL && head->data == 30 && head->next->data == 10 && head->next->next->data == 20 && head->next->next->next == NULL);

    // 10 -> 20 -> NULL
    list_remove_at_beginning(&head);
    TEST(head != NULL && head->data == 10 && head->next->data == 20 && head->next->next == NULL);

    // 10 -> 20 -> 40 -> NULL
    list_insert_at_end(&head, 40);
    TEST(head != NULL && head->data == 10 && head->next->data == 20 && head->next->next->data == 40 && head->next->next->next == NULL);

    // 10 -> 20 -> NULL
    list_remove_at_end(&head);
    TEST(head != NULL && head->data == 10 && head->next->data == 20 && head->next->next == NULL);

    // 10 -> 20 -> NULL
    node *search = list_search(head, 20);
    TEST(search != NULL && search->data == 20);
    TEST(list_search(head, 15) == NULL);

    // 30 -> 10 -> 20 -> NULL
    list_insert_at_beginning(&head, 30);
    // 10 -> 20 -> 30 -> NULL
    list_sort(&head);
    TEST(head != NULL && head->data == 10 && head->next->data == 20 && head->next->next->data == 30 && head->next->next->next == NULL);

    // 30 -> 20 -> 10 -> NULL
    list_reverse(&head);
    TEST(head != NULL && head->data == 30 && head->next->data == 20 && head->next->next->data == 10 && head->next->next->next == NULL);

    // 30 -> 20 -> 10 -> NULL
    TEST(list_get_at_index(head, -1) == -1)
    TEST(list_get_at_index(head, 0) == 30)
    TEST(list_get_at_index(head, 1) == 20)
    TEST(list_get_at_index(head, 2) == 10)
    TEST(list_get_at_index(head, 3) == -1);

    // 30 -> 20 -> 10 -> NULL
    TEST(list_insert_at_index(&head, -1, 40) == head)

    // 50 -> 30 -> 20 -> 10 -> NULL
    list_insert_at_index(&head, 0, 50);
    TEST(head != NULL && head->data == 50 && head->next->data == 30 && head->next->next->data == 20 && head->next->next->next->data == 10 && head->next->next->next->next == NULL);

    // 50 -> 30 -> 40 -> 20 -> 10 -> NULL
    list_insert_at_index(&head, 2, 40);
    TEST(head != NULL && head->data == 50 && head->next->data == 30 && head->next->next->data == 40 && head->next->next->next->data == 20 && head->next->next->next->next->data == 10 && head->next->next->next->next->next == NULL);

    // 50 -> 30 -> 40 -> 20 -> 10 -> 60 -> NULL
    list_insert_at_index(&head, 5, 60);
    TEST(head != NULL && head->data == 50 && head->next->data == 30 && head->next->next->data == 40 && head->next->next->next->data == 20 && head->next->next->next->next->data == 10 && head->next->next->next->next->next->data == 60 && head->next->next->next->next->next->next == NULL);

    // 50 -> 30 -> 40 -> 20 -> 10 -> 60 -> NULL
    list_insert_at_index(&head, 10, 70);
    TEST(head != NULL && head->data == 50 && head->next->data == 30 && head->next->next->data == 40 && head->next->next->next->data == 20 && head->next->next->next->next->data == 10 && head->next->next->next->next->next->data == 60 && head->next->next->next->next->next->next == NULL);

    // 50 -> 30 -> 40 -> 20 -> 10 -> 60 -> NULL
    list_remove_at_index(&head, -1);
    TEST(head != NULL && head->data == 50 && head->next->data == 30 && head->next->next->data == 40 && head->next->next->next->data == 20 && head->next->next->next->next->data == 10 && head->next->next->next->next->next->data == 60 && head->next->next->next->next->next->next == NULL);

    // 50 -> 30 -> 40 -> 20 -> 10 -> 60 -> NULL
    list_remove_at_index(&head, 10);
    TEST(head != NULL && head->data == 50 && head->next->data == 30 && head->next->next->data == 40 && head->next->next->next->data == 20 && head->next->next->next->next->data == 10 && head->next->next->next->next->next->data == 60 && head->next->next->next->next->next->next == NULL);

    // 50 -> 30 -> 40 -> 20 -> 10 -> NULL
    list_remove_at_index(&head, 5);
    TEST(head != NULL && head->data == 50 && head->next->data == 30 && head->next->next->data == 40 && head->next->next->next->data == 20 && head->next->next->next->next->data == 10 && head->next->next->next->next->next == NULL);

    // 50 -> 30 -> 20 -> 10 -> NULL
    list_remove_at_index(&head, 2);
    TEST(head != NULL && head->data == 50 && head->next->data == 30 && head->next->next->data == 20 && head->next->next->next->data == 10 && head->next->next->next->next == NULL);

    // 30 -> 20 -> 10 -> NULL
    list_remove_at_index(&head, 0);
    TEST(head != NULL && head->data == 30 && head->next->data == 20 && head->next->next->data == 10 && head->next->next->next == NULL);

    // 30 -> 20 -> NULL
    // 10 -> NULL
    node *init, *end;
    list_split(head, &init, &end);
    TEST(init != NULL && init->data == 30 && init->next->data == 20 && head->next->next == NULL);
    TEST(end != NULL && end->data == 10 && end->next == NULL);

    // 20 -> 30 -> NULL
    // 10 -> NULL
    // 10 -> 20 -> 30 -> NULL
    list_sort(&init);
    head = list_sorted_merge(init, end);
    TEST(head != NULL && head->data == 10 && head->next->data == 20 && head->next->next->data == 30 && head->next->next->next == NULL);

    // 30 -> 20 -> 10 -> NULL
    list_reverse(&head);
    TEST(head != NULL && head->data == 30 && head->next->data == 20 && head->next->next->data == 10 && head->next->next->next == NULL);
    // 10 -> 20 -> 30 -> NULL
    list_merge_sort(&head);
    TEST(head != NULL && head->data == 10 && head->next->data == 20 && head->next->next->data == 30 && head->next->next->next == NULL);
}
