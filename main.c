#include <stdio.h>
#include <SDL2/SDL.h>

#define BLOCK_W 10
#define BLOCK_H 10
#define WIDTH 1
#define HEIGHT 1
#define WIDTH_P BLOCK_W * WIDTH
#define HEIGHT_P BLOCK_H * HEIGHT

enum block {STONE, GRASS, SKY};


void memset32 (Uint32 *ptr, Uint32 fill, size_t size) {
  printf("%a\n", ptr);
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
      return 0xabcdef;
      // return 0xCCCCCC;
    case GRASS:
      return 0x00FF00;
    case SKY:
    default:
      return 0x0000FF;
  }
}


void set_block (enum block *blocks, enum block block, int x, int y) {
  if (x >= BLOCK_W || y >= HEIGHT) {
    return;
  }
  blocks[y*WIDTH + x] = block;
}

void blocks_to_pixels (enum block *blocks, Uint32 *pixels) {
  size_t size = WIDTH * HEIGHT;
  int dy;
  printf("\n");
  while (size) {
    size--;
    Uint32 color = block_c(blocks[size]);
    for (dy = 0; dy < BLOCK_H; dy++) {
      printf("%d\n", ((size/WIDTH)*WIDTH_P*BLOCK_H) + ((size%WIDTH)*BLOCK_W) + dy);
      memset32(pixels + ((size/WIDTH)*WIDTH_P*BLOCK_H) + ((size%WIDTH)*BLOCK_W) + dy, color, BLOCK_W);
    }
  }
}


void shift (pixels, d) {
  //
}

void save (slice, x) {
  //
}

void load (pixels, slice, x) {
  //
}

void move (Uint32 * pixels, int x, int dx) {
  int d = dx < 0 ? -1 : +1;
  int right_x, left_x;
  Uint32 *right, *left;

  while (dx != 0) {
    x += d;
    dx += -d;

    left_x = x - (WIDTH/2);
    left = pixels;
    right_x = x + (WIDTH/2);
    right = pixels + WIDTH_P;
    if (d > 0) {
      save(left, left_x);
      shift(pixels, d);
      load(pixels, right, right_x);
    } else {
      save(right, right_x);
      shift(pixels, d);
      load(pixels, left, left_x);
    }

  }
}


int main (int argc, char *argv) {
  int quit = 0;
  int x = 0;
  int y = 0;
  SDL_Event event;

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window * window = SDL_CreateWindow("CCraft",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH_P, HEIGHT_P, 0);

  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Texture * texture = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WIDTH_P, HEIGHT_P);

  Uint32 *pixels = (Uint32 *)malloc(WIDTH_P * HEIGHT_P * sizeof(Uint32));
  enum block *blocks = (Uint32 *)malloc(WIDTH_P * HEIGHT_P * sizeof(enum block));

  clear(blocks, SKY);

  while (!quit) {
    blocks_to_pixels(blocks, pixels);
    SDL_UpdateTexture(texture, NULL, pixels, WIDTH_P * sizeof(Uint32));
    SDL_WaitEvent(&event);

    // set_block(blocks, STONE, x++, y);
    // if (x >= WIDTH) {
    //   x = 0;
    //   y++;
    // }

    switch (event.type) {
      case SDL_QUIT:
        quit = 1;
        break;
    }

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
  }

  free(pixels);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
