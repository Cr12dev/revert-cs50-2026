#include "./graphics.h"
#include "./ui.h"
#include <stdio.h>
#include <stdbool.h>
#include "config.h"

#define FRAME_DELAY 16 

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Uso: %s <ruta_a_imagen>\n", argv[0]);
        return 1;
    }

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    Image* test_image = NULL;

    if (!graphics_init(&window, &renderer, WINDOW_WIDTH, WINDOW_HEIGHT)) {
        return 1;
    }

    ui_init("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");

    test_image = image_load(renderer, argv[1]);
    if (!test_image) {
        printf("No se pudo cargar la imagen. Saliendo... \n");
        ui_cleanup();
        graphics_cleanup(window, renderer);
        return 1;
    }

    Button btn_save = {{10, 10, 120, 40}, "Guardar", {100, 100, 255}, false};
    Button btn_draw = {{140, 10, 120, 40}, "Dibujar: OFF", {255, 100, 100}, false};
    Button btn_scale = {{270, 10, 120, 40}, "Escala: 1x", {100, 255, 100}, false};

    bool drawing_mode = false;
    float current_scale = 1.0f;
    bool running = true;
    SDL_Event event;

    while (running){
        while (SDL_PollEvent(&event)) {
            if (button_is_clicked(&btn_save, &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                if (image_save(test_image, "saved_image.png")) {
                    printf("Imagen guardada como saved_image.png\n");
                }
            }
            if (button_is_clicked(&btn_draw, &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                drawing_mode = !drawing_mode;
                btn_draw.label = drawing_mode ? "Dibujar: ON" : "Dibujar: OFF";
            }
            if (button_is_clicked(&btn_scale, &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                if (current_scale == 1.0f) current_scale = 1.5f;
                else if (current_scale == 1.5f) current_scale = 2.0f;
                else current_scale = 1.0f;
                
                static char scale_label[20];
                sprintf(scale_label, "Escala: %.1fx", current_scale);
                btn_scale.label = scale_label;
            }

            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
                    break;
                case SDL_MOUSEMOTION:
                case SDL_MOUSEBUTTONDOWN:
                    if (drawing_mode && (event.motion.state & SDL_BUTTON_LMASK || event.button.button == SDL_BUTTON_LEFT)) {
                        int mx = (event.type == SDL_MOUSEMOTION) ? event.motion.x : event.button.x;
                        int my = (event.type == SDL_MOUSEMOTION) ? event.motion.y : event.button.y;
                        
                        // Solo dibujar si no estamos sobre botones
                        if (my > 60) {
                            int ww, wh;
                            SDL_GetRendererOutputSize(renderer, &ww, &wh);
                            int max_w = ww - (IMAGE_PADDING * 2);
                            int max_h = wh - (IMAGE_PADDING * 2);
                            float auto_scale = 1.0f;
                            if (test_image->width > max_w || test_image->height > max_h) {
                                float sw = (float)max_w / test_image->width;
                                float sh = (float)max_h / test_image->height;
                                auto_scale = (sw < sh) ? sw : sh;
                            }
                            float final_scale = auto_scale * current_scale;
                            int fw = (int)(test_image->width * final_scale);
                            int fh = (int)(test_image->height * final_scale);
                            int start_x = (ww - fw) / 2;
                            int start_y = (wh - fh) / 2;

                            int img_x = (int)((mx - start_x) / final_scale);
                            int img_y = (int)((my - start_y) / final_scale);

                            image_draw_point(test_image, renderer, img_x, img_y, 0xFFFF0000); // Rojo
                            image_update_texture(renderer, test_image);
                        }
                    }
                    break;
            }
        }

        render_clear(renderer, 45, 45, 90);
        image_render_centered(renderer, test_image, current_scale);
        
        button_render(renderer, &btn_save);
        button_render(renderer, &btn_draw);
        button_render(renderer, &btn_scale);

        render_present(renderer);
        SDL_Delay(FRAME_DELAY);
    }

    image_free(test_image);
    ui_cleanup();
    graphics_cleanup(window, renderer);
    return 0;
}