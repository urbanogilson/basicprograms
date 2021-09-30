#include <stdio.h>
#include "matrix.h"

int main(void)
{
    int x = 5;
    int *ptr_x = &x;
    fprintf(stdout, "x = %d\t &x = %p\n", x, (void *)&x);
    fprintf(stdout, "ptr_x = %p\t &ptr_x = %p\t *ptr_x = %d\n", (void *)ptr_x, (void *)&ptr_x, *ptr_x);
    *ptr_x = 10;
    fprintf(stdout, "x = %d\n", x);
    fprintf(stdout, "\n");

    int array[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    for (size_t i = 0; i < 9; i++)
    {
        fprintf(stdout, "%d ", array[i]); /* Array notation */
        // fprintf(stdout, "%d\n", *(array + i)); /* Pointer notation */
        // fprintf(stdout, "%d\n", *(i + array)); /* Pointer notation (Also valid) */
        // fprintf(stdout, "%d\n", i[array]);     /* Array notation (Also valid) */
    }
    fprintf(stdout, "\n\n");

    size_t n_rows = 3, n_cols = 3;

    int **matrix = matrix_dummy(n_rows, n_cols);
    matrix_print(matrix, n_rows, n_cols);
    fprintf(stdout, "\n");
    matrix_print_memory(matrix, n_rows, n_cols);
    matrix_free(matrix);
}
