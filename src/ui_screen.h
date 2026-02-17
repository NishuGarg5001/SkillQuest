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
    std::array<std::array<std::array<float, 2>, UI_cellsX>, UI_cellsY> inventory_box_positions;
    std::vector<std::array<float, 4>> grid_hlines_params;
    std::vector<std::array<float, 4>> grid_vlines_params;

    public:
        UIScreen(float x, float y, float w, float h) : Screen(x, y, w, h)
        {
            float stepX = GRID_LINE_WIDTH + GRID_BOX_WIDTH;
            float stepY = GRID_LINE_WIDTH + GRID_BOX_HEIGHT;

            float total_grid_width = UI_cellsX * stepX + GRID_LINE_WIDTH;
            float total_grid_height = UI_cellsY * stepY + GRID_LINE_WIDTH;

            float x1 = getX() + (getWidth() - total_grid_width)/2.0f;
            float y1 = getY() + (getHeight() - total_grid_height)/2.0f;

            size_t i_limit = (INVENTORY_SIZE / UI_cellsX) + 1;
            size_t j_limit = (INVENTORY_SIZE % UI_cellsX);

            //hlines
            for(size_t i=0; i<i_limit; i++)
                for(size_t j=0; j<UI_cellsX; j++)
                    for(size_t k=0; k<GRID_LINE_WIDTH; k++)
                        if(j != UI_cellsX - 1)
                            grid_hlines_params.push_back({x1 + j*stepX, y1 + i*stepY + k, x1 + (j+1)*stepX - 1.0f, y1 + i*stepY + k});
                        else
                            grid_hlines_params.push_back({x1 + j*stepX, y1 + i*stepY +k, x1 + (j+1)*stepX + GRID_LINE_WIDTH - 1.0f, y1 + i*stepY + k});

            size_t i = i_limit;

            for(size_t j=0; j<j_limit; j++)
                for(size_t k=0; k<GRID_LINE_WIDTH; k++)
                    if(j != j_limit - 1)
                        grid_hlines_params.push_back({x1 + j*stepX, y1 + i*stepY + k, x1 + (j+1)*stepX - 1.0f, y1 + i*stepY + k});
                    else
                        grid_hlines_params.push_back({x1 + j*stepX, y1 + i*stepY +k, x1 + (j+1)*stepX + GRID_LINE_WIDTH - 1.0f, y1 + i*stepY + k});
            //hlines

            //vlines
            i_limit = (INVENTORY_SIZE % UI_cellsX);
            j_limit = (INVENTORY_SIZE / UI_cellsX);

            for(size_t j=0; j<j_limit; j++)
                for(size_t i=0; i<=UI_cellsX; i++)
                    for(size_t k=0; k<GRID_LINE_WIDTH; k++)
                        grid_vlines_params.push_back({x1 + i*stepX + k, y1 + j*stepY, x1 + i*stepX +k, y1 + (j+1)*stepY - 1.0f});
                        
            size_t j = j_limit;
            for(size_t i=0; i<=i_limit; i++)
                for(size_t k=0; k<GRID_LINE_WIDTH; k++)
                    grid_vlines_params.push_back({x1 + i*stepX + k, y1 + j*stepY, x1 + i*stepX + k, y1 + (j+1)*stepY + GRID_LINE_WIDTH - 1.0f});
            //vlines
            
            for(size_t i=0; i<UI_cellsY; i++)
                for(size_t j=0; j<UI_cellsX; j++)
                {
                    inventory_box_positions[i][j][0] = x1 + j*stepX + GRID_LINE_WIDTH;
                    inventory_box_positions[i][j][1] = y1 + i*stepY + GRID_LINE_WIDTH;
                }
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
            for(size_t i=0; i<inventory.size(); i++)
            {
                size_t x = i % UI_cellsX;
                size_t y = i / UI_cellsX;
                SDL_FRect dst = {inventory_box_positions[y][x][0], inventory_box_positions[y][x][1], static_cast<float>(GRID_BOX_WIDTH), static_cast<float>(GRID_BOX_HEIGHT)};
                SDL_RenderFillRect(renderer, &dst);
                if(inventory[i].has_value())
                {
                    SDL_Texture* texture = IMG_LoadTexture(renderer, inventory[i]->path.c_str());
                    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
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