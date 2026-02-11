#ifndef ICON_SCREEN_H
#define ICON_SCREEN_H

#include "screen.h"

class IconScreen : public Screen
{
    public:
        IconScreen(float x, float y, float w, float h) : Screen(x, y, w, h)
        {}

        void render(SDL_Renderer *renderer) const
        {
            renderBox(renderer);
        }
};

#endif