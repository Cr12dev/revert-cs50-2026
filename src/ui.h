#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

typedef struct {
    SDL_Rect rect;
    const char* label;
    SDL_Color color;
    bool hovered;
} Button;

void ui_init(const char* font_path);
void ui_cleanup();
void button_render(SDL_Renderer* renderer, Button* button);
bool button_is_clicked(Button* button, SDL_Event* event);

#endif
