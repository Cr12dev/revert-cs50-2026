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

Button ui_create_button(int x, int y, int w, int h, const char* label, SDL_Color color, int padding) {
    Button btn;
    btn.rect = (SDL_Rect){x, y, w, h};
    btn.label = label;
    btn.color = color;
    btn.hovered = false;
    btn.padding = padding;
    return btn;
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
            
            // Si padding es 0, pegamos al borde. Si es > 0, lo usamos de offset.
            // Si queremos mantener centrado por defecto, podríamos usar un valor especial like -1.
            // Pero seguiremos la instrucción del usuario para el toolbox.
            SDL_Rect text_rect;
            if (button->padding < 0) {
                // Centrado automático
                text_rect.x = button->rect.x + (button->rect.w - tw) / 2;
                text_rect.y = button->rect.y + (button->rect.h - th) / 2;
            } else if (button->padding == 0) {
                // 0% padding: pegado al borde (top-left)
                text_rect.x = button->rect.x;
                text_rect.y = button->rect.y;
            } else {
                // Offset fijo por padding
                text_rect.x = button->rect.x + button->padding;
                text_rect.y = button->rect.y + button->padding;
            }
            text_rect.w = tw;
            text_rect.h = th;

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
Checkbox ui_create_checkbox(int x, int y, int size, const char* label, SDL_Color color, bool checked) {
    Checkbox cb;
    cb.rect = (SDL_Rect){x, y, size, size};
    cb.label = label;
    cb.color = color;
    cb.checked = checked;
    cb.hovered = false;
    return cb;
}

void checkbox_render(SDL_Renderer* renderer, Checkbox* checkbox) {
    // Render the box
    SDL_SetRenderDrawColor(renderer, checkbox->color.r, checkbox->color.g, checkbox->color.b, 255);
    if (checkbox->hovered) {
        int r = (checkbox->color.r + 40 > 255) ? 255 : checkbox->color.r + 40;
        int g = (checkbox->color.g + 40 > 255) ? 255 : checkbox->color.g + 40;
        int b = (checkbox->color.b + 40 > 255) ? 255 : checkbox->color.b + 40;
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    }
    SDL_RenderFillRect(renderer, &checkbox->rect);

    // Border
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &checkbox->rect);

    // If checked, draw an inner mark
    if (checkbox->checked) {
        SDL_Rect inner = {
            checkbox->rect.x + checkbox->rect.w / 4,
            checkbox->rect.y + checkbox->rect.h / 4,
            checkbox->rect.w / 2,
            checkbox->rect.h / 2
        };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &inner);
    }

    // Render label
    if (global_font && checkbox->label) {
        SDL_Surface* text_surf = TTF_RenderText_Blended(global_font, checkbox->label, (SDL_Color){255, 255, 255, 255});
        if (text_surf) {
            SDL_Texture* text_tex = SDL_CreateTextureFromSurface(renderer, text_surf);
            int tw, th;
            SDL_QueryTexture(text_tex, NULL, NULL, &tw, &th);
            
            SDL_Rect text_rect = {
                checkbox->rect.x + checkbox->rect.w + 10,
                checkbox->rect.y + (checkbox->rect.h - th) / 2,
                tw,
                th
            };

            SDL_RenderCopy(renderer, text_tex, NULL, &text_rect);
            SDL_DestroyTexture(text_tex);
            SDL_FreeSurface(text_surf);
        }
    }
}

bool checkbox_handle_event(Checkbox* checkbox, SDL_Event* event) {
    if (event->type == SDL_MOUSEMOTION) {
        int x = event->motion.x;
        int y = event->motion.y;
        checkbox->hovered = (x >= checkbox->rect.x && x <= checkbox->rect.x + checkbox->rect.w &&
                             y >= checkbox->rect.y && y <= checkbox->rect.y + checkbox->rect.h);
    }
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            int x = event->button.x;
            int y = event->button.y;
            if (x >= checkbox->rect.x && x <= checkbox->rect.x + checkbox->rect.w &&
                y >= checkbox->rect.y && y <= checkbox->rect.y + checkbox->rect.h) {
                checkbox->checked = !checkbox->checked;
                return true;
            }
        }
    }
    return false;
}
