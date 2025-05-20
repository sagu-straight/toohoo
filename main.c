#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <stdlib.h>
#include <time.h>
#include "entity.h"
#include "ll.h"
#include "rand.h"

#define CHUNK_SIZE 4096 // sdl mixer stuff
#define STEP 8 // player step size

char* REIMU_IMG = "fumoreimu.png";
char* CIRNO_IMG = "fumocirno.png";
char* BULLET_IMG = "fumookuustar1.png";
char* GOHEI_IMG = "fumoreimugohei1.png";

list enemy_list;
list enemy_bullet_list;
list player_bullet_list;

SDL_Texture* cirno_tex;
SDL_Texture* reimu_tex;
SDL_Texture* bullet_tex;

int window_height = 800;
int window_width = 800;

int main() {
  // inits
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
  IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
  Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, CHUNK_SIZE);
  srand(time(NULL)); // sets random seed to system time

  // open window 
  SDL_Renderer* renderer;
  SDL_Window* window;
  SDL_CreateWindowAndRenderer(window_width, window_height, 0, &window, &renderer);

  // initialize enemy texture
  SDL_Surface* cirno_surface = IMG_Load(CIRNO_IMG);
  cirno_tex = SDL_CreateTextureFromSurface(renderer, cirno_surface);
  SDL_FreeSurface(cirno_surface);

  // initialize enemy bullet texture
  SDL_Surface* bullet_surface = IMG_Load(BULLET_IMG);
  bullet_tex = SDL_CreateTextureFromSurface(renderer, bullet_surface);
  SDL_FreeSurface(bullet_surface);

  SDL_Surface* gohei_surface = IMG_Load(GOHEI_IMG);
  SDL_Texture* gohei_tex = SDL_CreateTextureFromSurface(renderer, gohei_surface);
  SDL_FreeSurface(gohei_surface);

  // load baka.wav
  Mix_Chunk* baka = Mix_LoadWAV("baka.wav");

  // initialize player struct and texture
  SDL_Surface* reimu_surface = IMG_Load(REIMU_IMG);
  reimu_tex = SDL_CreateTextureFromSurface(renderer, reimu_surface);
  SDL_FreeSurface(reimu_surface);
  player_entity player;
  int middle_of_screen;
  middle_of_screen =  window_width/ 2;
  initialize_player_entity_from_texture(middle_of_screen, window_height, reimu_tex, &player);
  player.y -= player.h + 100; // lazy fix so she doesnt spawn off-screen

  // initialize lists
  initialize_list(&enemy_list);
  initialize_list(&enemy_bullet_list);
  initialize_list(&player_bullet_list);

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
          case SDLK_LSHIFT:
          case SDLK_RSHIFT:
            player.shift_k = 1;
            break;
          case SDLK_j:
            player.fire_k = 1;
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
          case SDLK_LSHIFT:
          case SDLK_RSHIFT:
            player.shift_k = 0;
            break;
          case SDLK_j:
            player.fire_k = 0;
            break;
        }
        break;
    }

    // updating player 
    player.cooldown--;
    int new_p_x = player.x + (player.right_k - player.left_k) * (STEP >> (player.shift_k));
    int new_p_y = player.y - (player.up_k - player.down_k) * (STEP >> (player.shift_k));
    if (new_p_x + player.w < window_width && new_p_x > 0)
      player.x = new_p_x;
    if (new_p_y + player.h < window_height && new_p_y > 0)
      player.y = new_p_y;

    // create player bullets
    if (player.fire_k && player.cooldown <= 0) {
      bullet_entity* b = malloc(sizeof(bullet_entity));
      initialize_bullet_entity_from_texture(player.x, player.y,
                                            0, 10, gohei_tex, linear_bullet_update, b);
      add_node_to_list(&player_bullet_list, b);
      player.cooldown = PLAYER_BULLET_COOLDOWN;
    }

    // update player bullets
    node* aux = player_bullet_list.start;
    while (aux != NULL) {
      node* next = aux->next;
      bullet_entity* aux1 = (bullet_entity*) aux->e;

      aux1->update_position(aux1);

      // if the bullet is out of bounds, delete the fella
      if (aux1->y > window_height || aux1->y + aux1->h < ENEMY_SPAWN_LINE ||
          aux1->x + aux1->w < 0 || aux1->x > window_width) {
        remove_node_from_list(&player_bullet_list, aux);
        free(aux);
        free(aux1);
      }

      aux = next;
    }

    // TODO: update and/or generate enemies
    // DEMO enemy generation
    if (rand() < RAND_MAX/10) {
      enemy_entity* enemy = make_simple_enemy(cirno_tex);
      add_node_to_list(&enemy_list, enemy);
      Mix_PlayChannel(-1, baka, 0);
    }

    // enemy updating
    aux = enemy_list.start;
    while (aux != NULL) {
      node* next = aux->next;
      enemy_entity* aux1 = (enemy_entity*) aux->e;

      aux1->update_position(aux1);

      // if the enemy is out of bounds, delete the fella
      // allows fellas to exist off the window to the side and above, but not below
      if (aux1->y > window_height) {
        remove_node_from_list(&enemy_list, aux);
        free(aux);
        if (aux1->data != 0)
          free(aux1->data);
        free(aux1);
      }

      aux = next;
    }

    // enemy bullet updates
    aux = enemy_bullet_list.start;
    while (aux != NULL) {
      node* next = aux->next;
      bullet_entity* aux1 = (bullet_entity*) aux->e;

      aux1->update_position(aux1);

      // if the bullet is out of bounds, delete the fella
      if (aux1->y > window_height || aux1->y + aux1->h < ENEMY_SPAWN_LINE ||
          aux1->x + aux1->w < 0 || aux1->x > window_width) {
        remove_node_from_list(&enemy_bullet_list, aux);
        free(aux);
        free(aux1);
      }

      aux = next;
    }

    // TODO: check and handle collisions

    // player rendering
    SDL_RenderClear(renderer);
    draw_entity_to_buffer(renderer, (entity*) &player);

    // TODO render player bullets
    for (node* aux = player_bullet_list.start; aux != NULL; aux = aux->next) {
      draw_entity_to_buffer(renderer, aux->e);
    }
    
    // enemy rendering
    for (node* aux = enemy_list.start; aux != NULL; aux = aux->next) {
      draw_entity_to_buffer(renderer, aux->e);
    }
    // enemy bullet rendering
    for (node* aux = enemy_bullet_list.start; aux != NULL; aux = aux->next) {
      draw_entity_to_buffer(renderer, aux->e);
    }
    
    
    // finish rendering
    SDL_RenderPresent(renderer);

    SDL_Delay(1000/60); // temporary solution for limiting frame rate
  } while (event.type != SDL_QUIT);

  //quits and frees
  SDL_DestroyTexture(reimu_tex);
  SDL_DestroyTexture(cirno_tex);
  IMG_Quit();
  Mix_Quit();
  SDL_Quit();
}
