#include "entity.h"

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
