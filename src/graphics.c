#include "graphics.h"
#include "memory.h"
#include <stdio.h>
#include <math.h>
#include <string.h>



// Definicion local de funciones de memoria para asegurar punteros estables
// void* sdl_malloc(size_t size) { return malloc(size); }
// void* sdl_calloc(size_t nmemb, size_t size) { return calloc(nmemb, size); }
// void* sdl_realloc(void* ptr, size_t size) { return realloc(ptr, size); }
// void  sdl_free(void* ptr) { if (ptr) free(ptr); }

bool graphics_init(SDL_Window** window, SDL_Renderer** renderer, int width, int height){
    // Inicializar SDL Video y Eventos
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        fprintf(stderr, "Error al iniciar el SDL: %s\n", SDL_GetError());
        return false;
    }

    // Configurar pistas para mejorar el rendimiento en GPU
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

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
        printf("Aviso: No se pudo crear renderer acelerado, intentando software... %s\n", SDL_GetError());
        *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_SOFTWARE);
    }

    if (*renderer) {
        SDL_RendererInfo info;
        SDL_GetRendererInfo(*renderer, &info);
        printf("Renderer Driver: %s (GPU Accel: %s)\n",
               info.name,
               (info.flags & SDL_RENDERER_ACCELERATED) ? "YES" : "NO");
    } else {
        printf("Error al crear el renderizador: %s\n", SDL_GetError());
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

    if (!surface) {
        printf("Error: No se pudo convertir la superficie al formato RGBA32\n");
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Error creando textura: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return NULL;
    }

    Image* image = (Image*)malloc(sizeof(Image));
    if (!image) {
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
        return NULL;
    }

    image->texture = texture;
    image->surface = surface;
    image->backup_surface = SDL_ConvertSurface(surface, surface->format, 0);
    image->width = surface->w;
    image->height = surface->h;

    return image;
}

void image_free(Image* image){
    if (image) {
        if (image->texture) SDL_DestroyTexture(image->texture);
        if (image->surface) SDL_FreeSurface(image->surface);
        if (image->backup_surface) SDL_FreeSurface(image->backup_surface);
        free(image);
    }
}

bool image_save(Image* image, const char* filename) {
    if (!image || !image->surface) return false;
    return IMG_SavePNG(image->surface, filename) == 0;
}

void image_draw_point(Image* image, SDL_Renderer* renderer, int x, int y, Uint32 color) {
    (void)renderer;
    if (!image || !image->surface) return;
    
    SDL_Surface* surf = image->surface;
    SDL_LockSurface(surf);
    Uint8* pixels = (Uint8*)surf->pixels;

    for (int dy = -2; dy <= 2; dy++) {
        for (int dx = -2; dx <= 2; dx++) {
            int px = x + dx;
            int py = y + dy;
            if (px >= 0 && px < image->width && py >= 0 && py < image->height) {
                // Escritura segura mediante memcpy para evitar problemas de alineacion
                Uint8* target = pixels + py * surf->pitch + px * 4;
                memcpy(target, &color, 4);
            }
        }
    }
    SDL_UnlockSurface(surf);
}

void image_update_texture(SDL_Renderer* renderer, Image* image) {
    (void)renderer;
    if (!image || !image->surface || !image->texture) return;
    SDL_UpdateTexture(image->texture, NULL, image->surface->pixels, image->surface->pitch);
}

void image_reset(Image* image) {
    if (!image || !image->surface || !image->backup_surface) return;
    SDL_BlitSurface(image->backup_surface, NULL, image->surface, NULL);
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
    SDL_Surface* surf = image->surface;
    SDL_LockSurface(surf);
    Uint8* pixels = (Uint8*)surf->pixels;

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Uint8* p_ptr = pixels + y * surf->pitch + x * 4;
            Uint32 pixel_val;
            memcpy(&pixel_val, p_ptr, 4);

            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel_val, surf->format, &r, &g, &b, &a);
            Uint8 gray = (Uint8)(0.299f * r + 0.587f * g + 0.114f * b);
            
            Uint32 out_p = SDL_MapRGBA(surf->format, gray, gray, gray, a);
            memcpy(p_ptr, &out_p, 4);
        }
    }
    SDL_UnlockSurface(surf);
}

