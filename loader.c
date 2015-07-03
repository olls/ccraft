#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "textures.h"


void print_c (color *c) {
  printf("rgb(%d, %d, %d)\n", c->r, c->g, c->b);
}


texture load_b (char data[]) {
  color c;
  texture tex = (texture)malloc(BLOCK_W * BLOCK_H * sizeof(color));
  size_t size = BLOCK_W * BLOCK_H;
  int i = 0;

  while (i <= size) {
    c.r = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4));
    c.g = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2));
    c.b = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33)));
    data += 4;

    tex[i++] = c;
  }
}


int main (int argc, char *argv) {

  texture block_tex = load_b(r_block_tex);

  return 0;
}
