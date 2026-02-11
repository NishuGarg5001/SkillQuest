#ifndef GAME_SCREEN_H
#define GAME_SCREEN_H

#include "screen.h"
#include "resources.h"
#include "player.h"
#include "random.h"

enum class GameScreenState
{
    RESOURCES
};

class GameScreen : public Screen
{
    const Resource *player_resource_target = nullptr;
    std::vector<std::array<float, 4>> grid_hlines_params;
    std::vector<std::array<float, 4>> grid_vlines_params;
    std::vector<std::array<float, 2>> resource_box_positions;
    GameScreenState state = GameScreenState::RESOURCES;
    const std::array<Resource, 4> game_resources = 
    {
        Resource(COPPER, "copper.png", {object_list.at(COPPER_ORE)}, {5}),
        Resource(TIN, "tin.png", {object_list.at(TIN_ORE)}, {5}),
        Resource(IRON, "iron.png", {object_list.at(IRON_ORE)}, {5}),
        Resource(GOLD, "gold.png", {object_list.at(GOLD_ORE)}, {5})
    };

    public:
        GameScreen(float x, float y, float w, float h) : Screen(x, y, w, h)
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
                    resource_box_positions.push_back({x1 + j*stepX + GRID_LINE_WIDTH, y1 + i*stepY + GRID_LINE_WIDTH});
        }

        void render(SDL_Renderer *renderer) const
        {
            renderBox(renderer);
            switch(state)
            {
                case GameScreenState::RESOURCES:
                {
                    renderResources(renderer);
                    break;
                }
                default:
                    break;
            }
        }

        void renderResources(SDL_Renderer *renderer) const
        {
            renderGrid(renderer);
            SDL_SetRenderDrawColor(renderer, GRID_BOX_COLOR.r, GRID_BOX_COLOR.g, GRID_BOX_COLOR.b, GRID_BOX_COLOR.a);
            for(size_t i=0; i<resource_box_positions.size(); i++)
            {
                SDL_FRect dst = {resource_box_positions[i][0], resource_box_positions[i][1], static_cast<float>(GRID_BOX_WIDTH), static_cast<float>(GRID_BOX_HEIGHT)};
                SDL_RenderFillRect(renderer, &dst);
                if(i < game_resources.size())
                {
                    SDL_Texture* texture = IMG_LoadTexture(renderer, game_resources[i].path.c_str());
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

        bool setPlayerTarget(ResourceName item)
        {
            for(const auto& it : game_resources)
                if(it.name == item)
                {
                    player_resource_target = &it;
                    return true;
                }
            player_resource_target = nullptr;
            return false;
        }

        const Resource* getPlayerTarget() const noexcept
        {
            return player_resource_target;
        }

        std::vector<DropResult> extractResource(Player& player) 
        //Extracts resource and adds it to inventory, returns name to updateState for verbose
        {
            if(player_resource_target == nullptr)
                return {};
            //handle drop_rate = 0 case
            std::vector<DropResult> res = {};
            res.reserve(player_resource_target->len);
            for (size_t i=0; i<player_resource_target->len; i++)
            {
                if(random_int(0, player_resource_target->drop_rates[i] - 1) == 0)
                        if(player.addItem(player_resource_target->objects[i]))
                            res.emplace_back(player_resource_target->objects[i].name, player_resource_target->rarities[i], player_resource_target->rarity_colors[i]);
            }
            if(!res.empty())
                return res;
            return {};
        }

        void stopExtraction()
        {
            player_resource_target = nullptr;
        }
};

#endif