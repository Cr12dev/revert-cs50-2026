#include "plugin.h"
#include <stdlib.h>

static void apply_negative(Image* image) {
    if (!image || !image->surface) return;
    
    SDL_Surface* surf = image->surface;
    SDL_LockSurface(surf);
    
    int total_pixels = image->width * image->height;
    Uint32* pixels = (Uint32*)surf->pixels;

    for (int i = 0; i < total_pixels; i++) {
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixels[i], surf->format, &r, &g, &b, &a);
        pixels[i] = SDL_MapRGBA(surf->format, 255 - r, 255 - g, 255 - b, a);
    }
    
    SDL_UnlockSurface(surf);
}

static ImagePlugin plugin = {
    .info = {
        .name = "Negative Filter",
        .author = "System",
        .version = "1.0.1",
        .show_in_advanced = false
    },
    .apply = apply_negative,
    .apply_param = NULL
};

ImagePlugin* plugin_init() {
    return &plugin;
}
