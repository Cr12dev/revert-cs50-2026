#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

typedef struct {
    SDL_Texture* texture;
    int width;
    int height;
} Image;

bool graphics_init(SDL_Window** window, SDL_Renderer** renderer, int width, int height);
void graphics_cleanup(SDL_Window* window, SDL_Renderer* renderer);


Image* image_load(SDL_Renderer* renderer, const char* filename);
void image_free(Image* image);
void image_render(SDL_Renderer* renderer, Image* image, int x, int y);
void image_render_centered(SDL_Renderer* renderer, Image* image);


void render_clear(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b);
void render_present(SDL_Renderer* renderer);

#endif