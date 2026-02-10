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
#include <SDL3_image/SDL_image.h>

//screen dimensions
constexpr size_t SCREEN_WIDTH  = 1366;
constexpr size_t SCREEN_HEIGHT = 768;
constexpr size_t TEXT_LINES = 32; // includes 1 line for commands

//font
constexpr const char* FONT_PATH = "assets/VT323-Regular.ttf";
constexpr float FONT_SIZE = static_cast<float>(SCREEN_HEIGHT/TEXT_LINES);

//assets
constexpr std::string ASSET_SPRITE_PATH = "assets/sprites/";

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
constexpr size_t UI_SCALE = 4; //Should take values [1, 2, 4, 8, 16]
constexpr size_t LINE_WIDTH = 4; //LINE WIDTH SHOULD BE > 1

//inventory dimensions
constexpr size_t INVENTORY_BOX_WIDTH = 8 * UI_SCALE;
constexpr size_t INVENTORY_BOX_HEIGHT = 8 * UI_SCALE;
constexpr size_t INVENTORY_BOXES_PER_ROW = 36 / UI_SCALE; //48
constexpr size_t INVENTORY_BOXES_PER_COL = 24 / UI_SCALE; //32
constexpr size_t INVENTORY_END = INVENTORY_BOX_HEIGHT * INVENTORY_BOXES_PER_COL + LINE_WIDTH * (INVENTORY_BOXES_PER_COL + 1);

constexpr size_t INVENTORY_VAULT_V_OFFSET = 10;

//vault dimensions
constexpr size_t VAULT_BOX_WIDTH = INVENTORY_BOX_WIDTH;
constexpr size_t VAULT_BOX_HEIGHT = INVENTORY_BOX_HEIGHT;
constexpr size_t VAULT_BOXES_PER_ROW = INVENTORY_BOXES_PER_ROW;
constexpr size_t VAULT_BOXES_PER_COL = INVENTORY_BOXES_PER_COL + UI_SCALE;
constexpr size_t VAULT_END = INVENTORY_END + INVENTORY_VAULT_V_OFFSET + VAULT_BOX_HEIGHT * VAULT_BOXES_PER_COL + LINE_WIDTH * (VAULT_BOXES_PER_COL + 1);

constexpr size_t HLINE_OFFSET = SCREEN_HEIGHT - static_cast<size_t>(FONT_SIZE);
constexpr size_t VLINE_OFFSET_RAW = SCREEN_WIDTH - (INVENTORY_BOXES_PER_ROW + 1) * LINE_WIDTH - (INVENTORY_BOX_WIDTH * INVENTORY_BOXES_PER_ROW) + 1;
constexpr size_t CURSOR_WIDTH = 8;
constexpr size_t VLINE_OFFSET = VLINE_OFFSET_RAW - CURSOR_WIDTH;
//constexpr size_t NUM_LINES = HLINE_OFFSET / static_cast<size_t>(FONT_SIZE);

constexpr size_t PROGRESSBAR_PARTITIONS = 30;
constexpr size_t PROGRESSBAR_SPACING = 2;
constexpr size_t PROGRESS_BAR_WIDTH = (SCREEN_WIDTH - VLINE_OFFSET_RAW - (PROGRESSBAR_PARTITIONS - 1) * PROGRESSBAR_SPACING) / 
                                        PROGRESSBAR_PARTITIONS;

//Game constants
constexpr size_t INVENTORY_SIZE = INVENTORY_BOXES_PER_ROW * INVENTORY_BOXES_PER_COL;
constexpr size_t VAULT_SIZE = VAULT_BOXES_PER_ROW * VAULT_BOXES_PER_COL;
constexpr size_t NUM_TOOLS = 1;
constexpr std::string invalid_ui_state = "";
constexpr std::string invalid_target = "";
constexpr std::string invalid_action = "";
const std::array<std::string, 5> valid_actions = {"stop", "mine", "search", "view", "deposit"};
const std::array<std::string, 1> valid_forage_sources = {"ground"};
const std::array<std::string, 4> valid_mining_sources = {"copper", "tin", "iron", "gold"};
const std::array<std::string, 3> skills_list = {"health", "foraging", "mining"};
const std::array<std::string, 3> valid_ui_states = {"progress", "inventory", "vault"};
const std::array<int, 3> starting_levels = {10, 1, 1};

//item list
const std::array<std::string, 6> valid_items = {"stone", "stick", "copper ore", "tin ore", "iron ore", "gold ore"};

//Colors
constexpr SDL_Color WHITE = {255, 255, 255, 255};
constexpr SDL_Color BLACK = {0, 0, 0, 255};
constexpr SDL_Color BROWN = {165, 42, 42, 255};
constexpr SDL_Color YELLOW = {255, 255, 0, 255};
constexpr SDL_Color ORANGE = {255, 165, 0, 255};
constexpr SDL_Color RED = {255, 0, 0, 255};
constexpr SDL_Color INVENTORY_BOX_COLOR = {187, 117, 71, 255};
constexpr SDL_Color INVENTORY_LINE_COLOR = {91, 49, 56, 255};

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

using enum Rarity;

std::string action_to_skill(std::string_view action)
{
    if(action == "search")
        return "foraging";
    if(action == "mine")
        return "mining";
    return invalid_action;
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