/*
Article: https://opensource.com/article/21/8/memory-programming-c

Adding validation if the malloc returned NULL in "get_heap_array" function
*/

#include <stdio.h>
#include <stdlib.h>

int *get_heap_array(unsigned n)
{
    int *heap_nums = malloc(sizeof(int) * n);

    /*
    https://opensource.com/article/21/8/memory-programming-c#comment-217606
    */
    if (NULL == heap_nums) /* failure? */
        return NULL;       /* if so, return NULL */

    unsigned i;
    for (i = 0; i < n; i++)
        heap_nums[i] = i + 1; /* initialize the array */

    /* stack storage for variables heap_nums and i released
     automatically when get_num_array returns */
    return heap_nums; /* return (copy of) the pointer */
}

int main()
{
    unsigned n = 100, i;
    int *heap_nums = get_heap_array(n); /* save returned address */

    if (NULL == heap_nums) /* malloc failed */
        fprintf(stderr, "%s\n", "malloc(...) failed...");
    else
    {
        for (i = 0; i < n; i++)
            printf("%i\n", heap_nums[i]);
        free(heap_nums); /* free the heap storage */
    }

    return 0;
}