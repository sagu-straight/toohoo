#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_ttf.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../headers/entity.h"
#include "../headers/ll.h"
#include "../headers/rand.h"

#define CHUNK_SIZE 4096 // sdl mixer stuff
#define STEP 8 // player step size
#define HIT_PENALTY 10
#define HIT_BONUS 50
#define KILL_BONUS 200
#define HITLESS_BONUS 10000
#define REIMU_HITBOX_R 3
#define PRE_BOSS_CIRNO 15

char* REIMU_IMG = "assets/fumoreimu.png";
char* CIRNO_IMG = "assets/fumocirno.png";
char* OKUU_IMG  = "assets/fumookuu.png"; 
char* STAR_IMG  = "assets/fumookuustar1.png";
char* FLAKE_IMG = "assets/snowflake.png";
char* GOHEI_IMG = "assets/fumoreimugohei1.png";
char* BACKGROUND_IMG = "assets/background.jpg";
char* FONT = "assets/lepidos.ttf";

list enemy_list;
list enemy_bullet_list;
list player_bullet_list;

player_entity player;

SDL_Texture* cirno_tex;
SDL_Texture* reimu_tex;
SDL_Texture* okuu_tex;
SDL_Texture* star_tex;
SDL_Texture* flake_tex;

int window_height = 800;
int window_width = 500;


