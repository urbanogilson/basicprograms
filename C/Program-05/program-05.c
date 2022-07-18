#include <stdio.h>

#include "matrix.h"

size_t str_len(const char *string) {
  size_t len = 0;

  while (*(string + len) != '\0') {
    len++;
  }

  return len;
}

int main(void) {
  int x = 5;
  int *ptr_x = &x;
  fprintf(stdout, "x = %d\t &x = %p\n", x, (void *)&x);
  fprintf(stdout, "ptr_x = %p\t &ptr_x = %p\t *ptr_x = %d\n", (void *)ptr_x,
          (void *)&ptr_x, *ptr_x);
  *ptr_x = 10;
  fprintf(stdout, "x = %d\n", x);
  fprintf(stdout, "\n");

  int array[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  for (size_t i = 0; i < 9; i++) {
    fprintf(stdout, "%d ", array[i]);  // Array notation
    // fprintf(stdout, "%d\n", *(array + i));  // Pointer notation
    // fprintf(stdout, "%d\n", *(i + array));  // Pointer notation (Also valid)
    // fprintf(stdout, "%d\n", i[array]);      // Array notation (Also valid)
  }
  fprintf(stdout, "\n\n");

  char *test = "Hello, World!";
  fprintf(stdout, "Length of '%s' is %zd\n", test, str_len(test));

  matrix_t m = matrix_ones(3, 3);
  matrix_print(stdout, &m);
  matrix_print_memory(stdout, &m);
  matrix_free(m);

  matrix_t m_a = matrix_ones(4, 3);
  matrix_t m_b = matrix_ones(3, 2);
  matrix_t m_c = matrix_multiply(m_a, m_b);
  matrix_print(stdout, &m_c);
  matrix_print_memory(stdout, &m_c);
  matrix_free(m_a);
  matrix_free(m_b);
  matrix_free(m_c);
}
