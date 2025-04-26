#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <string.h>

#define CHUNK_SIZE 4096
#define STEP 5

typedef struct {
  int x;
  int y;
  int w;
  int h;
  SDL_Texture* tex;
} entity;

// this struct basically inherits the entity struct and we
// can do a sort of polymorphism with typecasting because of it
typedef struct {
  int x;
  int y;
  int w;
  int h;
  SDL_Texture* tex;
  char up_k;
  char down_k;
  char right_k;
  char left_k;
} player_entity;

void draw_entity_to_buffer(SDL_Renderer* r, entity* e) {
    SDL_Rect a = {e->x, e->y, e->w, e->h};
    SDL_RenderCopy(r, e->tex, NULL, &a);
}

void initialize_entity_from_texture(int x, int y, SDL_Texture* t, entity* dest) {
  dest->x = x;
  dest->y = y;
  dest->tex = t;
  SDL_QueryTexture(t, NULL, NULL, &(dest->w), &(dest->h));
}
void initialize_player_entity_from_texture(int x, int y, SDL_Texture* t, player_entity* dest) {
  dest->x = x;
  dest->y = y;
  dest->tex = t;
  dest->up_k = 0;
  dest->down_k = 0;
  dest->right_k = 0;
  dest->left_k = 0;
  SDL_QueryTexture(t, NULL, NULL, &(dest->w), &(dest->h));
}

int main() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
  IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
  Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, CHUNK_SIZE);

  // open window 
  SDL_Renderer* renderer;
  SDL_Window* window;
  SDL_CreateWindowAndRenderer(800, 800, SDL_WINDOW_MAXIMIZED, &window, &renderer);

  // initialize player struct
  SDL_Surface* reimu_surface = IMG_Load("fumoreimu.png");
  SDL_Texture* reimu_tex = SDL_CreateTextureFromSurface(renderer, reimu_surface);
  SDL_FreeSurface(reimu_surface);
  player_entity player;
  initialize_player_entity_from_texture(0, 0, reimu_tex, &player);

  // main game loop
  SDL_Event event;
  do { 
    SDL_PollEvent(&event);

    // event handling, mainly input at the moment
    switch (event.type) {
      case SDL_KEYDOWN: 
        switch (event.key.keysym.sym) {
          case SDLK_w:
            player.up_k = 1;
            break;
          case SDLK_a:
            player.left_k = 1;
            break;
          case SDLK_s:
            player.down_k = 1;
            break;
          case SDLK_d:
            player.right_k = 1;
            break;
        }
        break;

      case SDL_KEYUP: 
        switch (event.key.keysym.sym) {
          case SDLK_w:
            player.up_k = 0;
            break;
          case SDLK_a:
            player.left_k = 0;
            break;
          case SDLK_s:
            player.down_k = 0;
            break;
          case SDLK_d:
            player.right_k = 0;
            break;
        }
        break;
    }

    // updating player position
    player.x += (player.right_k - player.left_k) * STEP;
    player.y -= (player.up_k - player.down_k) * STEP;

    SDL_RenderClear(renderer);
    draw_entity_to_buffer(renderer, (entity*) &player);
    SDL_RenderPresent(renderer);

    SDL_Delay(1000/60); // temporary solution for limiting frame rate
  } while (event.type != SDL_QUIT);

  SDL_DestroyTexture(reimu_tex);
  IMG_Quit();
  Mix_Quit();
  SDL_Quit();
}
