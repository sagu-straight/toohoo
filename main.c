#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include "entity.h"
#include "ll.h"

#define CHUNK_SIZE 4096 // sdl mixer stuff
#define STEP 5 // player step size


int main() {
  // inits
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
  IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
  Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, CHUNK_SIZE);

  // open window 
  SDL_Renderer* renderer;
  SDL_Window* window;
  SDL_CreateWindowAndRenderer(800, 800, SDL_WINDOW_MAXIMIZED, &window, &renderer);

  // initialize enemy texture
  SDL_Surface* cirno_surface = IMG_Load("fumocirno.png");
  SDL_Texture* cirno_tex = SDL_CreateTextureFromSurface(renderer, cirno_surface);
  SDL_FreeSurface(cirno_surface);

  // initialize player struct and texture
  SDL_Surface* reimu_surface = IMG_Load("fumoreimu.png");
  SDL_Texture* reimu_tex = SDL_CreateTextureFromSurface(renderer, reimu_surface);
  SDL_FreeSurface(reimu_surface);
  player_entity player;
  int middle_of_screen;
  int bottom_of_screen;
  SDL_GetWindowSize(window, &middle_of_screen, &bottom_of_screen);
  middle_of_screen /= 2;
  initialize_player_entity_from_texture(middle_of_screen - player.w/2, bottom_of_screen, reimu_tex, &player);
  player.y -= player.h; // lazy fix so she doesnt spawn off-screen

  // initialize enemy structures 
  list enemy_list;
  initialize_list(&enemy_list);

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

    // TODO: update and/or generate enemies

    // TODO: update and/or generate bullets


    // player rendering
    SDL_RenderClear(renderer);
    draw_entity_to_buffer(renderer, (entity*) &player);
    
    // TODO: render enemies and bullets
    
    // finish rendering
    SDL_RenderPresent(renderer);

    SDL_Delay(1000/60); // temporary solution for limiting frame rate
  } while (event.type != SDL_QUIT);

  //quits and frees
  SDL_DestroyTexture(reimu_tex);
  IMG_Quit();
  Mix_Quit();
  SDL_Quit();
}
