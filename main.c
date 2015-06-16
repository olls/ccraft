#include <stdio.h>
#include <SDL2/SDL.h>

#define BLOCK_W 8
#define BLOCK_H 8
#define WIDTH 100
#define HEIGHT 60
#define WIDTH_P BLOCK_W * WIDTH
#define HEIGHT_P BLOCK_H * HEIGHT

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


// void shift (pixels, d) {
//   //
// }

// void save (slice, x) {
//   //
// }

// void load (pixels, slice, x) {
//   //
// }

// void move (enum block * blocks, int x, int dx) {
//   int d = dx < 0 ? -1 : +1;
//   int right_x, left_x;
//   enum block *right, *left;

//   while (dx != 0) {
//     x += d;
//     dx += -d;

//     left_x = x - (WIDTH/2);
//     left = blocks;
//     right_x = x + (WIDTH/2);
//     right = blocks + WIDTH_P;
//     if (d > 0) {
//       save(left, left_x);
//       shift(blocks, d);
//       load(blocks, right, right_x);
//     } else {
//       save(right, right_x);
//       shift(blocks, d);
//       load(blocks, left, left_x);
//     }

//   }
// }


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
  int x;

  clear(blocks, SKY);

  for (x = 0; x < WIDTH; x++) {
    set_block(blocks, GRASS, x, HEIGHT-3);
    set_block(blocks, STONE, x, HEIGHT-2);
    set_block(blocks, STONE, x, HEIGHT-1);
  }
}


void move_player(struct coord *player, SDL_KeyboardEvent key) {
  if (key.keysym.sym == SDLK_RIGHT) {
    player->x++;
  }
  if (key.keysym.sym == SDLK_LEFT) {
    player->x--;
  }
}


int main (int argc, char *argv) {
  int quit = 0;
  SDL_Event event;

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window * window = SDL_CreateWindow("CCraft",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH_P, HEIGHT_P, 0);

  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Texture * texture = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WIDTH_P, HEIGHT_P);

  Uint32 *pixels = (Uint32 *)malloc(WIDTH_P * HEIGHT_P * sizeof(Uint32));
  enum block *blocks = (enum block *)malloc(WIDTH_P * HEIGHT_P * sizeof(enum block));
  enum block *objects = (enum block *)malloc(WIDTH_P * HEIGHT_P * sizeof(enum block));

  struct coord player = {WIDTH/2, HEIGHT-4};

  set_scene(blocks);

  while (!quit) {

    place_objects(objects, player);
    blocks_to_pixels(blocks, objects, pixels);

    SDL_UpdateTexture(texture, NULL, pixels, WIDTH_P * sizeof(Uint32));
    SDL_WaitEvent(&event);

    switch (event.type) {
      case SDL_KEYDOWN:
        move_player(&player, event.key);
        break;
      case SDL_QUIT:
        quit = 1;
        break;
    }

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
