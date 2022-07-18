#include "matrix.h"

#include <stdio.h>

matrix_t matrix_create(const size_t rows, const size_t cols) {
  matrix_t matrix = {.rows = rows, .cols = cols, .matrix = NULL};

  int *array = (int *)calloc(sizeof(int), rows * cols);

  if (array == NULL) return matrix;

  matrix.matrix = malloc(rows * sizeof(int *));

  if (matrix.matrix == NULL) {
    free(array);
    return matrix;
  }

  for (size_t i = 0; i < rows; i++) {
    *(matrix.matrix + i) = array + (i * cols);
  }

  return matrix;
}

void matrix_free(matrix_t matrix) {
  free(*matrix.matrix);
  free(matrix.matrix);
}

matrix_t matrix_ones(const size_t rows, const size_t cols) {
  matrix_t matrix = matrix_create(rows, cols);

  if (matrix.matrix == NULL) {
    return matrix;
  }

  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      *(*(matrix.matrix + i) + j) = 1;
    }
  }

  return matrix;
}

void matrix_print(FILE *output, const matrix_t *matrix) {
  if (matrix->matrix == NULL) {
    fprintf(output, "Invalid matrix\n");
    return;
  }

  for (size_t i = 0; i < matrix->rows; i++) {
    for (size_t j = 0; j < matrix->cols; j++) {
      fprintf(output, "%02d ", *(*(matrix->matrix + i) + j));
    }
    fprintf(output, "\n");
  }
}

void matrix_print_memory(FILE *output, const matrix_t *matrix) {
  if (matrix->matrix == NULL) {
    fprintf(output, "Invalid matrix\n");
    return;
  }

  for (size_t i = 0; i < matrix->rows; i++) {
    for (size_t j = 0; j < matrix->cols; j++) {
      fprintf(output, "%p ", (void *)(*(matrix->matrix + i) + j));
    }
    fprintf(output, "\n");
  }
}

matrix_t matrix_multiply(const matrix_t matrix_a, const matrix_t matrix_b) {
  matrix_t matrix = {.rows = 0, .cols = 0, .matrix = NULL};

  if (matrix_a.cols != matrix_b.rows) return matrix;

  matrix = matrix_create(matrix_a.rows, matrix_b.cols);

  if (matrix.matrix == NULL) return matrix;

  for (size_t i = 0; i < matrix.rows; i++) {
    for (size_t j = 0; j < matrix.cols; j++) {
      for (size_t k = 0; k < matrix_a.cols; k++) {
        matrix.matrix[i][j] += matrix_a.matrix[i][k] * matrix_a.matrix[k][j];
      }
    }
  }

  return matrix;
}
