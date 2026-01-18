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

constexpr uint8_t SCREEN_WIDTH  = 120;
constexpr uint8_t SCREEN_HEIGHT = 29;
constexpr uint8_t COMMAND_SCREEN_WIDTH = 99;
constexpr uint8_t UI_SCREEN_WIDTH = SCREEN_WIDTH - (COMMAND_SCREEN_WIDTH + 1);
constexpr uint8_t HISTORY_LENGTH = SCREEN_HEIGHT - 2;
constexpr uint8_t INVENTORY_SIZE = 28;
constexpr auto tick = std::chrono::milliseconds(600);
constexpr const char* RED    = "\x1b[31m";
constexpr const char* GREEN  = "\x1b[32m";
constexpr const char* BROWN = "\x1b[33m";
constexpr const char* BLUE   = "\x1b[34m";
constexpr const char* MAGENTA= "\x1b[35m";
constexpr const char* CYAN   = "\x1b[36m";
constexpr const char* WHITE  = "\x1b[37m";

using high_clock = std::chrono::high_resolution_clock;

enum InputKey
{
    KEY_EXTENDED1 = 0,
    KEY_EXTENDED2 = 224,
    UP = 72,
    DOWN = 80,
    ENTER = 13,
    ESC = 27
};

enum GameState : std::uint8_t
{
    QUIT,
    RUNNING,
    MAIN,
    PAUSE,
    SAVE
};

enum PlayerState : std::uint8_t
{
    NONE,
    MINING_STATE
};

enum ActionVerb : std::uint8_t
{
    NO_ACTION,
    MINE
};

enum Resources : std::uint8_t
{
    NO_RESOURCE,
    COPPER,
    TIN,
    IRON,
    GOLD
};

enum Objects : std::uint8_t
{
    NO_ITEM,
    COPPER_ORE,
    TIN_ORE,
    IRON_ORE,
    GOLD_ORE
};

enum Skills : std::uint8_t
{
    HEALTH,
    MINING
};

const std::unordered_map<std::string_view, ActionVerb> action_map = 
{
    {"mine", MINE}
};

const std::unordered_map<std::string_view, Resources> ores_map = 
{
    {"copper", COPPER},
    {"tin", TIN},
    {"iron", IRON},
    {"gold", GOLD}
};

const std::unordered_map<Objects, std::string> objects_map_inverse = 
{
    {COPPER_ORE, "copper ore"},
    {TIN_ORE, "tin ore"},
    {IRON_ORE, "iron ore"},
    {GOLD_ORE, "gold ore"}
};

const std::unordered_map<Resources, uint8_t> resource_min_level = 
{
    {COPPER, 1},
    {TIN, 1},
    {IRON, 10},
    {GOLD, 20}
};

const std::unordered_map<Skills, std::unordered_map<Objects, std::pair<uint8_t, uint32_t>>> xp_table
{
    {MINING,
        {
            {COPPER_ORE, {1, 4}},
            {TIN_ORE, {1, 4}},
            {IRON_ORE, {10, 15}},
            {GOLD_ORE, {20, 30}}
        }
    }
};

const std::unordered_map<ActionVerb, Skills> action_to_skill
{
    {MINE, MINING}
};

const std::unordered_map<Skills, PlayerState> skill_to_playerstate
{
    {MINING, MINING_STATE}
};

const std::unordered_map<PlayerState, Skills> playerstate_to_skill
{
    {MINING_STATE, MINING}
};

const std::unordered_map<Skills, std::string> skill_to_verbose
{
    {MINING, "mining"}
};

const std::unordered_map<uint8_t, uint32_t> level_exp_mapping
{
    {1, 0},
    {2, 83},
    {3, 170}
};

#endif