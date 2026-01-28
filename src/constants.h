#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>
#include <vector>
#include <deque>
#include <unordered_map>
#include <algorithm>
#include <optional>
#include <array>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

//screen dimensions
constexpr size_t SCREEN_WIDTH  = 1366;
constexpr size_t SCREEN_HEIGHT = 768;

//font
constexpr const char* FONT_PATH = "assets/VT323-Regular.ttf";
constexpr float FONT_SIZE = 24.0f;

//time constants
constexpr Uint64 TICK = 600;
constexpr Uint64 CURSOR_BLINK_TIME = 600;

//menu dimensions
constexpr size_t MAIN_MENU_BOX_WIDTH = 120;
constexpr size_t MAIN_MENU_BOX_HEIGHT = static_cast<size_t>(FONT_SIZE * 3.0f + 0.2f * FONT_SIZE);
constexpr size_t PAUSE_MENU_BOX_WIDTH = 205;
constexpr size_t PAUSE_MENU_BOX_HEIGHT = static_cast<size_t>(FONT_SIZE * 3.0f + 0.2f * FONT_SIZE);
constexpr size_t SAVE_MENU_BOX_WIDTH = 90;
constexpr size_t SAVE_MENU_BOX_HEIGHT = static_cast<size_t>(FONT_SIZE * 3.0f + 0.2f * FONT_SIZE);

//UI dimensions
constexpr size_t INVENTORY_SCALE = 4; //Should take values [1, 2, 4, 8, 16]
constexpr size_t INVENTORY_BOX_WIDTH = 8 * INVENTORY_SCALE;
constexpr size_t INVENTORY_BOX_HEIGHT = 8 * INVENTORY_SCALE;
constexpr size_t INVENTORY_BOXES_PER_ROW = 48 / INVENTORY_SCALE;
constexpr size_t INVENTORY_BOXES_PER_COL = 32 / INVENTORY_SCALE;
constexpr size_t INVENTORY_LINE_WIDTH = 3;
constexpr size_t INVENTORY_END = INVENTORY_BOX_HEIGHT * INVENTORY_BOXES_PER_COL + INVENTORY_LINE_WIDTH * (INVENTORY_BOXES_PER_COL + 1);
constexpr size_t HLINE_OFFSET = SCREEN_HEIGHT - static_cast<size_t>(FONT_SIZE);
constexpr size_t VLINE_OFFSET_RAW = SCREEN_WIDTH - (INVENTORY_BOXES_PER_ROW + 1) * INVENTORY_LINE_WIDTH - (INVENTORY_BOX_WIDTH * INVENTORY_BOXES_PER_ROW) + 1;
constexpr size_t NUM_LINES = HLINE_OFFSET / static_cast<size_t>(FONT_SIZE);
constexpr size_t CURSOR_WIDTH = 8;
constexpr size_t VLINE_OFFSET = VLINE_OFFSET_RAW - CURSOR_WIDTH;
constexpr size_t PROGRESSBAR_PARTITIONS = 30;
constexpr size_t PROGRESSBAR_SPACING = 2;
constexpr size_t PROGRESS_BAR_WIDTH = (SCREEN_WIDTH - VLINE_OFFSET_RAW - (PROGRESSBAR_PARTITIONS - 1) * PROGRESSBAR_SPACING) / 
                                        PROGRESSBAR_PARTITIONS;

//Game constants
constexpr size_t INVENTORY_SIZE = INVENTORY_BOXES_PER_ROW * INVENTORY_BOXES_PER_COL;

//Colors
constexpr SDL_Color WHITE = {255, 255, 255, 255};
constexpr SDL_Color BLACK = {0, 0, 0, 255};
constexpr SDL_Color BROWN = {165, 42, 42, 255};
constexpr SDL_Color YELLOW = {255, 255, 0, 255};
constexpr SDL_Color ORANGE = {255, 165, 0, 255};
constexpr SDL_Color RED = {255, 0, 0, 255};

enum class Rarity
{
    ALWAYS,
    COMMON,
    UNCOMMON,
    RARE,
    VERY_RARE
};

enum class GameState : int
{
    QUIT,
    RUNNING,
    MAIN,
    PAUSE,
    SAVE
};

enum class UIState : int
{
    BLANK,
    UI_INVENTORY,
    UI_PROGRESS
};

enum class PlayerState : int
{
    NONE,
    MINING_STATE
};

enum class ActionVerb : int
{
    INVALID,
    MINE,
    VIEW,
    USE
};

enum class VerbObject : int
{
    NO_OBJECT,
    INVENTORY,
    PROGRESS
};

enum class ResourceName : int
{
    NO_RESOURCE,
    COPPER,
    TIN,
    IRON,
    GOLD
};

enum class ObjectName : int
{
    NO_ITEM,
    COPPER_ORE,
    TIN_ORE,
    IRON_ORE,
    GOLD_ORE
};

enum class Skills : int
{
    NO_SKILL,
    HEALTH,
    MINING
};

using enum Rarity;
using enum ActionVerb;
using enum VerbObject;
using enum PlayerState;
using enum ResourceName;
using enum ObjectName;
using enum Skills;

const std::unordered_map<std::string_view, ActionVerb> action_map = 
{
    {"mine", MINE},
    {"view", VIEW}
};

const std::unordered_map<std::string_view, ResourceName> ores_map = 
{
    {"copper", COPPER},
    {"tin", TIN},
    {"iron", IRON},
    {"gold", GOLD}
};

std::string_view objects_map_inverse(ObjectName obj)
{
    switch(obj)
    {
        case COPPER_ORE: return "copper ore";
        case TIN_ORE: return "tin ore";
        case IRON_ORE: return "iron ore";
        case GOLD_ORE: return "gold ore";
        default: return "null";
    }
}

Skills action_to_skill(ActionVerb action)
{
    switch(action)
    {
        case ActionVerb::MINE: return MINING;
        default: return NO_SKILL;
    }
}

PlayerState skill_to_playerstate(Skills skill)
{
    switch(skill)
    {
        case MINING: return MINING_STATE;
        default: return NONE;
    }
}

Skills playerstate_to_skill(PlayerState player_state)
{
    switch(player_state)
    {
        case MINING_STATE: return MINING;
        default: return NO_SKILL;
    }
}

std::string skill_to_verbose_u(Skills skill)
{
    switch(skill)
    {
        case HEALTH: return "Health";
        case MINING: return "Mining";
        default: return "null";
    }
}

std::string skill_to_verbose_l(Skills skill)
{
    switch(skill)
    {
        case HEALTH: return "health";
        case MINING: return "mining";
        default: return "null";
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