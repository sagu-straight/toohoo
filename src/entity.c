#include "../headers/entity.h"
#include <SDL2/SDL_rect.h>
#include <math.h>
#include <stdlib.h>
#include "../headers/ll.h"
#include "../headers/rand.h"

int rand_range(int min, int max) {
  return min+(rand()%(max - min));
}

void draw_entity_to_buffer(SDL_Renderer* r, entity* e) {
  SDL_Rect a = {e->x, e->y, e->w, e->h};
  SDL_RenderCopy(r, e->tex, NULL, &a);
}

int rectangles_collide(SDL_Rect a, SDL_Rect b) {
    if (a.x + a.w <= b.x) return 0; 
    if (a.x >= b.x + b.w) return 0; 
    if (a.y + a.h <= b.y) return 0; 
    if (a.y >= b.y + b.h) return 0; 

    return 1;
}

void initialize_entity_from_texture(float x, float y, SDL_Texture* t, entity* dest) {
  dest->x = x;
  dest->y = y;
  dest->tex = t;
  SDL_QueryTexture(t, NULL, NULL, &(dest->w), &(dest->h));
}

void initialize_player_entity_from_texture(float x, float y, SDL_Texture* t, player_entity* dest) {
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

void initialize_enemy_entity_from_texture(float x, float y, SDL_Texture* t, int health, enemy_entity* dest, void (*update) (enemy_entity*), int buff_size) {
  dest->x = x;
  dest->y = y;
  dest->tex = t;
  dest->health = health;
  SDL_QueryTexture(t, NULL, NULL, &(dest->w), &(dest->h));
  dest->update_position = update;
  dest->data_size = buff_size;
  if (buff_size > 0)
    dest->data = malloc(buff_size);
  else
    dest->data = 0;
}

void linear_bullet_update(bullet_entity* b);

void initialize_bullet_entity_from_texture(float x, float y, float dx, float dy, SDL_Texture* t, void (*update) (bullet_entity*), bullet_entity* dest) {
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

void simple_enemy_update(enemy_entity* e) {
  static int step = 4;

  e->y += step;

  *(int*) e->data -= 1;
  if (*(int*) e->data > 0) return;

  int b_xspeed = rand_range(-7, 7);
  int b_yspeed = -12;

  bullet_entity* b = malloc(sizeof(bullet_entity));
  initialize_bullet_entity_from_texture(e->x, e->y, b_xspeed, b_yspeed, flake_tex, linear_bullet_update, b);
  b->h = b->w = 35;
  add_node_to_list(&enemy_bullet_list, b);
  *(int*) e->data = SIMPLE_ENEMY_COOLDOWN;
}

enemy_entity* make_simple_enemy(SDL_Texture* tex) {
      enemy_entity* enemy = malloc(sizeof(enemy_entity));
      initialize_enemy_entity_from_texture(rand_range(0, window_width),
                                           ENEMY_SPAWN_LINE, tex, 3, enemy, simple_enemy_update, sizeof(int));
      *(int*) enemy->data = SIMPLE_ENEMY_COOLDOWN;
      enemy->x = enemy->x + enemy->w >= window_width ? window_width - enemy->w : enemy->x;
      return enemy;
}


enum boss_state {
  DRAMATIC_ENTRANCE,
  ATTACK,
};

struct boss_data {
  enum boss_state state;
  int cooldown;
};

#define BOSS_COOLDOWN 30
#define BOSS_HEALTH 2000
#define BOSS_BULLET_SPEED 3

extern player_entity player;
extern SDL_Texture* star_tex;
void boss_update(enemy_entity* e) {
  struct boss_data* data = e->data;
  int tx;
  int ty;
  switch (data->state) {
    case DRAMATIC_ENTRANCE:
      e->y += 1;
      if (e->y >= ENEMY_SPAWN_LINE + 200)
        data->state = ATTACK;
      break;
    case ATTACK:
      tx = (e->x + e->w) - (player.x + player.w);
      ty = (e->y + e->h) - (player.y + player.h);
      if (data->cooldown <= 0) {
        bullet_entity* b = malloc(sizeof(bullet_entity));
        float vec_len = sqrt(tx*tx + ty*ty);
        float ftx = tx / vec_len;
        float fty = ty / vec_len;
        initialize_bullet_entity_from_texture(e->x, e->y,
           -ftx * BOSS_BULLET_SPEED, fty * BOSS_BULLET_SPEED, star_tex, linear_bullet_update, b);
        add_node_to_list(&enemy_bullet_list, b);
        data->cooldown = BOSS_COOLDOWN;
      } else data->cooldown--;
      break;
  }
}

enemy_entity* make_boss(SDL_Texture* tex) {
  enemy_entity* boss = malloc(sizeof(enemy_entity));
  initialize_enemy_entity_from_texture((float) window_width / 2 - 32, ENEMY_SPAWN_LINE, tex, BOSS_HEALTH,
                                        boss, boss_update, sizeof(struct boss_data));

  struct boss_data data;
  data.state = DRAMATIC_ENTRANCE;
  data.cooldown = BOSS_COOLDOWN;

  *(struct boss_data*) boss->data = data;

  return boss;
}


