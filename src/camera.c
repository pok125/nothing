#include <SDL2/SDL.h>

#include <math.h>
#include <assert.h>

#include "./camera.h"
#include "./error.h"
#include "./renderer.h"

#define RATIO_X 16.0f
#define RATIO_Y 9.0f

struct camera_t {
    int debug_mode;
    int blackwhite_mode;
    point_t position;
};

static vec_t effective_ratio(const SDL_Rect *view_port);
static vec_t effective_scale(const SDL_Rect *view_port);
static vec_t camera_point(const camera_t *camera,
                          const SDL_Rect *view_port,
                          const vec_t p);
static rect_t camera_rect(const camera_t *camera,
                          const SDL_Rect *view_port,
                          const rect_t rect);
static triangle_t camera_triangle(const camera_t *camera,
                                  const SDL_Rect *view_port,
                                  const triangle_t t);

camera_t *create_camera(point_t position)
{
    camera_t *camera = malloc(sizeof(camera_t));

    if (camera == NULL) {
        throw_error(ERROR_TYPE_LIBC);
        return NULL;
    }

    camera->position = position;
    camera->debug_mode = 0;
    camera->blackwhite_mode = 0;

    return camera;
}

void destroy_camera(camera_t *camera)
{
    assert(camera);

    free(camera);
}


int camera_fill_rect(const camera_t *camera,
                     SDL_Renderer *render,
                     rect_t rect,
                     color_t color)
{
    assert(camera);
    assert(render);

    SDL_Rect view_port;
    SDL_RenderGetViewport(render, &view_port);

    const SDL_Rect sdl_rect = rect_for_sdl(
        camera_rect(camera, &view_port, rect));

    const SDL_Color sdl_color = color_for_sdl(camera->blackwhite_mode ? color_desaturate(color) : color);

    if (SDL_SetRenderDrawColor(render, sdl_color.r, sdl_color.g, sdl_color.b, sdl_color.a) < 0) {
        throw_error(ERROR_TYPE_SDL2);
        return -1;
    }

    if (camera->debug_mode) {
        if (SDL_RenderDrawRect(render, &sdl_rect) < 0) {
            throw_error(ERROR_TYPE_SDL2);
            return -1;
        }
    } else {
        if (SDL_RenderFillRect(render, &sdl_rect) < 0) {
            throw_error(ERROR_TYPE_SDL2);
            return -1;
        }
    }

    return 0;
}

int camera_draw_rect(const camera_t * camera,
                     SDL_Renderer *render,
                     rect_t rect,
                     color_t color)
{
    assert(camera);
    assert(render);

    SDL_Rect view_port;
    SDL_RenderGetViewport(render, &view_port);

    const SDL_Rect sdl_rect = rect_for_sdl(
        camera_rect(camera, &view_port, rect));

    const SDL_Color sdl_color = color_for_sdl(camera->blackwhite_mode ? color_desaturate(color) : color);

    if (SDL_SetRenderDrawColor(render, sdl_color.r, sdl_color.g, sdl_color.b, sdl_color.a) < 0) {
        throw_error(ERROR_TYPE_SDL2);
        return -1;
    }

    if (SDL_RenderDrawRect(render, &sdl_rect) < 0) {
        throw_error(ERROR_TYPE_SDL2);
        return -1;
    }

    return 0;
}

int camera_draw_triangle(const camera_t *camera,
                         SDL_Renderer *render,
                         triangle_t t,
                         color_t color)
{
    assert(camera);
    assert(render);

    SDL_Rect view_port;
    SDL_RenderGetViewport(render, &view_port);

    const SDL_Color sdl_color = color_for_sdl(camera->blackwhite_mode ? color_desaturate(color) : color);

    if (SDL_SetRenderDrawColor(render, sdl_color.r, sdl_color.g, sdl_color.b, sdl_color.a) < 0) {
        throw_error(ERROR_TYPE_SDL2);
        return -1;
    }

    if (draw_triangle(render, camera_triangle(camera, &view_port, t)) < 0) {
        return -1;
    }

    return 0;
}

int camera_fill_triangle(const camera_t *camera,
                         SDL_Renderer *render,
                         triangle_t t,
                         color_t color)
{
    assert(camera);
    assert(render);

    SDL_Rect view_port;
    SDL_RenderGetViewport(render, &view_port);

    const SDL_Color sdl_color = color_for_sdl(camera->blackwhite_mode ? color_desaturate(color) : color);

    if (SDL_SetRenderDrawColor(render, sdl_color.r, sdl_color.g, sdl_color.b, sdl_color.a) < 0) {
        throw_error(ERROR_TYPE_SDL2);
        return -1;
    }

    if (fill_triangle(render, camera_triangle(camera, &view_port, t)) < 0) {
        return -1;
    }

    return 0;
}

void camera_center_at(camera_t *camera, point_t position)
{
    assert(camera);
    camera->position = position;
}

void camera_toggle_debug_mode(camera_t *camera)
{
    assert(camera);
    camera->debug_mode = !camera->debug_mode;
}

void camera_toggle_blackwhite_mode(camera_t *camera)
{
    assert(camera);
    camera->blackwhite_mode = !camera->blackwhite_mode;
}

/* ---------- Private Function ---------- */

static vec_t effective_ratio(const SDL_Rect *view_port)
{
    if ((float) view_port->w / RATIO_X > (float) view_port->h / RATIO_Y) {
        return vec(RATIO_X, (float) view_port->h / ((float) view_port->w / RATIO_X));
    } else {
        return vec((float) view_port->w / ((float) view_port->h / RATIO_Y), RATIO_Y);
    }
}

static vec_t effective_scale(const SDL_Rect *view_port)
{
    return vec_entry_div(
        vec((float) view_port->w, (float) view_port->h),
        vec_scala_mult(effective_ratio(view_port), 50.0f));
}

static vec_t camera_point(const camera_t *camera,
                          const SDL_Rect *view_port,
                          const vec_t p)

{
    return vec_sum(
        vec_entry_mult(
            vec_sum(p, vec_neg(camera->position)),
            effective_scale(view_port)),
        vec((float) view_port->w * 0.5f,
            (float) view_port->h * 0.5f));
}

static triangle_t camera_triangle(const camera_t *camera,
                                  const SDL_Rect *view_port,
                                  const triangle_t t)
{
    return triangle(
        camera_point(camera, view_port, t.p1),
        camera_point(camera, view_port, t.p2),
        camera_point(camera, view_port, t.p3));
}

static rect_t camera_rect(const camera_t *camera,
                          const SDL_Rect *view_port,
                          const rect_t rect)
{
    return rect_from_vecs(
        camera_point(
            camera,
            view_port,
            vec(rect.x, rect.y)),
        vec_entry_mult(
            effective_scale(view_port),
            vec(rect.w, rect.h)));
}
