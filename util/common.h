#define BLOCK_W 8
#define BLOCK_H 8


#define print_c(c) {printf("rgb(%d, %d, %d)\n", c->r, c->g, c->b);}


typedef enum {EMPTY, STONE, GRASS, PLAYER, SKY} block;

typedef block *blocks;

typedef struct {
  int x;
  int y;
} coord;

typedef struct {
  int r;
  int g;
  int b;
} color;

typedef color *texture;

void error (char *mgs, ...);