void image_apply_sepia(Image* image) {
    if (!image || !image->surface) return;
    SDL_Surface* surf = image->surface;
    SDL_LockSurface(surf);
    Uint8* pixels = (Uint8*)surf->pixels;

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Uint8* p_ptr = pixels + y * surf->pitch + x * 4;
            Uint32 pixel_val;
            memcpy(&pixel_val, p_ptr, 4);

            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel_val, surf->format, &r, &g, &b, &a);
            
            int tr = (int)(0.393f * r + 0.769f * g + 0.189f * b);
            int tg = (int)(0.349f * r + 0.686f * g + 0.168f * b);
            int tb = (int)(0.272f * r + 0.534f * g + 0.131f * b);

            Uint8 out_r = (tr > 255) ? 255 : tr;
            Uint8 out_g = (tg > 255) ? 255 : tg;
            Uint8 out_b = (tb > 255) ? 255 : tb;

            Uint32 out_p = SDL_MapRGBA(surf->format, out_r, out_g, out_b, a);
            memcpy(p_ptr, &out_p, 4);
        }
    }
    SDL_UnlockSurface(surf);
}

void image_apply_invert(Image* image) {
    if (!image || !image->surface) return;
    SDL_Surface* surf = image->surface;
    SDL_LockSurface(surf);
    Uint8* pixels = (Uint8*)surf->pixels;

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Uint8* p_ptr = pixels + y * surf->pitch + x * 4;
            Uint32 pixel_val;
            memcpy(&pixel_val, p_ptr, 4);

            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel_val, surf->format, &r, &g, &b, &a);
            Uint32 out_p = SDL_MapRGBA(surf->format, 255 - r, 255 - g, 255 - b, a);
            memcpy(p_ptr, &out_p, 4);
        }
    }
    SDL_UnlockSurface(surf);
}

void image_apply_brightness(Image* image, int factor) {
    if (!image || !image->surface) return;
    SDL_Surface* surf = image->surface;
    SDL_LockSurface(surf);
    Uint8* pixels = (Uint8*)surf->pixels;

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Uint8* p_ptr = pixels + y * surf->pitch + x * 4;
            Uint32 pixel_val;
            memcpy(&pixel_val, p_ptr, 4);

            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel_val, surf->format, &r, &g, &b, &a);
            
            int nr = (int)r + factor;
            int ng = (int)g + factor;
            int nb = (int)b + factor;
            r = (Uint8)((nr > 255) ? 255 : (nr < 0 ? 0 : nr));
            g = (Uint8)((ng > 255) ? 255 : (ng < 0 ? 0 : ng));
            b = (Uint8)((nb > 255) ? 255 : (nb < 0 ? 0 : nb));
            
            Uint32 out_p = SDL_MapRGBA(surf->format, r, g, b, a);
            memcpy(p_ptr, &out_p, 4);
        }
    }
    SDL_UnlockSurface(surf);
}

void image_apply_contrast(Image* image, float factor) {
    if (!image || !image->surface) return;
    SDL_Surface* surf = image->surface;
    SDL_LockSurface(surf);
    Uint8* pixels = (Uint8*)surf->pixels;

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Uint8* p_ptr = pixels + y * surf->pitch + x * 4;
            Uint32 pixel_val;
            memcpy(&pixel_val, p_ptr, 4);

            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel_val, surf->format, &r, &g, &b, &a);
            
            int nr = (int)((factor * (r - 128)) + 128);
            int ng = (int)((factor * (g - 128)) + 128);
            int nb = (int)((factor * (b - 128)) + 128);
            r = (Uint8)((nr > 255) ? 255 : (nr < 0 ? 0 : nr));
            g = (Uint8)((ng > 255) ? 255 : (ng < 0 ? 0 : ng));
            b = (Uint8)((nb > 255) ? 255 : (nb < 0 ? 0 : nb));

            Uint32 out_p = SDL_MapRGBA(surf->format, r, g, b, a);
            memcpy(p_ptr, &out_p, 4);
        }
    }
    SDL_UnlockSurface(surf);
}

void image_apply_saturation(Image* image, float factor) {
    if (!image || !image->surface) return;
    SDL_Surface* surf = image->surface;
    SDL_LockSurface(surf);
    Uint8* pixels = (Uint8*)surf->pixels;

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Uint8* p_ptr = pixels + y * surf->pitch + x * 4;
            Uint32 pixel_val;
            memcpy(&pixel_val, p_ptr, 4);

            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel_val, surf->format, &r, &g, &b, &a);
            
            float lum = 0.299f * r + 0.587f * g + 0.114f * b;
            int nr = (int)(lum + (r - lum) * factor);
            int ng = (int)(lum + (g - lum) * factor);
            int nb = (int)(lum + (b - lum) * factor);

            r = (Uint8)((nr > 255) ? 255 : (nr < 0 ? 0 : nr));
            g = (Uint8)((ng > 255) ? 255 : (ng < 0 ? 0 : ng));
            b = (Uint8)((nb > 255) ? 255 : (nb < 0 ? 0 : nb));

            Uint32 out_p = SDL_MapRGBA(surf->format, r, g, b, a);
            memcpy(p_ptr, &out_p, 4);
        }
    }
    SDL_UnlockSurface(surf);
}

