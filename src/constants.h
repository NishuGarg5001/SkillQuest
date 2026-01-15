#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>
#include <chrono>
#include <unordered_map>
#include <optional>
#include <conio.h>
#include <thread>

constexpr size_t SCREEN_WIDTH  = 120;
constexpr size_t SCREEN_HEIGHT = 27;
constexpr auto tick = std::chrono::milliseconds(600);
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

const std::unordered_map<std::string_view, ActionVerb> action_map = {
    {"mine", MINE}
};

const std::unordered_map<std::string_view, Resources> ores_map = {
    {"copper", COPPER},
    {"tin", TIN},
    {"iron", IRON},
    {"gold", GOLD}
};

const std::unordered_map<Objects, std::string_view> objects_map_inverse = {
    {COPPER_ORE, "copper ore"},
    {TIN_ORE, "tin ore"},
    {IRON_ORE, "iron ore"},
    {GOLD_ORE, "gold ore"}
};

#endif