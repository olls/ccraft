#include <stdio.h>
#include <stdlib.h>

#include "util/common.h"
#include "textures.h"


struct color *
load_b(char data[])
{
  struct color * tex = (struct color *)malloc(BLOCK_W * BLOCK_H * sizeof(struct color));

  for (int i = 0; i < BLOCK_W * BLOCK_H; i++)
  {
    tex[i].r = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4));
    tex[i].g = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2));
    tex[i].b = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33)));

    data += 4;
  }

  return tex;
}
