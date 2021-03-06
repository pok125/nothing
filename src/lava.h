#ifndef LAVA_H_
#define LAVA_H_

#include "./camera.h"
#include "./rect.h"

typedef struct lava_t lava_t;

lava_t *create_lava_from_stream(FILE *stream);
void destroy_lava(lava_t *lava);

int lava_render(const lava_t *lava,
                SDL_Renderer *renderer,
                const camera_t *camera);
int lava_update(lava_t *lava, Uint32 delta_time);

int lava_overlaps_rect(const lava_t *lava,
                       rect_t rect);

#endif  // LAVA_H_
