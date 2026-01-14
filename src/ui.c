#include "ui.h"
#include <stdio.h>

static TTF_Font* global_font = NULL;

void ui_init(const char* font_path) {
    global_font = TTF_OpenFont(font_path, 18);
    if (!global_font) {
        printf("Error cargando fuente: %s\n", TTF_GetError());
    }
}

void ui_cleanup() {
    if (global_font) TTF_CloseFont(global_font);
}

void button_render(SDL_Renderer* renderer, Button* button) {
    SDL_SetRenderDrawColor(renderer, button->color.r, button->color.g, button->color.b, 255);
    if (button->hovered) {
        int r = (button->color.r + 40 > 255) ? 255 : button->color.r + 40;
        int g = (button->color.g + 40 > 255) ? 255 : button->color.g + 40;
        int b = (button->color.b + 40 > 255) ? 255 : button->color.b + 40;
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    }
    SDL_RenderFillRect(renderer, &button->rect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &button->rect);

    if (global_font && button->label) {
        SDL_Surface* text_surf = TTF_RenderText_Blended(global_font, button->label, (SDL_Color){255, 255, 255, 255});
        if (text_surf) {
            SDL_Texture* text_tex = SDL_CreateTextureFromSurface(renderer, text_surf);
            int tw, th;
            SDL_QueryTexture(text_tex, NULL, NULL, &tw, &th);
            SDL_Rect text_rect = {
                button->rect.x + (button->rect.w - tw) / 2,
                button->rect.y + (button->rect.h - th) / 2,
                tw, th
            };
            SDL_RenderCopy(renderer, text_tex, NULL, &text_rect);
            SDL_DestroyTexture(text_tex);
            SDL_FreeSurface(text_surf);
        }
    }
}

bool button_is_clicked(Button* button, SDL_Event* event) {
    if (event->type == SDL_MOUSEMOTION) {
        int x = event->motion.x;
        int y = event->motion.y;
        button->hovered = (x >= button->rect.x && x <= button->rect.x + button->rect.w &&
                           y >= button->rect.y && y <= button->rect.y + button->rect.h);
    }
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        int x = event->button.x;
        int y = event->button.y;
        return (x >= button->rect.x && x <= button->rect.x + button->rect.w &&
                y >= button->rect.y && y <= button->rect.y + button->rect.h);
    }
    return false;
}
