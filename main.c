#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define BLOCK_W 8
#define BLOCK_H 8
#define WIDTH 100
#define HEIGHT 60
#define WIDTH_P BLOCK_W * WIDTH
#define HEIGHT_P BLOCK_H * HEIGHT
#define MOVE_RATE 1/30

enum block {EMPTY, STONE, GRASS, PLAYER, SKY};

struct coord {
  int x;
  int y;
};


void memset32 (Uint32 *ptr, Uint32 fill, size_t size) {
  while(size) {
    ptr[--size] = fill;
  }
}


void clear (enum block *blocks, enum block fill) {
  size_t size = WIDTH_P * HEIGHT_P;
  while(size) {
    blocks[--size] = fill;
  }
}


Uint32 block_c (enum block block) {
  switch (block) {
    case STONE:
      // return 0xabcdef;
      return 0xCCCCCC;
    case GRASS:
      return 0x00FF00;
    case PLAYER:
      return 0xDA9898;
    case SKY:
    default:
      return 0x0000FF;
  }
}


void blocks_to_pixels (enum block *blocks, enum block * objects, Uint32 *pixels) {
  size_t size = WIDTH * HEIGHT;
  int dy;
  Uint32 color;

  while (size) {
    size--;

    if (objects[size] != EMPTY) {
      color = block_c(objects[size]);
    } else {
      color = block_c(blocks[size]);
    }

    for (dy = 0; dy < BLOCK_H; dy++) {
      memset32(pixels + ((size/WIDTH)*WIDTH_P*BLOCK_H) + ((size%WIDTH)*BLOCK_W) + (dy*WIDTH_P), color, BLOCK_W);
    }
  }
}


void set_block (enum block *blocks, enum block block, int x, int y) {
  if (x >= WIDTH || y >= HEIGHT) {
    return;
  }
  blocks[y*WIDTH + x] = block;
}


void place_objects(enum block * objects, struct coord player) {
  clear(objects, EMPTY);
  set_block(objects, PLAYER, player.x, player.y);
}


void set_scene(enum block * blocks) {
  int x, y;
  int ground = 2 * HEIGHT / 3;

  clear(blocks, SKY);

  for (x = 0; x < WIDTH; x++) {
    if (ground == HEIGHT - 3) {
      ground = ground - (rand()%2);
    } else if (ground == 1) {
      ground = ground + (rand()%2);
    } else {
      ground = ground - (1 - rand()%3);
    }

    set_block(blocks, GRASS, x, ground);

    for (y = ground + 1; y < HEIGHT; y++) {
      set_block(blocks, STONE, x, y);
    }
  }
}


int xy(int x, int y) {
  return (y * WIDTH) + x;
}


void move_player(struct coord *player, SDL_KeyboardEvent key, enum block * blocks) {
  if (key.keysym.sym == SDLK_RIGHT && player->x < WIDTH-1) {
    if (blocks[xy(player->x + 1, player->y)] == SKY) {
      player->x++;

    } else if (player->y >= 0 &&
               blocks[xy(player->x, player->y - 1)] == SKY &&
               blocks[xy(player->x + 1, player->y - 1)] == SKY) {
      player->x++;
      player->y--;

    }
  }
  if (key.keysym.sym == SDLK_LEFT && player->x > 0) {
    if (blocks[xy(player->x - 1, player->y)] == SKY) {
      player->x--;

    } else if (player->y >= 0 &&
               blocks[xy(player->x, player->y - 1)] == SKY &&
               blocks[xy(player->x - 1, player->y - 1)] == SKY) {
      player->x--;
      player->y--;

    }
  }
}

void gravity(struct coord *player, enum block * blocks) {
  if (blocks[xy(player->x, player->y + 1)] == SKY) {
    player->y++;
  }
}


int main (int argc, char *argv) {
  int quit = 0;
  SDL_Event event;

  srand(time(NULL));

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window * window = SDL_CreateWindow("CCraft",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH_P, HEIGHT_P, 0);

  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Texture * texture = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WIDTH_P, HEIGHT_P);

  Uint32 *pixels = (Uint32 *)malloc(WIDTH_P * HEIGHT_P * sizeof(Uint32));
  enum block *blocks = (enum block *)malloc(WIDTH_P * HEIGHT_P * sizeof(enum block));
  enum block *objects = (enum block *)malloc(WIDTH_P * HEIGHT_P * sizeof(enum block));

  struct coord player = {WIDTH/2, 4};

  set_scene(blocks);

  while (!quit) {

    place_objects(objects, player);
    blocks_to_pixels(blocks, objects, pixels);

    SDL_UpdateTexture(texture, NULL, pixels, WIDTH_P * sizeof(Uint32));
    SDL_PollEvent(&event);

    switch (event.type) {
      case SDL_KEYDOWN:
        move_player(&player, event.key, blocks);
        break;
      case SDL_QUIT:
        quit = 1;
        break;
    }

    gravity(&player, blocks);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
  }

  free(pixels);
  free(blocks);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
