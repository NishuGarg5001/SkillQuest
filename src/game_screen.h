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
    std::array<std::array<std::array<float, 2>, GS_cellsX>, GS_cellsY> resource_box_positions;
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

            float total_grid_width = GS_cellsX * stepX + GRID_LINE_WIDTH;
            float total_grid_height = GS_cellsY * stepY + GRID_LINE_WIDTH;

            float x1 = getX() + (getWidth() - total_grid_width)/2.0f;
            float y1 = getY() + (getHeight() - total_grid_height)/2.0f;

            for(size_t i=0; i<=GS_cellsY; i++)
                for(size_t j=0; j<GS_cellsX; j++)
                    for(size_t k=0; k<GRID_LINE_WIDTH; k++)
                        if(j != GS_cellsX - 1)
                            grid_hlines_params.push_back({x1 + j*stepX, y1 + i*stepY + k, x1 + (j+1)*stepX - 1.0f, y1 + i*stepY + k});
                        else
                            grid_hlines_params.push_back({x1 + j*stepX, y1 + i*stepY +k, x1 + (j+1)*stepX + GRID_LINE_WIDTH - 1.0f, y1 + i*stepY + k});
                
            for(size_t i=0; i<=GS_cellsX; i++)
                for(size_t j=0; j<GS_cellsY; j++)
                    for(size_t k=0; k<GRID_LINE_WIDTH; k++)
                        if(j != GS_cellsY - 1)
                            grid_vlines_params.push_back({x1 + i*stepX + k, y1 + j*stepY, x1 + i*stepX +k, y1 + (j+1)*stepY - 1.0f});
                        else
                            grid_vlines_params.push_back({x1 + i*stepX + k, y1 + j*stepY, x1 + i*stepX + k, y1 + (j+1)*stepY + GRID_LINE_WIDTH - 1.0f});

            for(size_t i=0; i<GS_cellsY; i++)
                for(size_t j=0; j<GS_cellsX; j++)
                {
                    resource_box_positions[i][j][0] = x1 + j*stepX + GRID_LINE_WIDTH;
                    resource_box_positions[i][j][1] = y1 + i*stepY + GRID_LINE_WIDTH;
                }
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
            for(size_t i=0; i<game_resources.size(); i++)
            {
                size_t x = i % GS_cellsX;
                size_t y = i / GS_cellsX;
                SDL_FRect dst = {resource_box_positions[y][x][0], resource_box_positions[y][x][1], static_cast<float>(GRID_BOX_WIDTH), static_cast<float>(GRID_BOX_HEIGHT)};
                SDL_Texture* texture = IMG_LoadTexture(renderer, game_resources[i].path.c_str());
                SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
                SDL_RenderTexture(renderer, texture, nullptr, &dst);
                SDL_DestroyTexture(texture);
            }
        }

        void renderGrid(SDL_Renderer *renderer) const
        {
            SDL_SetRenderDrawColor(renderer, GRID_LINE_COLOR.r, GRID_LINE_COLOR.g, GRID_LINE_COLOR.b, GRID_LINE_COLOR.a);
            for(const auto params : grid_hlines_params)
                SDL_RenderLine(renderer, params[0], params[1], params[2], params[3]);
            for(const auto params : grid_vlines_params)
                SDL_RenderLine(renderer, params[0], params[1], params[2], params[3]);
            SDL_SetRenderDrawColor(renderer, GRID_BOX_COLOR.r, GRID_BOX_COLOR.g, GRID_BOX_COLOR.b, GRID_BOX_COLOR.a);
            for(size_t y=0; y<resource_box_positions.size(); y++)
                for(size_t x=0; x<resource_box_positions[y].size(); x++)
                {
                    SDL_FRect dst = {resource_box_positions[y][x][0], resource_box_positions[y][x][1], static_cast<float>(GRID_BOX_WIDTH), static_cast<float>(GRID_BOX_HEIGHT)};
                    SDL_RenderFillRect(renderer, &dst);
                }
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

        int handleMouseClick(int x, int y)
        {
            float stepX = GRID_LINE_WIDTH + GRID_BOX_WIDTH;
            float stepY = GRID_LINE_WIDTH + GRID_BOX_HEIGHT;

            float total_grid_width = GS_cellsX * stepX + GRID_LINE_WIDTH;
            float total_grid_height = GS_cellsY * stepY + GRID_LINE_WIDTH;

            float x1 = getX() + (getWidth() - total_grid_width)/2.0f;
            float y1 = getY() + (getHeight() - total_grid_height)/2.0f;

            float posx = x - x1;
            float posy = y - y1;

            if(posx < 0 || posy < 0)
                return -1;

            float localX = fmod(posx, stepX);
            float localY = fmod(posy, stepY);

            if (localX < GRID_LINE_WIDTH || localY < GRID_LINE_WIDTH)
                return -1;

            x = static_cast<int>(posx / stepX);
            y = static_cast<int>(posy / stepY);

            if((x >= GS_cellsX) || (y >= GS_cellsY))
                return -1;

            return y * GS_cellsX + x;
        }
};

#endif