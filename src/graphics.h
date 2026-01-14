#ifndef GRAPHICS_H
#define GRAPHICS_H

#ifdef _WIN32
    #include <SDL.h>
    #include <SDL_image.h>
    #include <SDL_ttf.h>
#else
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_ttf.h>
#endif

#include <stdbool.h>
#include "config.h"

typedef struct {
    SDL_Texture* texture;
    SDL_Surface* surface;
    int width;
    int height;
} Image;

bool graphics_init(SDL_Window** window, SDL_Renderer** renderer, int width, int height);
void graphics_cleanup(SDL_Window* window, SDL_Renderer* renderer);


Image* image_load(SDL_Renderer* renderer, const char* filename);
void image_free(Image* image);
void image_render(SDL_Renderer* renderer, Image* image, int x, int y);
void image_render_centered(SDL_Renderer* renderer, Image* image, float custom_scale);
bool image_save(Image* image, const char* filename);
void image_draw_point(Image* image, SDL_Renderer* renderer, int x, int y, Uint32 color);
void image_update_texture(SDL_Renderer* renderer, Image* image);

// Advanced Filters
void image_apply_grayscale(Image* image);
void image_apply_sepia(Image* image);
void image_apply_invert(Image* image);


void render_clear(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b);
void render_present(SDL_Renderer* renderer);

#endif