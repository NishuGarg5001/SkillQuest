#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>
#include <chrono>
#include <unordered_map>
#include <optional>
#include <conio.h>
#include <thread>
#include <deque>
#include <array>
#include <vector>
#include <string>
#include <variant>
#include <cstring>
#include <algorithm>

constexpr size_t SCREEN_WIDTH  = 120;
constexpr size_t SCREEN_HEIGHT = 29;
constexpr size_t COMMAND_SCREEN_WIDTH = 80;
constexpr size_t UI_SCREEN_WIDTH = SCREEN_WIDTH - (COMMAND_SCREEN_WIDTH + 1);
constexpr size_t HISTORY_LENGTH = SCREEN_HEIGHT - 2;
constexpr size_t INVENTORY_SIZE = 28;
constexpr size_t MAIN_MENU_BOX_WIDTH = 13;
constexpr size_t PAUSE_MENU_BOX_WIDTH = 21;
constexpr size_t SAVE_MENU_BOX_WIDTH = 10;
constexpr size_t PROGRESSBAR_PARTITIONS = 30;
constexpr auto tick = std::chrono::milliseconds(600);
constexpr const char* RESET = "\x1b[0m";
constexpr const char* RED = "\x1b[31m";
constexpr const char* GREEN = "\x1b[32m";
constexpr const char* BROWN = "\x1b[33m";
constexpr const char* YELLOW = "\x1b[93m";
constexpr const char* BLUE = "\x1b[34m";
constexpr const char* MAGENTA = "\x1b[35m";
constexpr const char* CYAN = "\x1b[36m";
constexpr const char* WHITE = "\x1b[37m";
constexpr const char* ORANGE = "\x1b[38;5;208m";
using high_clock = std::chrono::high_resolution_clock;

namespace CommandKey
{
    enum : uint8_t
    {
        RAW1 = 0,
        RAW2 = 224,
        UP = 72,
        DOWN = 80,
        ENTER = 13,
        ESC = 27
    };
};

namespace TextualKey
{
    enum : uint8_t
    {
        BACKSPACE = 8
    };
};

enum class Rarity
{
    ALWAYS,
    COMMON,
    UNCOMMON,
    RARE,
    VERY_RARE
};

enum class GameState : uint8_t
{
    QUIT,
    RUNNING,
    MAIN,
    PAUSE,
    SAVE
};

enum class PlayerState : uint8_t
{
    NONE,
    INVENTORY,
    MINING_STATE
};

enum class ActionVerb : uint8_t
{
    NO_ACTION,
    MINE,
    VIEW,
    USE
};

enum class ResourceName : uint8_t
{
    NO_RESOURCE,
    COPPER,
    TIN,
    IRON,
    GOLD
};

enum class ObjectName : uint8_t
{
    NO_ITEM,
    COPPER_ORE,
    TIN_ORE,
    IRON_ORE,
    GOLD_ORE
};

enum class Skills : uint8_t
{
    NO_SKILL,
    HEALTH,
    MINING
};

using enum Rarity;
using enum ActionVerb;
using enum PlayerState;
using enum ResourceName;
using enum ObjectName;
using enum Skills;

const std::unordered_map<std::string_view, ActionVerb> action_map = 
{
    {"mine", MINE}
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

std::string_view skill_to_verbose(Skills skill)
{
    switch(skill)
    {
        case HEALTH: return "health";
        case MINING: return "mining";
        default: return "null";
    }
}

uint32_t level_exp_mapping(uint8_t level)
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

std::vector<Rarity> drop_rate_to_rarity(const std::vector<uint16_t>& drop_rates)
{
    std::vector<Rarity> res{};
    res.reserve(drop_rates.size());
    for(size_t i=0; i<drop_rates.size(); i++)
    {
        uint16_t drop_rate = drop_rates[i];
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

std::vector<const char*> rarity_to_color(const std::vector<Rarity>& rarity)
{
    std::vector<const char*> res{};
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