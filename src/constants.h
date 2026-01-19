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

constexpr size_t SCREEN_WIDTH  = 120;
constexpr size_t SCREEN_HEIGHT = 29;
constexpr size_t COMMAND_SCREEN_WIDTH = 79;
constexpr size_t UI_SCREEN_WIDTH = SCREEN_WIDTH - (COMMAND_SCREEN_WIDTH + 1);
constexpr size_t HISTORY_LENGTH = SCREEN_HEIGHT - 2;
constexpr size_t INVENTORY_SIZE = 28;
constexpr auto tick = std::chrono::milliseconds(600);
constexpr const char* RED    = "\x1b[31m";
constexpr const char* GREEN  = "\x1b[32m";
constexpr const char* BROWN = "\x1b[33m";
constexpr const char* BLUE   = "\x1b[34m";
constexpr const char* MAGENTA= "\x1b[35m";
constexpr const char* CYAN   = "\x1b[36m";
constexpr const char* WHITE  = "\x1b[37m";

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
    MINING_STATE
};

enum class ActionVerb : uint8_t
{
    NO_ACTION,
    MINE,
    USE
};

enum class Resources : uint8_t
{
    NO_RESOURCE,
    COPPER,
    TIN,
    IRON,
    GOLD
};

enum class Objects : uint8_t
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

const std::unordered_map<std::string_view, Resources> ores_map = 
{
    {"copper", Resources::COPPER},
    {"tin", Resources::TIN},
    {"iron", Resources::IRON},
    {"gold", Resources::GOLD}
};

std::string_view objects_map_inverse(Objects obj)
{
    switch(obj)
    {
        case Objects::COPPER_ORE: return "copper ore";
        case Objects::TIN_ORE: return "tin ore";
        case Objects::IRON_ORE: return "iron ore";
        case Objects::GOLD_ORE: return "gold ore";
        default: return "null";
    }
}

uint8_t resource_min_level(Resources resource)
{
    switch(resource)
    {
        case Resources::COPPER: return 1;
        case Resources::TIN: return 1;
        case Resources::IRON: return 10;
        case Resources::GOLD: return 20;
        default: return 100;
    }
}

std::pair<uint8_t, uint32_t> xp_table(Skills skill, Objects obj)
{
    switch(skill)
    {
        case Skills::MINING:
        switch(obj)
        {
            case Objects::COPPER_ORE: return {1, 4};
            case Objects::TIN_ORE: return {1, 4};
            case Objects::IRON_ORE: return {10, 15};
            case Objects::GOLD_ORE: return {20, 30};
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
        case 3: return 170;
        case 4: return 400;
        default: return 10000000;
    }
}

#endif