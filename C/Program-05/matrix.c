#include "matrix.h"
#include <stdio.h>

int **matrix_create(size_t n_rows, size_t n_cols)
{
    int *array = (int *)malloc(n_rows * n_cols * sizeof(int));

    if (array == NULL)
    {
        return NULL;
    }

    int **matrix = malloc(n_rows * sizeof(int *));

    if (matrix == NULL)
    {
        free(array);
        return NULL;
    }

    for (size_t i = 0; i < n_rows; i++)
    {
        *(matrix + i) = array + (i * n_cols);
    }

    return matrix;
}

int **matrix_dummy(size_t n_rows, size_t n_cols)
{
    int **matrix = matrix_create(n_rows, n_rows);

    if (matrix == NULL)
    {
        return NULL;
    }

    int count = 0;

    for (size_t i = 0; i < n_rows; i++)
    {
        for (size_t j = 0; j < n_cols; j++)
        {
            *(*(matrix + i) + j) = count++;
        }
    }

    return matrix;
}

void matrix_free(int **matrix)
{
    free(*matrix);
    free(matrix);
}

void matrix_print(int **matrix, size_t n_rows, size_t n_cols)
{
    for (size_t i = 0; i < n_rows; i++)
    {
        for (size_t j = 0; j < n_cols; j++)
        {
            fprintf(stdout, "%02d ", *(*(matrix + i) + j));
        }
        fprintf(stdout, "\n");
    }
}

void matrix_print_memory(int **matrix, size_t n_rows, size_t n_cols)
{
    for (size_t i = 0; i < n_rows; i++)
    {
        for (size_t j = 0; j < n_cols; j++)
        {
            fprintf(stdout, "%p ", (void *)(*(matrix + i) + j));
        }
        fprintf(stdout, "\n");
    }
}