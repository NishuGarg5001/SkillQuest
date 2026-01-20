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

const std::unordered_map<std::string_view, ActionVerb> action_map = 
{
    {"mine", ActionVerb::MINE}
};

const std::unordered_map<std::string_view, ResourceName> ores_map = 
{
    {"copper", ResourceName::COPPER},
    {"tin", ResourceName::TIN},
    {"iron", ResourceName::IRON},
    {"gold", ResourceName::GOLD}
};

std::string_view objects_map_inverse(ObjectName obj)
{
    switch(obj)
    {
        case ObjectName::COPPER_ORE: return "copper ore";
        case ObjectName::TIN_ORE: return "tin ore";
        case ObjectName::IRON_ORE: return "iron ore";
        case ObjectName::GOLD_ORE: return "gold ore";
        default: return "null";
    }
}

uint8_t resource_min_level(ResourceName resource_name)
{
    switch(resource_name)
    {
        case ResourceName::COPPER: return 1;
        case ResourceName::TIN: return 1;
        case ResourceName::IRON: return 10;
        case ResourceName::GOLD: return 20;
        default: return 100;
    }
}

std::pair<uint8_t, uint32_t> xp_table(Skills skill, ObjectName obj_name)
{
    switch(skill)
    {
        case Skills::MINING:
        switch(obj_name)
        {
            case ObjectName::COPPER_ORE: return {1, 4};
            case ObjectName::TIN_ORE: return {1, 4};
            case ObjectName::IRON_ORE: return {10, 15};
            case ObjectName::GOLD_ORE: return {20, 30};
            default: return {0, 0};
        }
        default: return {0, 0};
    }
}

Skills action_to_skill(ActionVerb action)
{
    switch(action)
    {
        case ActionVerb::MINE: return Skills::MINING;
        default: return Skills::NO_SKILL;
    }
}

PlayerState skill_to_playerstate(Skills skill)
{
    switch(skill)
    {
        case Skills::MINING: return PlayerState::MINING_STATE;
        default: return PlayerState::NONE;
    }
}

Skills playerstate_to_skill(PlayerState player_state)
{
    switch(player_state)
    {
        case PlayerState::MINING_STATE: return Skills::MINING;
        default: return Skills::NO_SKILL;
    }
}

std::string_view skill_to_verbose(Skills skill)
{
    switch(skill)
    {
        case Skills::HEALTH: return "health";
        case Skills::MINING: return "mining";
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

Rarity drop_rate_to_rarity(uint16_t drop_rate)
{
    if(drop_rate > 500)
        return Rarity::VERY_RARE;
    if(drop_rate > 125)
        return Rarity::RARE;
    if(drop_rate > 40)
        return Rarity::UNCOMMON;
    if(drop_rate > 1)
        return Rarity::COMMON;
    return Rarity::ALWAYS;
}

const char* rarity_to_color(Rarity rarity)
{
    switch(rarity)
    {
        case Rarity::ALWAYS: return WHITE;
        case Rarity::COMMON: return BROWN;
        case Rarity::UNCOMMON: return YELLOW;
        case Rarity::RARE: return ORANGE;
        case Rarity::VERY_RARE: return RED;
        default: return WHITE;
    }
}

uint16_t object_drop_rates(ResourceName resource_name, ObjectName obj_name) //drop rate x means object has a 1/x drop rate
{
    switch(resource_name)
    {
        case ResourceName::COPPER:
            switch(obj_name)
            {
                case ObjectName::COPPER_ORE: return 1;
                default: return 1;
            }
        case ResourceName::TIN:
            switch(obj_name)
            {
                case ObjectName::TIN_ORE: return 10;
                default: return 1;
            }
        case ResourceName::IRON:
            switch(obj_name)
            {
                case ObjectName::IRON_ORE: return 15;
                default: return 1;
            }
        case ResourceName::GOLD:
            switch(obj_name)
            {
                case ObjectName::GOLD_ORE: return 20;
                default: return 1;
            }
        default: return 1;
    }
}

ObjectName resource_to_object_mapping(ResourceName resource_name)
{
    switch(resource_name)
    {
        case ResourceName::COPPER: return ObjectName::COPPER_ORE;
        case ResourceName::TIN: return ObjectName::TIN_ORE;
        case ResourceName::IRON: return ObjectName::IRON_ORE;
        case ResourceName::GOLD: return ObjectName::GOLD_ORE;
        default: return ObjectName::NO_ITEM;
    }
}

#endif