#ifndef RESOURCES_H
#define RESOURCES_H

#include "constants.h"

struct Object
{
    const ObjectName name;
    const std::string path;

    explicit Object(ObjectName name, std::string path) :
    name(name),
    path(std::move(ASSET_SPRITE_PATH + path))
    {}
};

struct Tool : Object
{
    const Skills skill;
    explicit Tool(ObjectName name, std::string path, Skills skill) :
    Object(name, path),
    skill(skill)
    {}
};

struct Resource
{
    const ResourceName name;
    const std::vector<Object> objects;
    const std::vector<int> object_levels, exps, drop_rates; 
    //minimum levels to extract that object
    //exps gained from extracting that object
    //drop rates for that object
    const int resource_min_level; //minimum level to use resource = min(object_levels)
    const std::vector<Rarity> rarities;
    const std::vector<SDL_Color> rarity_colors;
    const size_t len;

    explicit Resource(ResourceName name, std::vector<Object> objects, std::vector<int> object_levels,
    std::vector<int> exps, std::vector<int> drop_rates) :
    name(name),
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
    const ObjectName obj_name;
    const Rarity rarity;
    SDL_Color rarity_color;
    const int exp;

    explicit DropResult(ObjectName obj_name, Rarity rarity, SDL_Color rarity_color, int exp) :
    obj_name(obj_name),
    rarity(rarity),
    rarity_color(rarity_color),
    exp(exp)
    {}
};


//actual game resources
//objects
const std::unordered_map<ObjectName, Object> object_list
{
    //{STONE, Object(STONE, "stone.png")},
    //{STICK, Object(STICK, "stick.png")},
    {COPPER_ORE, Object(COPPER_ORE, "copper_ore.png")}
    /*
    {TIN_ORE, Object(COPPER_ORE, "tin_ore.png")},
    {IRON_ORE, Object(COPPER_ORE, "iron_ore.png")},
    {GOLD_ORE, Object(COPPER_ORE, "gold_ore.png")},
    */
};

//name, objects, min_levels, exps, drop_rates
const std::unordered_map<ResourceName, Resource> resource_list
{
    //{GROUND, Resource(GROUND, {object_list.at(STONE), object_list.at(STICK)}, {1, 1}, {4, 4}, {10, 10})},
    {COPPER, Resource(COPPER, {object_list.at(COPPER_ORE)}, {1}, {4}, {10, 10})}
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