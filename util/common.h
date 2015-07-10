#include <stdint.h>

#define BLOCK_W 8
#define BLOCK_H 8


typedef enum block {
  EMPTY,
  STONE,
  GRASS,
  PLAYER,
  SKY,
  NUM_BLOCKS
} block_t;

typedef struct texture {
  enum block type;
  struct color * pixels;
} texture_t;

typedef struct coord {
  int x;
  int y;
} coord_t;

typedef struct color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} color_t;


void
error(char * mgs, ...);
