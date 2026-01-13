#include "graphics.h"
#include <stdio.h>



bool graphics_init(SDL_Window** window, SDL_Renderer** renderer, int width, int height){
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error al iniciar el SDL: %s\n", SDL_GetError());
        return false;
    }

    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("Error al iniciar SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return false;
    }


    *window = SDL_CreateWindow(
        "REVERT V.1.0.0",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );

    if (!*window) {
        printf("Error al crear la ventana: %s\n", SDL_GetError());
        return false;
    }

    *renderer = SDL_CreateRenderer(
        *window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!*renderer) {
        printf("Error añ crear el renderizador: %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        return false;
    }

    return true;
}

void graphics_cleanup(SDL_Window* window, SDL_Renderer* renderer){
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    IMG_Quit();
    SDL_Quit();
}

Image* image_load(SDL_Renderer* renderer, const char* filename) {
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface) {
        printf("Error cargando imagen '%s': %s\n", filename, IMG_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Error creando textura: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return NULL;
    }

    Image* image = malloc(sizeof(Image));
    if (!image) {
        printf("Error asignado memoria para imagen \n");
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
        return NULL;
    }

    image->texture = texture;
    image->width = surface->w;
    image->height = surface->h;

    SDL_FreeSurface(surface);
    return image;
}

void image_free(Image* image){
    if (image) {
        if (image->texture) {
            SDL_DestroyTexture(image->texture);
        }
        free(image);
    }
}

void image_render(SDL_Renderer* renderer, Image* image, int x, int y) {
    if (!image || !image->texture) return;

    SDL_Rect dest_rect = {
        .x = x,
        .y = y,
        .w = image->width,
        .h = image->height
    };

    SDL_RenderCopy(renderer, image->texture, NULL, &dest_rect);
}

void image_render_centered(SDL_Renderer* renderer, Image* image) {
    if (!image || !image->texture) return;

    int window_width, window_height;
    SDL_GetRendererOutputSize(renderer, &window_width, &window_height);

    SDL_Rect dest_rect = {
        .x = (window_width - image->width) / 2,
        .y = (window_height - image->height) / 2,
        .w = image->width,
        .h = image->height
    };

    SDL_RenderCopy(renderer, image->texture, NULL, &dest_rect);

}

void render_clear(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderClear(renderer);
}

void render_present(SDL_Renderer* renderer) {
    SDL_RenderPresent(renderer);
}