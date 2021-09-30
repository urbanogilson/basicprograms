#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>

int **matrix_create(size_t n_rows, size_t n_cols);

int **matrix_dummy(size_t n_rows, size_t n_cols);

void matrix_free(int **matrix);

void matrix_print(int **matrix, size_t n_rows, size_t n_cols);

void matrix_print_memory(int **matrix, size_t n_rows, size_t n_cols);

#endif /* MATRIX_H */