#include "plugin.h"
#include <stdlib.h>
#include <time.h>

// Version robusta de Roughness
static void process_noise(Image* image, int intensity) {
    if (!image || !image->surface || intensity <= 0) return;
    
    SDL_Surface* surf = image->surface;
    if (SDL_LockSurface(surf) != 0) return;

    Uint8* pixels = (Uint8*)surf->pixels;
    int h = surf->h;
    int w = surf->w;
    int pitch = surf->pitch;

    // Usamos una semilla fija o externa para evitar resetear srand constantemente
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = true;
    }

    for (int y = 0; y < h; y++) {
        Uint8* row = pixels + y * pitch;
        for (int x = 0; x < w; x++) {
            Uint8* p = row + x * 4; // Asumimos 4 bytes por pixel (RGBA32)
            
            int noise = (rand() % intensity) - (intensity / 2);
            
            for (int i = 0; i < 3; i++) { // R, G, B
                int v = (int)p[i] + noise;
                p[i] = (Uint8)(v > 255 ? 255 : (v < 0 ? 0 : v));
            }
        }
    }
    SDL_UnlockSurface(surf);
}

static void apply_roughness(Image* image) {
    process_noise(image, 40);
}

static void apply_roughness_param(Image* image, float value) {
    process_noise(image, (int)value);
}

static ImagePlugin plugin = {
    .info = {
        .name = "Roughness",
        .author = "Cr12dev",
        .version = "1.1.1",
        .show_in_advanced = true
    },
    .apply = apply_roughness,
    .apply_param = apply_roughness_param
};

ImagePlugin* plugin_init() {
    return &plugin;
}