#include <stdio.h>
#include <stdlib.h>

#include "util/common.h"

#define IMG_FILE_SIZE WIDTH * HEIGHT * 4


color_t *
load_raw(char filename[])
{
  FILE * fp = fopen(filename, "r");
  if (fp == NULL) {
    error("Cannot open texture file: \"%s\"", filename);
    return NULL;
  }

  color_t * tex = (color_t *)malloc(BLOCK_W * BLOCK_H * sizeof(color_t));
  fread(tex, sizeof(color_t), BLOCK_W * BLOCK_H, fp);
  fclose(fp);

  return tex;
}
