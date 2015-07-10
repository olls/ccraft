#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "util/common.h"
#include "loader.h"
#include "textures.h"


#define WIDTH 75
#define HEIGHT 50
#define WIDTH_P BLOCK_W * WIDTH
#define HEIGHT_P BLOCK_H * HEIGHT


void
clear(block_t area[], block_t fill)
{
  size_t size = WIDTH * HEIGHT;
  while(size)
  {
    area[--size] = fill;
  }
}


Uint32
get_color(color_t * c)
{
  //       AARRGGBB
  return 0x00000000 |
         c->r << 16 |
         c->g << 8  |
         c->b << 0;
}


void
get_texture_row(color_t * texture, int y, Uint32 output[])
{
  for (int dx = 0; dx < BLOCK_W; dx++)
  {
    output[dx] = get_color(texture + ( y * BLOCK_W ) + dx);
  }
}


void
draw_blocks(
  color_t * textures[],  // List of pointers to textures.
  block_t scene[],
  block_t objects[],
  uint32_t * pixels  // Output buffer
) {

  // Loop through all blocks
  for (size_t i = 0; i < WIDTH * HEIGHT; i++)
  {

    block_t current;
    if (objects[i] != EMPTY)
    {
      current = objects[i];
    }
    else
    {
      current = scene[i];
    }

    // Number of pixels up to the current row
    int rows_l = (i / WIDTH) * WIDTH_P * BLOCK_H;

    // Number of pixels in row up to block
    int row_l = (i % WIDTH) * BLOCK_W;


    // Loop for every row of pixels in block height
    for (int dy = 0; dy < BLOCK_H; dy++)
    {
      Uint32 * row_pos = pixels + rows_l + row_l + (dy * WIDTH_P);

      get_texture_row(textures[current], dy, row_pos);

    }
  }
}


int
xy(int x, int y)
{
  return (y * WIDTH) + x;
}


void
set_block(block_t scene[], block_t block, int x, int y)
{
  if (x >= WIDTH || y >= HEIGHT)
  {
    return;
  }
  scene[xy(x, y)] = block;
}


void
place_objects(block_t objects[], coord_t player)
{
  clear(objects, EMPTY);
  set_block(objects, PLAYER, player.x, player.y);
}


void
setup_scene(block_t scene[])
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
move_player(coord_t * player, SDL_KeyboardEvent key, block_t blocks[])
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
    }
    else if (player->y >= 0 &&
             blocks[xy(player->x, player->y - 1)] == SKY &&
             blocks[xy(player->x + 1, player->y - 1)] == SKY)
    {
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
gravity(coord_t * player, block_t blocks[])
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
  block_t * scene = (block_t *)malloc(WIDTH * HEIGHT * sizeof(block_t));
  // A second layer of blocks
  block_t * objects = (block_t *)malloc(WIDTH * HEIGHT * sizeof(block_t));

  coord_t player = {WIDTH / 2, 4};

  setup_scene(scene);


  // Allocate array of pointers to textures
  color_t ** textures = (color_t **)malloc(NUM_BLOCKS * sizeof(color_t *));

  textures[STONE]  = load_b(stone_d);
  textures[GRASS]  = load_b(grass_d);
  textures[PLAYER] = load_b(player_d);
  textures[SKY]    = load_b(sky_d);


  printf("Starting\n");

  while (!quit)
  {
    place_objects(objects, player);
    draw_blocks(textures, scene, objects, pixels);

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
  free(textures); // TODO(olls): free individual textures

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  printf("Finished\n");

  return 0;
}
