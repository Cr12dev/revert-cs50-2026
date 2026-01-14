#include "graphics.h"
#include "memory.h"
#include <stdio.h>



bool graphics_init(SDL_Window** window, SDL_Renderer** renderer, int width, int height){
    if (SDL_SetMemoryFunctions(my_malloc, my_calloc, my_realloc, my_free) != 0){
        printf("Failed to set memory functions: %s\n", SDL_GetError());
        return false;
    } else {
        printf("Memory functions set successfully: %s\n", my_malloc);
    }
    
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

    if (TTF_Init() == -1) {
        printf("Error al iniciar SDL_ttf: %s\n", TTF_GetError());
        IMG_Quit();
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
    TTF_Quit();
    SDL_Quit();
}

Image* image_load(SDL_Renderer* renderer, const char* filename) {
    SDL_Surface* temp = IMG_Load(filename);
    if (!temp) {
        printf("Error cargando imagen '%s': %s\n", filename, IMG_GetError());
        return NULL;
    }

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(temp);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Error creando textura: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return NULL;
    }

    Image* image = malloc(sizeof(Image));
    if (!image) {
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
        return NULL;
    }

    image->texture = texture;
    image->surface = surface;
    image->width = surface->w;
    image->height = surface->h;

    return image;
}

void image_free(Image* image){
    if (image) {
        if (image->texture) SDL_DestroyTexture(image->texture);
        if (image->surface) SDL_FreeSurface(image->surface);
        free(image);
    }
}

bool image_save(Image* image, const char* filename) {
    if (!image || !image->surface) return false;
    return IMG_SavePNG(image->surface, filename) == 0;
}

void image_draw_point(Image* image, SDL_Renderer* renderer, int x, int y, Uint32 color) {
    if (!image || !image->surface) return;
    
    for (int dy = -2; dy <= 2; dy++) {
        for (int dx = -2; dx <= 2; dx++) {
            int px = x + dx;
            int py = y + dy;
            if (px >= 0 && px < image->width && py >= 0 && py < image->height) {
                Uint32* pixels = (Uint32*)image->surface->pixels;
                pixels[py * image->width + px] = color;
            }
        }
    }
}

void image_update_texture(SDL_Renderer* renderer, Image* image) {
    if (!image || !image->surface || !image->texture) return;
    SDL_UpdateTexture(image->texture, NULL, image->surface->pixels, image->surface->pitch);
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

void image_render_centered(SDL_Renderer* renderer, Image* image, float custom_scale) {
    if (!image || !image->texture) return;

    int window_width, window_height;
    SDL_GetRendererOutputSize(renderer, &window_width, &window_height);

    int max_w = window_width - (IMAGE_PADDING * 2);
    int max_h = window_height - (IMAGE_PADDING * 2);

    float auto_scale = 1.0f;
    if (image->width > max_w || image->height > max_h) {
        float scale_w = (float)max_w / image->width;
        float scale_h = (float)max_h / image->height;
        auto_scale = (scale_w < scale_h) ? scale_w : scale_h;
    }

    float final_scale = auto_scale * custom_scale;

    int final_w = (int)(image->width * final_scale);
    int final_h = (int)(image->height * final_scale);

    SDL_Rect dest_rect = {
        .x = (window_width - final_w) / 2,
        .y = (window_height - final_h) / 2,
        .w = final_w,
        .h = final_h
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

void image_apply_grayscale(Image* image) {
    if (!image || !image->surface) return;
    SDL_LockSurface(image->surface);
    Uint32* pixels = (Uint32*)image->surface->pixels;
    int pixel_count = image->width * image->height;

    for (int i = 0; i < pixel_count; i++) {
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixels[i], image->surface->format, &r, &g, &b, &a);
        Uint8 gray = (Uint8)(0.299f * r + 0.587f * g + 0.114f * b);
        pixels[i] = SDL_MapRGBA(image->surface->format, gray, gray, gray, a);
    }
    SDL_UnlockSurface(image->surface);
}

void image_apply_sepia(Image* image) {
    if (!image || !image->surface) return;
    SDL_LockSurface(image->surface);
    Uint32* pixels = (Uint32*)image->surface->pixels;
    int pixel_count = image->width * image->height;

    for (int i = 0; i < pixel_count; i++) {
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixels[i], image->surface->format, &r, &g, &b, &a);
        
        int tr = (int)(0.393f * r + 0.769f * g + 0.189f * b);
        int tg = (int)(0.349f * r + 0.686f * g + 0.168f * b);
        int tb = (int)(0.272f * r + 0.534f * g + 0.131f * b);

        Uint8 out_r = (tr > 255) ? 255 : tr;
        Uint8 out_g = (tg > 255) ? 255 : tg;
        Uint8 out_b = (tb > 255) ? 255 : tb;

        pixels[i] = SDL_MapRGBA(image->surface->format, out_r, out_g, out_b, a);
    }
    SDL_UnlockSurface(image->surface);
}

void image_apply_invert(Image* image) {
    if (!image || !image->surface) return;
    SDL_LockSurface(image->surface);
    Uint32* pixels = (Uint32*)image->surface->pixels;
    int pixel_count = image->width * image->height;

    for (int i = 0; i < pixel_count; i++) {
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixels[i], image->surface->format, &r, &g, &b, &a);
        pixels[i] = SDL_MapRGBA(image->surface->format, 255 - r, 255 - g, 255 - b, a);
    }
    SDL_UnlockSurface(image->surface);
}