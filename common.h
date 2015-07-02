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
