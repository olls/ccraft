enum block {EMPTY, STONE, GRASS, PLAYER, SKY};

struct coord {
  int x;
  int y;
};

struct color {
  int r;
  int g;
  int b;
};



typedef enum block block;
typedef block *blocks;

typedef struct coord coord;
typedef struct color color;
