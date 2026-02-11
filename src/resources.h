#ifndef RESOURCES_H
#define RESOURCES_H

#include "constants.h"

struct Object
{
    const ObjectName name;
    const std::string name_str;
    const std::string path;

    explicit Object(ObjectName name, std::string path) :
    name(name),
    name_str(object_name_to_string(name)),
    path(std::move(ASSET_SPRITE_PATH_OBJECTS + path))
    {}
};

struct Resource
{
    const ResourceName name;
    const std::string name_str;
    const std::string path;
    const std::vector<Object> objects;
    const std::vector<int> drop_rates; 
    //minimum levels to extract that object
    //exps gained from extracting that object
    //drop rates for that object
    const std::vector<Rarity> rarities;
    const std::vector<SDL_Color> rarity_colors;
    const size_t len;

    explicit Resource(ResourceName name, std::string path, std::vector<Object> objects, std::vector<int> drop_rates) :
    name(name),
    name_str(resource_name_to_string(name)),
    path(std::move(ASSET_SPRITE_PATH_RESOURCES + path)),
    objects(std::move(objects)),
    drop_rates(std::move(drop_rates)),
    rarities(std::move(drop_rate_to_rarity(this->drop_rates))),
    rarity_colors(std::move(rarity_to_color(this->rarities))),
    len(this->objects.size())
    {}
};

struct DropResult
{
    const ObjectName obj_name;
    const std::string obj_name_str;
    const Rarity rarity;
    SDL_Color rarity_color;

    explicit DropResult(ObjectName obj_name, Rarity rarity, SDL_Color rarity_color) :
    obj_name(obj_name),
    obj_name_str(object_name_to_string(obj_name)),
    rarity(rarity),
    rarity_color(rarity_color)
    {}
};


//actual game resources
//objects
const std::unordered_map<ObjectName, Object> object_list
{
    {STONE, Object(STONE, "stone.png")},
    {STICK, Object(STICK, "stick.png")},
    {COPPER_ORE, Object(COPPER_ORE, "copper_ore.png")},
    {TIN_ORE, Object(TIN_ORE, "tin_ore.png")},
    {IRON_ORE, Object(IRON_ORE, "iron_ore.png")},
    {GOLD_ORE, Object(GOLD_ORE, "gold_ore.png")},
};

#endif