int main() {
  // inits
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
  IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
  TTF_Init();
  Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, CHUNK_SIZE);
  srand(time(NULL)); // sets random seed to system time

  int score = 0;
  int cirno_spawn_count = 0;
  int okuu_spawned = 0;
  int hitless = 1;

  // open window 
  SDL_Renderer* renderer;
  SDL_Window* window;
  SDL_CreateWindowAndRenderer(window_width, window_height, 0, &window, &renderer);

  // initialize background texture
  SDL_Surface* background_surface = IMG_Load(BACKGROUND_IMG);
  SDL_Texture* background_tex = SDL_CreateTextureFromSurface(renderer, background_surface);
  SDL_FreeSurface(background_surface);

  // initialize enemy texture
  SDL_Surface* cirno_surface = IMG_Load(CIRNO_IMG);
  cirno_tex = SDL_CreateTextureFromSurface(renderer, cirno_surface);
  SDL_FreeSurface(cirno_surface);

  // initialize boss texture
  SDL_Surface* okuu_surface = IMG_Load(OKUU_IMG);
  okuu_tex = SDL_CreateTextureFromSurface(renderer, okuu_surface);
  SDL_FreeSurface(okuu_surface);

  // initialize enemy bullet texture
  SDL_Surface* bullet_surface = IMG_Load(FLAKE_IMG);
  flake_tex = SDL_CreateTextureFromSurface(renderer, bullet_surface);
  SDL_FreeSurface(bullet_surface);

  // initialize boss bullet texture
  SDL_Surface* star_surface = IMG_Load(STAR_IMG);
  star_tex = SDL_CreateTextureFromSurface(renderer, star_surface);
  SDL_FreeSurface(star_surface);

  // initialize gohei texture (player bullets)
  SDL_Surface* gohei_surface = IMG_Load(GOHEI_IMG);
  SDL_Texture* gohei_tex = SDL_CreateTextureFromSurface(renderer, gohei_surface);
  SDL_FreeSurface(gohei_surface);

  // load baka.wav
  Mix_Chunk* baka = Mix_LoadWAV("assets/baka.wav");

  // load font
  TTF_Font* font = TTF_OpenFont(FONT, 500);

  // initialize player struct and texture
  SDL_Surface* reimu_surface = IMG_Load(REIMU_IMG);
  reimu_tex = SDL_CreateTextureFromSurface(renderer, reimu_surface);
  SDL_FreeSurface(reimu_surface);
  int middle_of_screen =  window_width/ 2;
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

    // cirno generation
    if (cirno_spawn_count < PRE_BOSS_CIRNO) {
      if (rand_range(1, 100) <= 3) {
        cirno_spawn_count++;
        enemy_entity* enemy = make_simple_enemy(cirno_tex);
        add_node_to_list(&enemy_list, enemy);
        Mix_PlayChannel(-1, baka, 0);
      }
    } else if (!okuu_spawned) {
      enemy_entity* okuu = make_boss(okuu_tex);
      add_node_to_list(&enemy_list, okuu);
      okuu_spawned = 1;
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
        if (aux1->data_size > 0)
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

    // reimu's hitbox is made smaller than the sprite kind of like in real touhou
    SDL_Rect player_rect = {player.x + player.w/2.0 - REIMU_HITBOX_R, player.y + player.h/2.0 - REIMU_HITBOX_R, REIMU_HITBOX_R, REIMU_HITBOX_R};

    // check and handle collisions
    // player to enemy bullets
    aux = enemy_bullet_list.start;
    while (aux != NULL) {
      node* next = aux->next;

      bullet_entity* aux1 = aux->e;
      SDL_Rect ebullet_rect = {aux1->x, aux1->y, aux1->w, aux1->h};
      if (rectangles_collide(player_rect, ebullet_rect)) {
        score -= HIT_PENALTY;
        hitless = 0;
        remove_node_from_list(&enemy_bullet_list, aux);
        free(aux1);
        free(aux);
      }

      aux = next;
    }
    // player to enemies
    for (node* aux = enemy_list.start; aux != NULL; aux = aux->next) {
      enemy_entity* aux1 = aux->e;
      SDL_Rect enemy_rect = {aux1->x, aux1->y, aux1->w, aux1->h};
      if (rectangles_collide(player_rect, enemy_rect)) {
        score -= HIT_PENALTY;
        hitless = 0;
      }
    }
    // player bullets to enemies (most computationaly expensive one O(enemies * bullets))
    node* pbaux = player_bullet_list.start;
    node* eaux;
    while (pbaux != NULL) {
      eaux = enemy_list.start;
      node* next_pb = pbaux->next;
      while (eaux != NULL) {
        node* next_e = eaux->next;

        bullet_entity* pb = pbaux->e;
        enemy_entity* e = eaux->e;
        SDL_Rect pb_rect = {pb->x, pb->y, pb->w, pb->h};
        SDL_Rect e_rect = {e->x, e->y, e->w, e->h};
        if (rectangles_collide(pb_rect, e_rect)) {
          remove_node_from_list(&player_bullet_list, pbaux);
          e->health -= 1;
          score += HIT_BONUS;
          if (e->health <= 0) {
            score += KILL_BONUS;
            if (e->update_position == boss_update) {
              score += 10000;
              score += hitless * 20000;
            }
            remove_node_from_list(&enemy_list, eaux);
            free(eaux);
            if (e->data_size > 0)
              free(e->data);
            free(e);
          }

          free(pbaux);
          free(pb);
          break; // this bullet is now invalid so we skip to the next one (sneaky to debug arrrgh)
        }
        eaux = next_e;
      }
      pbaux = next_pb;
    }


    // ------ begin rendering ------

    // SDL_RenderClear(renderer);
    SDL_Rect a = {0, 0, window_width, window_height};
    SDL_RenderCopy(renderer, background_tex, NULL, &a);

    // player rendering
    draw_entity_to_buffer(renderer, (entity*) &player);
    // SDL_RenderDrawRect(renderer, &player_rect);

    // player bullet rendering
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

    // UI rendering
    SDL_Color white = {255,255,255};
    char text[16]; sprintf(text, "%d", score);
    SDL_Surface* text_surface = TTF_RenderText_Solid(font, text, white);
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_Rect text_rect = {0, 0, strlen(text) * 50, window_height / 10};
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
    
    SDL_RenderPresent(renderer);
    // ------ end rendering ------

    SDL_Delay(1000/60); // temporary solution for limiting frame rate
  } while (event.type != SDL_QUIT);

  //quits and frees
  SDL_DestroyTexture(reimu_tex);
  SDL_DestroyTexture(cirno_tex);
  TTF_Quit();
  IMG_Quit();
  Mix_Quit();
  SDL_Quit();
}
