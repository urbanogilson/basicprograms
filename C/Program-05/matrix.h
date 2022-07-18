#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>

typedef struct matrix_t {
  int **matrix;
  size_t rows;
  size_t cols;
} matrix_t;

matrix_t matrix_create(const size_t rows, const size_t cols);

void matrix_free(matrix_t matrix);

matrix_t matrix_ones(const size_t rows, const size_t cols);

matrix_t matrix_multiply(const matrix_t matrix_a, const matrix_t matrix_b);

void matrix_print(FILE *output, const matrix_t *matrix);

void matrix_print_memory(FILE *output, const matrix_t *matrix);

#endif /* MATRIX_H */