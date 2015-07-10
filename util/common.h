#define BLOCK_W 8
#define BLOCK_H 8


#define print_c(c) {printf("rgb(%d, %d, %d)\n", c->r, c->g, c->b);}
#define print_c_hex(c) {printf("%%%X%X%X,", c->r, c->g, c->b);}


enum block {
  EMPTY,
  STONE,
  GRASS,
  PLAYER,
  SKY,
  NUM_BLOCKS
};

struct texture {
  enum block type;
  struct color * pixels;
};

struct coord {
  int x;
  int y;
};

struct color {
  int r;
  int g;
  int b;
};


void
error(char * mgs, ...);
