enum block {EMPTY, STONE, GRASS, PLAYER, SKY};

struct coord {
  int x;
  int y;
};


typedef enum block block;
typedef block *blocks;

typedef struct coord coord;
