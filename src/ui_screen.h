#ifndef UI_SCREEN_H
#define UI_SCREEN_H

#include "screen.h"

enum class UIState
{
    NONE,
    INVENTORY,
    PROGRESS,
};

class UIScreen : public Screen
{
    UIState state = UIState::NONE;
    std::vector<std::array<float, 2>> inventory_box_positions;
    std::vector<std::array<float, 4>> grid_hlines_params;
    std::vector<std::array<float, 4>> grid_vlines_params;

    public:
        UIScreen(float x, float y, float w, float h) : Screen(x, y, w, h)
        {
            float stepX = GRID_LINE_WIDTH + GRID_BOX_WIDTH;
            float stepY = GRID_LINE_WIDTH + GRID_BOX_HEIGHT;

            size_t cellsX = static_cast<size_t>((getWidth() - GRID_LINE_WIDTH)/stepX);
            size_t cellsY = static_cast<size_t>((getHeight() - GRID_LINE_WIDTH)/stepY);

            float total_grid_width = cellsX * stepX + GRID_LINE_WIDTH;
            float total_grid_height = cellsY * stepY + GRID_LINE_WIDTH;

            float x1 = getX() + (getWidth() - total_grid_width)/2.0f;
            float y1 = getY() + (getHeight() - total_grid_height)/2.0f;

            for(size_t i=0; i<=cellsY; i++)
                for(size_t j=0; j<cellsX; j++)
                    for(size_t k=0; k<GRID_LINE_WIDTH; k++)
                        if(j != cellsX - 1)
                            grid_hlines_params.push_back({x1 + j*stepX, y1 + i*stepY + k, x1 + (j+1)*stepX - 1.0f, y1 + i*stepY + k});
                        else
                            grid_hlines_params.push_back({x1 + j*stepX, y1 + i*stepY +k, x1 + (j+1)*stepX + GRID_LINE_WIDTH - 1.0f, y1 + i*stepY + k});
                
            for(size_t i=0; i<=cellsX; i++)
                for(size_t j=0; j<cellsY; j++)
                    for(size_t k=0; k<GRID_LINE_WIDTH; k++)
                        if(j != cellsY - 1)
                            grid_vlines_params.push_back({x1 + i*stepX + k, y1 + j*stepY, x1 + i*stepX +k, y1 + (j+1)*stepY - 1.0f});
                        else
                            grid_vlines_params.push_back({x1 + i*stepX + k, y1 + j*stepY, x1 + i*stepX + k, y1 + (j+1)*stepY + GRID_LINE_WIDTH - 1.0f});
            
            size_t counter = 0;
            for(size_t i=0; i<cellsY; i++)
                for(size_t j=0; j<cellsX; j++)
                    inventory_box_positions.push_back({x1 + j*stepX + GRID_LINE_WIDTH, y1 + i*stepY + GRID_LINE_WIDTH});
        }

        void setState(UIState new_state) noexcept
        {
            state = new_state;
        }

        void render(SDL_Renderer *renderer, const Player& player) const
        {
            renderBox(renderer);
            switch(state)
            {
                case UIState::NONE:
                    break;
                case UIState::INVENTORY:
                {
                    renderInventory(renderer, player);
                    break;
                }
                case UIState::PROGRESS:
                {
                    renderProgress(renderer);
                    break;
                }
                default:
                    break;
            }
        }

        void renderInventory(SDL_Renderer *renderer, const Player& player) const
        {
            renderGrid(renderer);
            SDL_SetRenderDrawColor(renderer, GRID_BOX_COLOR.r, GRID_BOX_COLOR.g, GRID_BOX_COLOR.b, GRID_BOX_COLOR.a);
            const auto& inventory = player.getInventory();
            for(size_t i=0; i<inventory_box_positions.size(); i++)
            {
                SDL_FRect dst = {inventory_box_positions[i][0], inventory_box_positions[i][1], static_cast<float>(GRID_BOX_WIDTH), static_cast<float>(GRID_BOX_HEIGHT)};
                SDL_RenderFillRect(renderer, &dst);
                if(i < inventory.size() && inventory[i].has_value())
                {
                    SDL_Texture* texture = IMG_LoadTexture(renderer, inventory[i]->path.c_str());
                    SDL_RenderTexture(renderer, texture, nullptr, &dst);
                    SDL_DestroyTexture(texture);
                }
            }
        }

        void renderGrid(SDL_Renderer *renderer) const
        {
            SDL_SetRenderDrawColor(renderer, GRID_LINE_COLOR.r, GRID_LINE_COLOR.g, GRID_LINE_COLOR.b, GRID_LINE_COLOR.a);
            for(const auto params : grid_hlines_params)
                SDL_RenderLine(renderer, params[0], params[1], params[2], params[3]);
            for(const auto params : grid_vlines_params)
                SDL_RenderLine(renderer, params[0], params[1], params[2], params[3]);
        }

        void renderProgress(SDL_Renderer *renderer) const
        {

        }
};

#endif