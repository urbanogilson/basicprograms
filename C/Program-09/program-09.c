#include <inttypes.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  uint16_t a;
  uint16_t b;
  uint32_t c;
  uint32_t d;
  uint32_t e;
} x_t;

typedef struct {
  uint16_t a;
  uint16_t b;
  uint16_t c;
  uint16_t d;
  uint16_t e;
} y_t;

typedef struct {
  uint8_t a;
  uint8_t b;
} z_t;

typedef struct {
  uint32_t size;
  uint8_t arena[25];
} Arena;

#undef DEFINE_ARENA
#define DEFINE_ARENA(type)       \
  struct {                       \
    uint32_t size;               \
    uint8_t arena[sizeof(type)]; \
  }

void init_arena_raw(void *const arena, const void *const data,
                    const size_t size) {
  ((Arena *)arena)->size = size;
  memcpy(((Arena *)arena)->arena, data, size);
}

void print_arena(const char *const type, const void *arena, const size_t size) {
  printf("%s: %zu, size: %u\n\t", type, size, ((Arena *)arena)->size);
  for (size_t i = 0; i < ((Arena *)arena)->size; ++i) {
    printf("0x%X ", ((Arena *)arena)->arena[i]);
  }
  printf("\n");
}

int main(void) {
  const uint8_t data[] = {0x0,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7,  0x8,
                          0x9,  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                          0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24};

  Arena x;
  init_arena_raw(&x, data, sizeof(data));
  print_arena("Arena", &x, sizeof(x));

  DEFINE_ARENA(x_t) x_instance;
  init_arena_raw(&x_instance, data, sizeof(x_instance));
  print_arena("Arena_x_t", &x_instance, sizeof(x_instance));

  DEFINE_ARENA(y_t) y_instance;
  init_arena_raw(&y_instance, data, sizeof(y_instance));
  print_arena("Arena_y_t", &y_instance, sizeof(y_instance));

  DEFINE_ARENA(z_t) z_instance;
  init_arena_raw(&z_instance, data, sizeof(z_instance));
  print_arena("Arena_z_t", &z_instance, sizeof(z_instance));

  return 0;
}
