#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>
#include <array>
#include <vector>
#include <deque>
#include <optional>
#include <unordered_map>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

//screen dimensions
constexpr size_t SCREEN_WIDTH  = 1366;
constexpr size_t SCREEN_HEIGHT = 768;

//font
constexpr const char* FONT_PATH = "assets/VT323-Regular.ttf";
constexpr float FONT_SIZE = 24.0f;

//assets
const std::string ASSET_SPRITE_PATH_OBJECTS = "assets/sprites/objects/";
const std::string ASSET_SPRITE_PATH_RESOURCES = "assets/sprites/resources/";

//time constants
constexpr Uint64 TICK = 600;

//menu dimensions
constexpr size_t MAIN_MENU_BOX_WIDTH = 120;
constexpr size_t MAIN_MENU_BOX_HEIGHT = static_cast<size_t>(FONT_SIZE * 3.0f + 0.2f * FONT_SIZE);
constexpr size_t PAUSE_MENU_BOX_WIDTH = 205;
constexpr size_t PAUSE_MENU_BOX_HEIGHT = static_cast<size_t>(FONT_SIZE * 3.0f + 0.2f * FONT_SIZE);
constexpr size_t SAVE_MENU_BOX_WIDTH = 90;
constexpr size_t SAVE_MENU_BOX_HEIGHT = static_cast<size_t>(FONT_SIZE * 3.0f + 0.2f * FONT_SIZE);

//game screen dimensions
constexpr float GS_X = 0;
constexpr float GS_Y = 0;
constexpr float GS_W = static_cast<float>(SCREEN_WIDTH) * 0.7f;
constexpr float GS_H = static_cast<float>(SCREEN_HEIGHT) * 0.8f;

//text screen dimensions
constexpr float TS_X = 0;
constexpr float TS_Y = GS_H;
constexpr float TS_W = GS_W;
constexpr float TS_H = static_cast<float>(SCREEN_HEIGHT) - GS_H;
constexpr size_t NUM_LINES = static_cast<size_t>(TS_H / FONT_SIZE); 

//Icons screen dimensions
constexpr float IS_X = GS_W;
constexpr float IS_Y = 0;
constexpr float IS_W = static_cast<float>(SCREEN_WIDTH) - GS_W;
constexpr float IS_H = static_cast<float>(SCREEN_HEIGHT) * 0.3f;

//UI screen dimensions
constexpr float UIS_X = IS_X;
constexpr float UIS_Y = IS_H;
constexpr float UIS_W = IS_W;
constexpr float UIS_H = static_cast<float>(SCREEN_HEIGHT) - IS_H;

//Grid
constexpr size_t GRID_BOX_HEIGHT = 32;
constexpr size_t GRID_BOX_WIDTH = 32;
constexpr size_t GRID_LINE_WIDTH = 3;
constexpr SDL_Color GRID_BOX_COLOR = {187, 117, 71, 255};
constexpr SDL_Color GRID_LINE_COLOR = {91, 49, 56, 255};

//INVENTORY
constexpr size_t INVENTORY_SIZE = 50;

//Colors
constexpr SDL_Color WHITE = {255, 255, 255, 255};
constexpr SDL_Color BLACK = {0, 0, 0, 255};
constexpr SDL_Color BROWN = {165, 42, 42, 255};
constexpr SDL_Color YELLOW = {255, 255, 0, 255};
constexpr SDL_Color ORANGE = {255, 165, 0, 255};
constexpr SDL_Color RED = {255, 0, 0, 255};

enum class GameState : int
{
    QUIT,
    RUNNING,
    MAIN,
    PAUSE,
    SAVE
};

enum class PlayerState : int
{
    IDLE,
    MINING
};

enum class ResourceName : int
{
    GROUND,
    COPPER,
    TIN,
    IRON,
    GOLD
};

enum class ObjectName : int
{
    STONE,
    STICK,
    COPPER_ORE,
    TIN_ORE,
    IRON_ORE,
    GOLD_ORE
};

enum class Rarity
{
    ALWAYS,
    COMMON,
    UNCOMMON,
    RARE,
    VERY_RARE
};

using enum PlayerState;
using enum ResourceName;
using enum ObjectName;
using enum Rarity;

std::string resource_name_to_string(ResourceName res_name)
{
    switch(res_name)
    {
        case GROUND: return "ground";
        case COPPER: return "copper";
        case TIN: return "tin";
        case IRON: return "iron";
        case GOLD: return "gold";
        default: return "";
    }
}

std::string object_name_to_string(ObjectName obj_name)
{
    switch(obj_name)
    {
        case STONE: return "stone";
        case STICK: return "stick";
        case COPPER_ORE: return "copper ore";
        case TIN_ORE: return "tin ore";
        case IRON_ORE: return "iron ore";
        case GOLD_ORE: return "gold ore";
        default: return "";
    }
}

int level_exp_mapping(int level)
{
    switch(level)
    {
        case 1: return 0;
        case 2: return 83;
        case 3: return 174;
        case 4: return 276;
        case 5: return 388;
        case 6: return 512;
        case 7: return 650;
        case 8: return 801;
        case 9: return 969;
        case 10: return 1154;
        case 11: return 1358;
        default: return 10000000;
    }
}

std::vector<Rarity> drop_rate_to_rarity(const std::vector<int>& drop_rates)
{
    std::vector<Rarity> res{};
    res.reserve(drop_rates.size());
    for(size_t i=0; i<drop_rates.size(); i++)
    {
        int drop_rate = drop_rates[i];
        if(drop_rate > 500)
            res.push_back(VERY_RARE);
        else if(drop_rate > 125)
            res.push_back(RARE);
        else if(drop_rate > 40)
            res.push_back(UNCOMMON);
        else if(drop_rate > 1)
            res.push_back(COMMON);
        else
            res.push_back(ALWAYS);
    }
    return res;
}

std::vector<SDL_Color> rarity_to_color(const std::vector<Rarity>& rarity)
{
    std::vector<SDL_Color> res{};
    res.reserve(rarity.size());
    for(size_t i=0; i<rarity.size(); i++)
    {
        switch(rarity[i])
        {
            case ALWAYS: res.push_back(WHITE); break;
            case COMMON: res.push_back(BROWN); break;
            case UNCOMMON: res.push_back(YELLOW); break;
            case RARE: res.push_back(ORANGE); break;
            case VERY_RARE: res.push_back(RED); break;
            default: res.push_back(WHITE);
        }
    }
    return res;
}

#endif