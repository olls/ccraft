#define BLOCK_W 8
#define BLOCK_H 8


#define print_c(c) {printf("rgb(%d, %d, %d)\n", c->r, c->g, c->b);}
#define print_c_hex(c) {printf("%%%X%X%X,", c->r, c->g, c->b);}


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
  int r;
  int g;
  int b;
} color_t;


void
error(char * mgs, ...);
