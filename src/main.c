#include "graphics.h"
#include "ui.h"
#include <stdio.h>
#include <stdbool.h>
#include "config.h"
#include "plugin_manager.h"
#include <string.h>


#define FRAME_DELAY 16 

int main(int argc, char* argv[]) {
    printf("Starting application...\n");

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

    #ifdef _WIN32
    ui_init("C:\\Windows\\Fonts\\arial.ttf");
#else
    ui_init("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
#endif

    test_image = image_load(renderer, argv[1]);
    if (!test_image) {
        printf("No se pudo cargar la imagen. Saliendo... \n");
        ui_cleanup();
        graphics_cleanup(window, renderer);
        return 1;
    }

    // Botones (Estilo clasico: Negro/Blanco, Sin Margenes, 0 Padding)
    // Colores de la UI (Gris Oscuro)
    SDL_Color ui_dark_gray = {40, 40, 40, 255};
    
    Button btn_save   = ui_create_button(10, 10, 100, 30, "Guardar", ui_dark_gray, 0);
    Button btn_draw   = ui_create_button(110, 10, 120, 30, "Dibujar: OFF", ui_dark_gray, 0);
    Button btn_scale  = ui_create_button(230, 10, 100, 30, "Escala: 1x", ui_dark_gray, 0);
    Button btn_edit   = ui_create_button(330, 10, 100, 30, "Edit", ui_dark_gray, 0);

    //Advanced button
    Button btn_adv    = ui_create_button(430, 10, 100, 30, "Advanced", ui_dark_gray, 0);

    // Botones de filtros (Toolbox Vertical - Sin Margenes)
    Button btn_gray   = ui_create_button(330, 40, 100, 30, "Gris", ui_dark_gray, 0);
    Button btn_sepia  = ui_create_button(330, 70, 100, 30, "Sepia", ui_dark_gray, 0);
    Button btn_invert = ui_create_button(330, 100, 100, 30, "Inv", ui_dark_gray, 0);

    Button btn_plugins = ui_create_button(530, 10, 100, 30, "Plugins", ui_dark_gray, 0);

    PluginManager pm;
    plugin_manager_init(&pm);
    printf("Compiling plugin sources...\n");
    plugin_manager_compile_sources("plugins_src", "plugins_bin");
    
    printf("Loading plugins...\n");
    plugin_manager_load_dir(&pm, "."); 
    plugin_manager_load_dir(&pm, "plugins_bin");
    printf("Plugins loaded: %d\n", pm.count);

    // Crear botones para plugins dinamicamente
    printf("Initializing plugin UI arrays...\n");
    Button plugin_buttons[MAX_PLUGINS];
    Button adv_plugin_labels[MAX_PLUGINS];
    Button adv_plugin_minus[MAX_PLUGINS];
    Button adv_plugin_plus[MAX_PLUGINS];
    memset(plugin_buttons, 0, sizeof(plugin_buttons));
    memset(adv_plugin_labels, 0, sizeof(adv_plugin_labels));
    memset(adv_plugin_minus, 0, sizeof(adv_plugin_minus));
    memset(adv_plugin_plus, 0, sizeof(adv_plugin_plus));
    int adv_plugin_count = 0;


    printf("Creating plugin buttons...\n");
    for (int i = 0; i < pm.count; i++) {
        if (pm.plugins[i].plugin) {
            printf("Creating button for plugin: %s\n", pm.plugins[i].plugin->info.name);
            plugin_buttons[i] = ui_create_button(530, 40 + (i * 30), 150, 30, pm.plugins[i].plugin->info.name, ui_dark_gray, 5);
            
            if (pm.plugins[i].plugin->info.show_in_advanced) {
                printf(" - Adding to advanced panel\n");
                int y = 310 + (adv_plugin_count * 40);
                adv_plugin_labels[i] = ui_create_button(20, y, 150, 30, pm.plugins[i].plugin->info.name, ui_dark_gray, 0);
                adv_plugin_minus[i] = ui_create_button(170, y, 40, 30, "-", ui_dark_gray, -1);
                adv_plugin_plus[i] = ui_create_button(210, y, 40, 30, "+", ui_dark_gray, -1);
                adv_plugin_count++;
            }
        }
    }
    printf("Plugin UI setup complete.\n");





    bool drawing_mode = false;
    bool show_toolbox = false;
    bool show_adv = false;
    bool show_plugins = false;
    float current_scale = 1.0f;
    bool running = true;

    SDL_Event event;

    // Elementos del panel Advanced
    Button lbl_title = ui_create_button(10, 50, 480, 40, "--- ADVANCED SETTINGS ---", ui_dark_gray, -1);
    
    Button opt_brightness = ui_create_button(20, 110, 150, 30, "Brightness", ui_dark_gray, 0);
    Button less_brightness = ui_create_button(170, 110, 40, 30, "-", ui_dark_gray, -1);
    Button more_brightness = ui_create_button(210, 110, 40, 30, "+", ui_dark_gray, -1);

    Button opt_contrast = ui_create_button(20, 150, 150, 30, "Contrast", ui_dark_gray, 0);
    Button less_contrast = ui_create_button(170, 150, 40, 30, "-", ui_dark_gray, -1);
    Button more_contrast = ui_create_button(210, 150, 40, 30, "+", ui_dark_gray, -1);

    Button opt_saturation = ui_create_button(20, 190, 150, 30, "Saturation", ui_dark_gray, 0);
    Button less_saturation = ui_create_button(170, 190, 40, 30, "-", ui_dark_gray, -1);
    Button more_saturation = ui_create_button(210, 190, 40, 30, "+", ui_dark_gray, -1);

    Button opt_blur = ui_create_button(20, 230, 150, 30, "Blur", ui_dark_gray, 0);
    Button less_blur = ui_create_button(170, 230, 40, 30, "-", ui_dark_gray, -1);
    Button more_blur = ui_create_button(210, 230, 40, 30, "+", ui_dark_gray, -1);

    Button opt_normal = ui_create_button(20, 270, 150, 30, "NormalMap", ui_dark_gray, 0);
    Checkbox normalmap_checkbox = ui_create_checkbox(170, 270, 30, "", ui_dark_gray, false);



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

            if (button_is_clicked(&btn_edit, &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                show_toolbox = !show_toolbox;
                btn_edit.label = show_toolbox ? "Close" : "Edit";
            }

            if (button_is_clicked(&btn_adv, &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                show_adv = !show_adv;
                btn_adv.label = show_adv ? "Close" : "Advanced";
            }

            if (button_is_clicked(&btn_plugins, &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                show_plugins = !show_plugins;
                btn_plugins.label = show_plugins ? "Close" : "Plugins";
            }


            // Lógica de filtros (solo si toolbox está abierto)
            if (show_toolbox) {
                if (button_is_clicked(&btn_gray, &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                    image_apply_grayscale(test_image);
                    image_update_texture(renderer, test_image);
                }
                if (button_is_clicked(&btn_sepia, &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                    image_apply_sepia(test_image);
                    image_update_texture(renderer, test_image);
                }
                if (button_is_clicked(&btn_invert, &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                    image_apply_invert(test_image);
                    image_update_texture(renderer, test_image);
                }
                
            }

            if (show_adv) {
                if (button_is_clicked(&more_brightness, &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                    image_apply_brightness(test_image, 10);
                    image_update_texture(renderer, test_image);
                }
                if (button_is_clicked(&less_brightness, &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                    image_apply_brightness(test_image, -10);
                    image_update_texture(renderer, test_image);
                }
                if (button_is_clicked(&more_contrast, &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                    image_apply_contrast(test_image, 1.1f);
                    image_update_texture(renderer, test_image);
                }
                if (button_is_clicked(&less_contrast, &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                    image_apply_contrast(test_image, 0.9f);
                    image_update_texture(renderer, test_image);
                }
                if (button_is_clicked(&more_saturation, &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                    image_apply_saturation(test_image, 1.1f);
                    image_update_texture(renderer, test_image);
                }
                if (button_is_clicked(&less_saturation, &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                    image_apply_saturation(test_image, 0.9f);
                    image_update_texture(renderer, test_image);
                }
                if (button_is_clicked(&more_blur, &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                    image_apply_gaussian_blur(test_image, 2); // Blur mas fuerte
                    image_update_texture(renderer, test_image);
                }
                if (button_is_clicked(&less_blur, &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                    image_apply_gaussian_blur(test_image, 1); // Blur suave
                    image_update_texture(renderer, test_image);
                }
                if (checkbox_handle_event(&normalmap_checkbox, &event)) {
                    if (normalmap_checkbox.checked) {
                        image_apply_normalmap(test_image);
                    } else {
                        image_reset(test_image);
                    }
                    image_update_texture(renderer, test_image);
                }


                // Plugins en panel avanzado
                for (int i = 0; i < pm.count; i++) {
                    if (pm.plugins[i].plugin->info.show_in_advanced) {
                        if (button_is_clicked(&adv_plugin_plus[i], &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                            if (pm.plugins[i].plugin->apply_param) {
                                pm.plugins[i].plugin->apply_param(test_image, 50.0f); // Ejemplo: incremento de intensidad
                                image_update_texture(renderer, test_image);
                            }
                        }
                        if (button_is_clicked(&adv_plugin_minus[i], &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                            if (pm.plugins[i].plugin->apply_param) {
                                pm.plugins[i].plugin->apply_param(test_image, 10.0f); // Ejemplo: intensidad baja
                                image_update_texture(renderer, test_image);
                            }
                        }
                    }
                }
            }


            if (show_plugins) {
                for (int i = 0; i < pm.count; i++) {
                    if (button_is_clicked(&plugin_buttons[i], &event) && event.type == SDL_MOUSEBUTTONDOWN) {
                        if (pm.plugins[i].plugin && pm.plugins[i].plugin->apply) {
                            pm.plugins[i].plugin->apply(test_image);
                            image_update_texture(renderer, test_image);
                        }
                    }
                }
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
                    // ... (resto de la lógica de dibujo)
                    if (drawing_mode && (event.motion.state & SDL_BUTTON_LMASK || event.button.button == SDL_BUTTON_LEFT)) {
                        int mx = (event.type == SDL_MOUSEMOTION) ? event.motion.x : event.button.x;
                        int my = (event.type == SDL_MOUSEMOTION) ? event.motion.y : event.button.y;
                        
                        bool over_top_bar = (my < 45);
                        bool over_toolbox = show_toolbox && (mx >= 330 && mx <= 430 && my < 135);
                        bool over_adv_panel = show_adv && (mx >= 10 && mx <= 510 && my >= 40 && my <= 640);

                        if (!over_top_bar && !over_toolbox && !over_adv_panel) {
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
        button_render(renderer, &btn_edit);
        button_render(renderer, &btn_adv);
        button_render(renderer, &btn_plugins);


        if (show_toolbox) {
            button_render(renderer, &btn_gray);
            button_render(renderer, &btn_sepia);
            button_render(renderer, &btn_invert);
        }

        if (show_adv) {
            // Panel Advanced (Gris Oscuro)
            SDL_Rect adv_panel = {10, 40, 500, 600};
            SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
            SDL_RenderFillRect(renderer, &adv_panel);
            
            // Borde gris mas suave
            SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
            SDL_RenderDrawRect(renderer, &adv_panel);

            // Renderizado de contenido del panel
            button_render(renderer, &lbl_title);
            button_render(renderer, &opt_brightness);
            button_render(renderer, &less_brightness);
            button_render(renderer, &more_brightness);
            
            button_render(renderer, &opt_contrast);
            button_render(renderer, &less_contrast);
            button_render(renderer, &more_contrast);

            button_render(renderer, &opt_saturation);
            button_render(renderer, &less_saturation);
            button_render(renderer, &more_saturation);

            button_render(renderer, &opt_blur);
            button_render(renderer, &less_blur);
            button_render(renderer, &more_blur);

            button_render(renderer, &opt_normal);
            checkbox_render(renderer, &normalmap_checkbox);

            // Renderizado de plugins avanzados
            for (int i = 0; i < pm.count; i++) {
                if (pm.plugins[i].plugin->info.show_in_advanced) {
                    button_render(renderer, &adv_plugin_labels[i]);
                    button_render(renderer, &adv_plugin_minus[i]);
                    button_render(renderer, &adv_plugin_plus[i]);
                }
            }
        }


        if (show_plugins) {
            for (int i = 0; i < pm.count; i++) {
                button_render(renderer, &plugin_buttons[i]);
            }
        }



        render_present(renderer);
        SDL_Delay(FRAME_DELAY);
    }

    image_free(test_image);
    plugin_manager_cleanup(&pm);
    ui_cleanup();

    graphics_cleanup(window, renderer);
    return 0;
}