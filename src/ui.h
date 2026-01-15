#ifndef UI_H
#define UI_H

#ifdef _WIN32
    #include <SDL.h>
    #include <SDL_ttf.h>
#else
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_ttf.h>
#endif
#include <stdbool.h>

typedef struct {
    SDL_Rect rect;
    const char* label;
    SDL_Color color;
    bool hovered;
    int padding;
} Button;

typedef struct {
    SDL_Rect rect;
    const char* label;
    SDL_Color color;
    bool checked;
    bool hovered;
} Checkbox;

void ui_init(const char* font_path);
void ui_cleanup();
Button ui_create_button(int x, int y, int w, int h, const char* label, SDL_Color color, int padding);
void button_render(SDL_Renderer* renderer, Button* button);
bool button_is_clicked(Button* button, SDL_Event* event);

Checkbox ui_create_checkbox(int x, int y, int size, const char* label, SDL_Color color, bool checked);
void checkbox_render(SDL_Renderer* renderer, Checkbox* checkbox);
bool checkbox_handle_event(Checkbox* checkbox, SDL_Event* event);


#endif
