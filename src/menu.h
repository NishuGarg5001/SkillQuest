#ifndef MENU_H
#define MENU_H

#include "constants.h"

class Menu
{
    size_t index;
    const std::vector<std::string> items;
    const size_t menu_size, menu_box_width, menu_box_height;
    const float menu_pos_x, menu_pos_y;
    const SDL_FRect menu_box;

    public:
        explicit Menu(std::vector<std::string> items, size_t menu_box_width, size_t menu_box_height) :
        index(0),
        items(std::move(items)),
        menu_size(this->items.size()),
        menu_box_width(menu_box_width),
        menu_box_height(menu_box_height),
        menu_pos_x(static_cast<float>((SCREEN_WIDTH - menu_box_width) / 2)),
        menu_pos_y(static_cast<float>((SCREEN_HEIGHT - menu_box_height) / 2)),
        menu_box{menu_pos_x, menu_pos_y, static_cast<float>(menu_box_width), static_cast<float>(menu_box_height)}
        {}

        std::string_view currentItem() const noexcept
        {
            return items[index];
        }

        void moveUp() noexcept
        {
            if(index > 0)
                index--;
        }

        void moveDown() noexcept
        {
            if(index < menu_size - 1)
                index++;
        }

        void renderMenu(SDL_Renderer* renderer, TTF_Font* font) const
        {
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            SDL_RenderFillRect(renderer, &menu_box);

            for(size_t i=0; i<menu_size; i++)
            {
                std::string text = (i == index) ? "->" + items[i] : items[i];
                SDL_Surface* text_surface = TTF_RenderText_Blended(font, (text).c_str(), strlen(text.c_str()), WHITE);
                SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
                SDL_DestroySurface(text_surface);
                int w, h;
                TTF_GetStringSize(font, text.c_str(), strlen(text.c_str()), &w, &h);
                SDL_FRect dst {menu_box.x + (menu_box.w - w) / 2.0f, menu_box.y + FONT_SIZE * i, static_cast<float>(w), static_cast<float>(h)};
                SDL_RenderTexture(renderer, text_texture, nullptr, &dst);
                SDL_DestroyTexture(text_texture);
            }
        }
};

#endif