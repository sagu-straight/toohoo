#ifndef __ENTITY__
#define __ENTITY__

#include <SDL2/SDL.h>
#include "ll.h"

extern list enemy_bullet_list;

extern SDL_Texture* bullet_tex;

// struct that describes any entity in the game
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
  char shift_k;
} player_entity;

// this also inherits entity
typedef struct enemy_struct enemy_entity;
typedef struct enemy_struct {
  int x;
  int y;
  int w;
  int h;
  SDL_Texture* tex;
  void (*update_position) (enemy_entity*);
  // data buffer to be allocated on enemy creation, so that each enemy
  // can have an arbitrary amount of data it can work with for its ai
  void* data; 
} enemy_entity;

// this also inherits entity
typedef struct bullet_struct bullet_entity;
typedef struct bullet_struct {
  int x;
  int y;
  int w;
  int h;
  SDL_Texture* tex;
  void (*update_position) (bullet_entity*);
  int dx;
  int dy;
  int t; // may be unused but i have some ideas for it
} bullet_entity;


void draw_entity_to_buffer(SDL_Renderer* r, entity* e);

void random_enemy_update(enemy_entity* e);

void initialize_entity_from_texture(int x, int y, SDL_Texture* t, entity* dest);
void initialize_enemy_entity_from_texture(int x, int y, SDL_Texture* t, enemy_entity* dest, void (*update) (enemy_entity*), int buff_size);
void initialize_player_entity_from_texture(int x, int y, SDL_Texture* t, player_entity* dest);
void initialize_bullet_entity_from_texture(int x, int y, int dx, int dy, SDL_Texture* t, void (*update) (bullet_entity*), bullet_entity* dest);

#endif
