#include "entity.h"
#include <stdlib.h>
#include "ll.h"
#include "rand.h"

int rand_range(int min, int max) {
  return min+(rand()%(max - min));
}

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
  dest->shift_k = 0;
  dest->fire_k = 0;
  dest->cooldown = PLAYER_BULLET_COOLDOWN;
  SDL_QueryTexture(t, NULL, NULL, &(dest->w), &(dest->h));
}

void initialize_enemy_entity_from_texture(int x, int y, SDL_Texture* t, enemy_entity* dest, void (*update) (enemy_entity*), int buff_size) {
  dest->x = x;
  dest->y = y;
  dest->tex = t;
  SDL_QueryTexture(t, NULL, NULL, &(dest->w), &(dest->h));
  dest->update_position = update;
  if (buff_size > 0)
    dest->data = malloc(buff_size);
  else
    dest->data = 0;
}

void linear_bullet_update(bullet_entity* b);

void initialize_bullet_entity_from_texture(int x, int y, int dx, int dy, SDL_Texture* t, void (*update) (bullet_entity*), bullet_entity* dest) {
  dest->x = x;
  dest->y = y;
  dest->dx = dx;
  dest->dy = dy;
  dest->tex = t;
  dest->t = 0;
  SDL_QueryTexture(t, NULL, NULL, &(dest->w), &(dest->h));
  dest->update_position = update;
}

void linear_bullet_update(bullet_entity* b) {
  b->x += b->dx;
  b->y -= b->dy;
}

#define SIMPLE_ENEMY_COOLDOWN 30 // abt 2 bullets p/ sec
void simple_enemy_update(enemy_entity* e) {
  static int step = 8;

  e->y += step;

  *(int*) e->data -= 1;
  if (*(int*) e->data > 0) return;

  int b_xspeed = rand_range(-7, 7);
  int b_yspeed = -12;

  bullet_entity* b = malloc(sizeof(bullet_entity));
  initialize_bullet_entity_from_texture(e->x, e->y, b_xspeed, b_yspeed, bullet_tex, linear_bullet_update, b);
  add_node_to_list(&enemy_bullet_list, b);
  *(int*) e->data = SIMPLE_ENEMY_COOLDOWN;
}

enemy_entity* make_simple_enemy(SDL_Texture* tex) {
      enemy_entity* enemy = malloc(sizeof(enemy_entity));
      initialize_enemy_entity_from_texture(rand_range(0, window_width),
                                           ENEMY_SPAWN_LINE, tex, enemy, simple_enemy_update, sizeof(int));
      *(int*) enemy->data = SIMPLE_ENEMY_COOLDOWN;
      return enemy;
}
