#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#include "util/common.h"
#include "loader.h"


#define WIDTH 75
#define HEIGHT 50
#define WIDTH_P (BLOCK_W * WIDTH)
#define HEIGHT_P (BLOCK_H * HEIGHT)

#define MOV_PER_SEC 10
#define GRAVITY 0.1f

#define FPS 30
#define ONE_SEC 1000000
#define FRAME_PERIOD_US (ONE_SEC / FPS)


void
clear(block_t area[], block_t fill)
{
  size_t size = WIDTH * HEIGHT;
  while(size)
  {
    area[--size] = fill;
  }
}


void
draw_blocks(
  color_t * textures[],  // List of pointers to textures.
  block_t bg[],
  block_t terrain[],
  block_t fg[],
  uint32_t * pixels  // Output buffer
) {

  // Loop through all blocks
  for (size_t i = 0; i < WIDTH * HEIGHT; i++)
  {

    block_t current;
    if (fg[i] != EMPTY)
    {
      current = fg[i];
    }
    else if (terrain[i] != EMPTY)
    {
      current = terrain[i];
    }
    else
    {
      current = bg[i];
    }

    int block_pos = ((i / WIDTH) * WIDTH_P * BLOCK_H) +  // Number of pixels up to the current row
                    ((i % WIDTH) * BLOCK_W);  // Number of pixels in row up to block

    // Loop for every row of pixels in block height
    for (int dy = 0; dy < BLOCK_H; dy++)
    {
      memcpy(pixels + block_pos + (dy * WIDTH_P),
             textures[current] + (dy * BLOCK_W),
             BLOCK_W * sizeof(uint32_t));
    }
  }
}


void
draw_object(object_t * object, uint32_t * pixels)
{
  pixels += (object->pos.y * WIDTH_P) + object->pos.x;

  // Loop for every row of pixels in block height
  for (int dy = 0; dy < object->height; dy++)
  {
    memcpy(pixels + (dy * WIDTH_P),
           object->texture + (dy * object->width),
           object->width * sizeof(uint32_t));
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
setup_scene(block_t bg[], block_t terrain[], block_t fg[])
{
  int x, y;
  int ground = 2 * HEIGHT / 3;

  clear(bg, SKY);
  clear(terrain, EMPTY);
  clear(fg, EMPTY);

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

    set_block(terrain, GRASS, x, ground);
    for (y = ground + 1; y < HEIGHT; y++)
    {
      set_block(terrain, STONE, x, y);
    }
  }
}


void
get_input(vec2_t * delta, SDL_Event * event, uint64_t now, uint64_t * last_move)
{
  if (event->type == SDL_KEYDOWN)
  {
    int dist = (double)(now - *last_move) * ((float)MOV_PER_SEC / (float)ONE_SEC);
    if (dist < 1) {
      return;
    }

    if (event->key.keysym.sym == SDLK_UP)
    {
      delta->y += dist;
    }
    if (event->key.keysym.sym == SDLK_LEFT)
    {
      delta->x -= dist;
    }
    if (event->key.keysym.sym == SDLK_RIGHT)
    {
      delta->x += dist;
    }
  }

  // Update time if we haven't pressed, or if we have moved
  *last_move = now;
}


void
get_gravity(vec2_t * delta)
{
  delta->y += GRAVITY;
}


void
update_player(object_t * player, vec2_t * delta, block_t * terrain)
{
  player->pos.x += delta->x;
  player->pos.y += delta->y;
}


void
free_textures(color_t ** textures)
{
  free(textures[STONE]);
  free(textures[GRASS]);
  free(textures[PLAYER]);
  free(textures[SKY]);

  free(textures);
}


color_t **
load_textures()
{
  // Allocate array of pointers to textures
  color_t ** textures = (color_t **)malloc(NUM_BLOCKS * sizeof(color_t *));

  if (!(textures[STONE]  = load_raw("img/stone")) ||
      !(textures[GRASS]  = load_raw("img/grass")) ||
      !(textures[PLAYER] = load_raw("img/player")) ||
      !(textures[SKY]    = load_raw("img/sky"))) {

    free_textures(textures);
    return NULL;
  }
  return textures;
}


int
main(int argc, char * argv)
{
  color_t ** textures = load_textures();
  if (!textures) {
    return 1;
  }

  srand(time(NULL));
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window * window = SDL_CreateWindow("CCraft",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH_P, HEIGHT_P, 0);

  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Texture * texture = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, WIDTH_P, HEIGHT_P);

  // The pixel buffer
  uint32_t * pixels = (uint32_t *)malloc(WIDTH_P * HEIGHT_P * sizeof(uint32_t));

  // The blocks arrays
  block_t * bg      = (block_t *)malloc(WIDTH * HEIGHT * sizeof(block_t));
  block_t * terrain = (block_t *)malloc(WIDTH * HEIGHT * sizeof(block_t));
  block_t * fg      = (block_t *)malloc(WIDTH * HEIGHT * sizeof(block_t));
  setup_scene(bg, terrain, fg);

  object_t player = {
    textures[PLAYER],
    8, 8,
    {(WIDTH_P-8) / 2, 4}
  };

  printf("Starting\n");

  // For FPS timing
  uint64_t next_frame = get_us();

  // For average FPS measurement
  int frame_count = 0;
  uint64_t next_measure = 0;

  uint64_t last_move = get_us();

  int quit = 0;
  SDL_Event event;
  vec2_t delta;

  while (!quit)
  {
    uint64_t now = get_us();
    delta.x = 0;
    delta.y = 0;

    // Measure FPS
    if (now >= next_measure)
    {
      // If last measurement was more than 1 sec ago
      next_measure = now + ONE_SEC;
      printf("%f us/frame\n", (double)ONE_SEC / (double)frame_count);
      frame_count = 0;
    }

    // Render
    if (now >= next_frame)
    {
      next_frame = now + FRAME_PERIOD_US;
      frame_count++;

      draw_blocks(textures, bg, terrain, fg, pixels);
      draw_object(&player, pixels);

      SDL_UpdateTexture(texture, NULL, pixels, WIDTH_P * sizeof(uint32_t));

      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
    }

    // Get inputs
    SDL_PollEvent(&event);

    get_input(&delta, &event, now, &last_move);

    if (event.type == SDL_QUIT)
    {
      quit = 1;
      error("Quitting");
    }

    // Update player
    get_gravity(&delta);

    update_player(&player, &delta, terrain);
  }

  free(pixels);
  free(bg);
  free(terrain);
  free(fg);
  free_textures(textures);

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  printf("Finished\n");

  return 0;
}
