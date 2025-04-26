#ifndef __ENTITY__
#define __ENTITY__

#include <SDL2/SDL.h>

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
} player_entity;

void draw_entity_to_buffer(SDL_Renderer* r, entity* e);

void initialize_entity_from_texture(int x, int y, SDL_Texture* t, entity* dest);
void initialize_player_entity_from_texture(int x, int y, SDL_Texture* t, player_entity* dest);
#endif
