#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

int main(int agrc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error al iniciar el SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* ventana = SDL_CreateWindow(
        "REVERT V.1.0.0",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640,
        480,
        SDL_WINDOW_SHOWN
    );

    if (!ventana) {
        printf("Error al crear la ventana: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderizador = SDL_CreateRenderer(
        ventana,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (!renderizador) {
        printf("Error añ crear el renderizador: %s\n", SDL_GetError());
        SDL_DestroyWindow(ventana);
        SDL_Quit();
        return 1;
    }

    bool ejecutando = true;
    SDL_Event evento;

    while (ejecutando){
        //Bucle principal de la aplicacion
        //Procesar eventos

        while (SDL_PollEvent(&evento))
        {
            if (evento.type == SDL_QUIT) {
                ejecutando = false;
            }
        }

        //Limpiar la pantalla de color azul
        SDL_SetRenderDrawColor(renderizador, 0, 0, 128, 255);
        SDL_RenderClear(renderizador);

        //Dibujar un objeto
        SDL_Rect rectangulo = {200, 150, 240, 180};
        SDL_SetRenderDrawColor(renderizador, 255, 0, 0, 255);
        SDL_RenderFillRect(renderizador, &rectangulo);

        SDL_RenderPresent(renderizador);

        SDL_Delay(16);
        
    }

    //Clean

    SDL_DestroyRenderer(renderizador);
    SDL_DestroyWindow(ventana);

    return 0;
}