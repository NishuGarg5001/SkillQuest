#ifndef SCREEN_H
#define SCREEN_H

#include "constants.h"

class Screen
{
    SDL_FRect rect;

    public:
        explicit Screen(float x, float y, float w, float h) : rect{x, y, w, h}
        {}

        float getX() const noexcept
        {
            return rect.x;
        }

        float getY() const noexcept
        {
            return rect.y;
        }

        float getWidth() const noexcept
        {
            return rect.w;
        }

        float getHeight() const noexcept
        {
            return rect.h;
        }

        void renderBox(SDL_Renderer *renderer, SDL_Color color = WHITE) const
        {
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderRect(renderer, &rect);
        }
};

#endif