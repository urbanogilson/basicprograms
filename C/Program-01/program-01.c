/*
Write a program that prints the endianness of the computer.

big-endian (BE) or little-endian (LE)
*/

#include <inttypes.h>
#include <stdio.h>

int main(void) {
  uint16_t number = 1;  // 0x0001

  uint8_t *cptr = (uint8_t *)&number;

  if (*cptr == 1)  // 0x01
  {
    printf("little-endian (LE)\n");
  } else if (*cptr == 0)  // 0x00
  {
    printf("big-endian (BE)\n");
  }
}
