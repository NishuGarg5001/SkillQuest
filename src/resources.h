#ifndef RESOURCES_H
#define RESOURCES_H

#include "constants.h"

struct Object
{
    const std::string name;
    const std::string path;

    explicit Object(std::string name, std::string path) :
    name(std::move(name)),
    path(std::move(ASSET_SPRITE_PATH + path))
    {}
};

struct Tool : Object
{
    const std::string skill;
    explicit Tool(std::string name, std::string path, std::string skill) :
    Object(std::move(name), std::move(path)),
    skill(std::move(skill))
    {}
};

struct Resource
{
    const std::string name;
    const std::vector<Object> objects;
    const std::vector<int> object_levels, exps, drop_rates; 
    //minimum levels to extract that object
    //exps gained from extracting that object
    //drop rates for that object
    const int resource_min_level; //minimum level to use resource = min(object_levels)
    const std::vector<Rarity> rarities;
    const std::vector<SDL_Color> rarity_colors;
    const size_t len;

    explicit Resource(std::string name, std::vector<Object> objects, std::vector<int> object_levels,
    std::vector<int> exps, std::vector<int> drop_rates) :
    name(std::move(name)),
    objects(std::move(objects)),
    object_levels(std::move(object_levels)),
    resource_min_level(*std::min_element(this->object_levels.begin(), this->object_levels.end())),
    exps(std::move(exps)),
    drop_rates(std::move(drop_rates)),
    rarities(std::move(drop_rate_to_rarity(this->drop_rates))),
    rarity_colors(std::move(rarity_to_color(this->rarities))),
    len(this->objects.size())
    {}
};

struct DropResult
{
    const std::string obj_name;
    const Rarity rarity;
    SDL_Color rarity_color;
    const int exp;

    explicit DropResult(std::string obj_name, Rarity rarity, SDL_Color rarity_color, int exp) :
    obj_name(std::move(obj_name)),
    rarity(rarity),
    rarity_color(rarity_color),
    exp(exp)
    {}
};


//actual game resources
//objects
const std::unordered_map<std::string, Object> object_list
{
    {"stone", Object("stone", "stone.png")},
    {"stick", Object("stick", "stick.png")},
    {"copper ore", Object("copper ore", "copper_ore.png")}
    /*
    {TIN_ORE, Object(COPPER_ORE, "tin_ore.png")},
    {IRON_ORE, Object(COPPER_ORE, "iron_ore.png")},
    {GOLD_ORE, Object(COPPER_ORE, "gold_ore.png")},
    */
};

//name, objects, min_levels, exps, drop_rates
const std::unordered_map<std::string, Resource> resource_list
{
    {"ground", Resource("ground", {object_list.at("stone"), object_list.at("stick")}, {1, 1}, {4, 4}, {10, 10})},
    {"copper", Resource("copper", {object_list.at("copper ore")}, {1}, {4}, {10})}
};
/*
    {TIN, Resource(TIN, {object_list.at(TIN_ORE)}, {1}, {4}, {5})},
    {IRON, Resource(IRON, {object_list.at(IRON_ORE)}, {10}, {20}, {10})},
    {GOLD, Resource(GOLD, {object_list.at(GOLD_ORE)}, {20}, {30}, {15})}
};
*/

//tools
/*
const std::unordered_map<ObjectName, Tool> tool_list
{
    {STONE_PICKAXE, Tool(STONE_PICKAXE, "stone_pickaxe.png", MINING)}
}
*/

#endif