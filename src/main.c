#include "./graphics.h"
#include <stdio.h>
#include <stdbool.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FRAME_DELAY 16 

int main(int agrc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    Image* test_image = NULL;

    if (!graphics_init(&window, &renderer, WINDOW_WIDTH, WINDOW_HEIGHT)) {
        return 1;
    }

    test_image = image_load(renderer, "./assets/images/1.png");
    if (!test_image) {
        printf("No se pudo cargar la imagen. Saliendo... \n");
        graphics_cleanup(window, renderer);
        return 1;
    }

    printf("Imagen cargada: %dx%d pixeles \n", test_image->width, test_image->height);


    bool running = true;
    SDL_Event event;

    while (running){


        while (SDL_PollEvent(&event))
        {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                    }
                    break;
            }
        }

        render_clear(renderer, 45, 45, 90);

        image_render_centered(renderer, test_image);

        render_present(renderer);

        SDL_Delay(FRAME_DELAY);
        
    }



    image_free(test_image);
    graphics_cleanup(window, renderer);

    printf("Programa terminado correcgtamente\n");

    return 0;
}