#include "plugin.h"
#include <stdlib.h>

static void apply_negative(Image* image) {
    if (!image || !image->surface) return;
    
    SDL_Surface* surf = image->surface;
    SDL_LockSurface(surf);
    Uint8* pixels = (Uint8*)surf->pixels;

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Uint8* p_ptr = pixels + y * surf->pitch + x * 4;
            p_ptr[0] = 255 - p_ptr[0]; // R
            p_ptr[1] = 255 - p_ptr[1]; // G
            p_ptr[2] = 255 - p_ptr[2]; // B
        }
    }
    SDL_UnlockSurface(surf);
}

static ImagePlugin plugin = {
    .info = {
        .name = "Negative Filter",
        .author = "System",
        .version = "1.0.0"
    },
    .apply = apply_negative
};

ImagePlugin* plugin_init() {
    return &plugin;
}
