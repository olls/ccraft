#include <stdint.h>

#define BLOCK_W 8
#define BLOCK_H 8

#define LIST_PART_LENGTH 64


typedef enum block
{
  EMPTY,
  STONE,
  GRASS,
  PLAYER,
  SKY,
  NUM_BLOCKS
} block_t;


typedef uint32_t color_t;

typedef struct vec2
{
  int x;
  int y;
} vec2_t;

typedef struct object
{
  color_t * texture;
  int width;
  int height;
  vec2_t pos;
} object_t;


void
error(char * mgs, ...);

uint64_t
get_us();