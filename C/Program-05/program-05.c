#include <stdio.h>
#include <stdlib.h>

int **create_matrix(size_t n_rows, size_t n_cols)
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

int **dummy_matrix(size_t n_rows, size_t n_cols)
{
    int **matrix = create_matrix(n_rows, n_rows);

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

void free_matrix(int **matrix, size_t n_rows, size_t n_cols)
{
    free(*matrix);
    free(matrix);
}

void print_memory(int **matrix, size_t n_rows, size_t n_cols)
{
    for (size_t i = 0; i < n_rows; i++)
    {
        for (size_t j = 0; j < n_cols; j++)
        {
            fprintf(stdout, "%p ", *(matrix + i) + j);
        }
        fprintf(stdout, "\n");
    }
}

void print_matrix(int **matrix, size_t n_rows, size_t n_cols)
{
    for (size_t i = 0; i < n_rows; i++)
    {
        for (size_t j = 0; j < n_cols; j++)
        {
            fprintf(stdout, "%d ", *(*(matrix + i) + j));
        }
        fprintf(stdout, "\n");
    }
}

int main(void)
{
    size_t n_rows = 3, n_cols = 3;

    int **matrix = create_matrix(n_rows, n_cols);
    print_matrix(matrix, n_rows, n_cols);
    print_memory(matrix, n_rows, n_cols);
    free_matrix(matrix, n_rows, n_cols);
}
