#ifndef PLAYER_H_
#define PLAYER_H_

#include "./goals.h"
#include "./lava.h"
#include "./camera.h"

typedef struct player_t player_t;
typedef struct platforms_t platforms_t;
typedef struct SDL_Renderer SDL_Renderer;

player_t *create_player(float x, float y, color_t color);
player_t *create_player_from_stream(FILE *stream);
void destroy_player(player_t * player);

int player_render(const player_t * player,
                  SDL_Renderer *renderer,
                  const camera_t *camera);
void player_update(player_t * player,
                   const platforms_t *platforms,
                   Uint32 delta_time);

void player_move_left(player_t *player);
void player_move_right(player_t *player);
void player_stop(player_t *player);
void player_jump(player_t *player);
void player_die(player_t *player);

void player_focus_camera(player_t *player,
                         camera_t *camera);
void player_hide_goals(const player_t *player,
                       goals_t *goal);
void player_die_from_lava(player_t *player,
                          const lava_t *lava);

#endif  // PLAYER_H_