void image_apply_gaussian_blur(Image* image, int radius) {
    if (!image || !image->surface || radius <= 0) return;

    int w = image->width;
    int h = image->height;
    SDL_Surface* surface = image->surface;
    SDL_PixelFormat* format = surface->format;
    
    // Buffer temporal para el calculo (alineado a 32 bits)
    Uint32* temp_pixels = (Uint32*)malloc(w * h * sizeof(Uint32));
    if (!temp_pixels) return;

    SDL_LockSurface(surface);
    Uint8* src_bytes = (Uint8*)surface->pixels;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            long long sum_r = 0, sum_g = 0, sum_b = 0, sum_a = 0;
            int count = 0;

            for (int ky = -radius; ky <= radius; ky++) {
                int ny = y + ky;
                if (ny < 0 || ny >= h) continue;

                for (int kx = -radius; kx <= radius; kx++) {
                    int nx = x + kx;
                    if (nx >= 0 && nx < w) {
                        Uint32 pix_val;
                        memcpy(&pix_val, src_bytes + (ny * surface->pitch) + (nx * 4), 4);
                        
                        Uint8 r, g, b, a;
                        SDL_GetRGBA(pix_val, format, &r, &g, &b, &a);
                        sum_r += r; sum_g += g; sum_b += b; sum_a += a;
                        count++;
                    }
                }
            }

            Uint32 final_color = SDL_MapRGBA(format, (Uint8)(sum_r / count), (Uint8)(sum_g / count), (Uint8)(sum_b / count), 255);
            temp_pixels[y * w + x] = final_color;
        }
    }

    // Copiar de vuelta respetando el pitch del hardware
    for (int y = 0; y < h; y++) {
        memcpy(src_bytes + (y * surface->pitch), &temp_pixels[y * w], w * 4);
    }

    SDL_UnlockSurface(surface);
    free(temp_pixels);
}

static float get_intensity(SDL_Surface* surf, int x, int y) {
    if (x < 0) x = 0;
    if (x >= surf->w) x = surf->w - 1;
    if (y < 0) y = 0;
    if (y >= surf->h) y = surf->h - 1;

    Uint32 pixel;
    Uint8* p = (Uint8*)surf->pixels + y * surf->pitch + x * 4;
    memcpy(&pixel, p, 4);
    Uint8 r, g, b, a;
    SDL_GetRGBA(pixel, surf->format, &r, &g, &b, &a);
    return (0.299f * r + 0.587f * g + 0.114f * b);
}

void image_apply_normalmap(Image* image) {
    if (!image || !image->surface) return;
    
    int w = image->width;
    int h = image->height;
    SDL_Surface* surf = image->surface;
    
    Uint32* temp_pixels = (Uint32*)malloc(w * h * sizeof(Uint32));
    if (!temp_pixels) return;

    SDL_LockSurface(surf);
    
    float strength = 2.0f; // Ajuste de relieve

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float tl = get_intensity(surf, x - 1, y - 1);
            float t  = get_intensity(surf, x,     y - 1);
            float tr = get_intensity(surf, x + 1, y - 1);
            float l  = get_intensity(surf, x - 1, y);
            float r  = get_intensity(surf, x + 1, y);
            float bl = get_intensity(surf, x - 1, y + 1);
            float b  = get_intensity(surf, x,     y + 1);
            float br = get_intensity(surf, x + 1, y + 1);

            // Sobel kernels
            float dx = (tr + 2.0f * r + br) - (tl + 2.0f * l + bl);
            float dy = (bl + 2.0f * b + br) - (tl + 2.0f * t + tr);
            float dz = 255.0f / strength;

            // Normalize
            float len = sqrtf(dx * dx + dy * dy + dz * dz);
            dx /= len;
            dy /= len;
            dz /= len;

            // Map to 0-255
            Uint8 nr = (Uint8)((dx * 0.5f + 0.5f) * 255);
            Uint8 ng = (Uint8)((dy * 0.5f + 0.5f) * 255);
            Uint8 nb = (Uint8)((dz * 0.5f + 0.5f) * 255);

            temp_pixels[y * w + x] = SDL_MapRGBA(surf->format, nr, ng, nb, 255);
        }
    }

    // Copiar al original
    Uint8* src_bytes = (Uint8*)surf->pixels;
    for (int y = 0; y < h; y++) {
        memcpy(src_bytes + (y * surf->pitch), &temp_pixels[y * w], w * 4);
    }

    SDL_UnlockSurface(surf);
    free(temp_pixels);
}