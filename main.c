#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "util/common.h"

#define WIDTH 75
#define HEIGHT 50
#define WIDTH_P BLOCK_W * WIDTH
#define HEIGHT_P BLOCK_H * HEIGHT


void
memset32(Uint32 * ptr, Uint32 fill, size_t size)
{
  while(size)
  {
    ptr[--size] = fill;
  }
}


void
clear(blocks area, block fill)
{
  size_t size = WIDTH * HEIGHT;
  while(size)
  {
    area[--size] = fill;
  }
}


Uint32
block_c(block block)
{
  switch (block)
  {
    case STONE:
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


void
blocks_to_pixels(blocks scene, blocks objects, Uint32 * pixels)
{
  size_t size = WIDTH * HEIGHT;
  int dy, rows_l, row_l;
  Uint32 color;

  while (size--)
  {

    if (objects[size] != EMPTY)
    {
      color = block_c(objects[size]);
    }
    else
    {
      color = block_c(scene[size]);
    }

    // Number of pixels upto the current row
    rows_l = (size / WIDTH) * WIDTH_P * BLOCK_H;
    // Number of pixels in row upto block
    row_l = (size % WIDTH) * BLOCK_W;

    // Loop for every row of pixels in block height
    for (dy = 0; dy < BLOCK_H; dy++)
    {
      memset32(pixels + rows_l + row_l + (dy * WIDTH_P), color, BLOCK_W);
    }
  }
}


int
xy(int x, int y)
{
  return (y * WIDTH) + x;
}


void
set_block(blocks scene, block block, int x, int y)
{
  if (x >= WIDTH || y >= HEIGHT)
  {
    return;
  }
  scene[xy(x, y)] = block;
}


void
place_objects(blocks objects, coord player)
{
  clear(objects, EMPTY);
  set_block(objects, PLAYER, player.x, player.y);
}


void
setup_scene(blocks scene)
{
  int x, y;
  int ground = 2 * HEIGHT / 3;

  clear(scene, SKY);

  for (x = 0; x < WIDTH; x++)
  {
    if (ground == HEIGHT - 3)
    {
      ground -= rand() % 2;
    }
    else if (ground == 1)
    {
      ground += rand() % 2;
    }
    else
    {
      ground -= 1 - rand() % 3;
    }

    set_block(scene, GRASS, x, ground);

    for (y = ground + 1; y < HEIGHT; y++)
    {
      set_block(scene, STONE, x, y);
    }
  }
}


void
move_player(coord * player, SDL_KeyboardEvent key, blocks blocks)
{
  if (key.keysym.sym == SDLK_UP && player->y > 1 &&
      blocks[xy(player->x, player->y - 1)] == SKY &&
      blocks[xy(player->x, player->y - 2)] == SKY &&
      blocks[xy(player->x, player->y + 1)] != SKY)
  {
    player->y -= 2;
  }

  if (key.keysym.sym == SDLK_RIGHT && player->x < WIDTH-1)
  {
    if (blocks[xy(player->x + 1, player->y)] == SKY)
    {
      player->x++;
    } else if (player->y >= 0 &&
               blocks[xy(player->x, player->y - 1)] == SKY &&
               blocks[xy(player->x + 1, player->y - 1)] == SKY) {
      player->x++;
      player->y--;
    }
  }

  if (key.keysym.sym == SDLK_LEFT && player->x > 0)
  {
    if (blocks[xy(player->x - 1, player->y)] == SKY)
    {
      player->x--;

    }
    else if (player->y >= 0 &&
             blocks[xy(player->x, player->y - 1)] == SKY &&
             blocks[xy(player->x - 1, player->y - 1)] == SKY)
    {
      player->x--;
      player->y--;
    }
  }
}


void
gravity(coord * player, blocks blocks)
{
  if (blocks[xy(player->x, player->y + 1)] == SKY)
  {
    player->y++;
  }
}


int
main(int argc, char * argv)
{
  int quit = 0;
  SDL_Event event;

  srand(time(NULL));
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window * window = SDL_CreateWindow("CCraft",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH_P, HEIGHT_P, 0);

  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Texture * texture = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WIDTH_P, HEIGHT_P);

  // The pixel values
  Uint32 * pixels = (Uint32 *)malloc(WIDTH_P * HEIGHT_P * sizeof(Uint32));
  // The blocks array
  blocks scene = (blocks)malloc(WIDTH * HEIGHT * sizeof(block));
  // A second layer of blocks
  blocks objects = (blocks)malloc(WIDTH * HEIGHT * sizeof(block));

  coord player = {WIDTH / 2, 4};

  setup_scene(scene);

  printf("Starting\n");
  while (!quit)
  {
    place_objects(objects, player);
    blocks_to_pixels(scene, objects, pixels);

    SDL_UpdateTexture(texture, NULL, pixels, WIDTH_P * sizeof(Uint32));
    SDL_PollEvent(&event);

    switch (event.type)
    {
      case SDL_KEYDOWN:
        move_player(&player, event.key, scene);
        break;
      case SDL_QUIT:
        quit = 1;
        error("Quitting");
        break;
    }

    gravity(&player, scene);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
  }

  free(pixels);
  free(scene);
  free(objects);

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
