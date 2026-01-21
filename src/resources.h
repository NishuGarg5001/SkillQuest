#ifndef RESOURCES_H
#define RESOURCES_H

#include "constants.h"

struct Object
{
    const ObjectName name;

    explicit Object(ObjectName name) noexcept :
    name(name)
    {}
};

struct Resource
{
    const ResourceName name;
    const std::vector<Object> objects;
    const std::vector<uint8_t> object_levels; //minimum levels to extract that object
    const uint8_t resource_min_level; //minimum level to use resource = min(object_levels)
    const std::vector<uint32_t> exps;
    const std::vector<uint16_t> drop_rates;
    const std::vector<Rarity> rarities;
    const std::vector<const char*> rarity_colors;
    const size_t len;

    explicit Resource(ResourceName name, std::vector<Object> objects, std::vector<uint8_t> object_levels,
    std::vector<uint32_t> exps, std::vector<uint16_t> drop_rates) 
    noexcept :
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
    const char* rarity_color;
    const uint32_t exp;

    explicit DropResult(ObjectName obj_name, Rarity rarity, const char* rarity_color, uint32_t exp) noexcept :
    obj_name(obj_name),
    rarity(rarity),
    rarity_color(rarity_color),
    exp(exp)
    {}
};

//name, skill, objects, min_levels, exps, drop_rates
const std::unordered_map<ResourceName, Resource> resource_list
{
    {COPPER, Resource(COPPER, {Object(COPPER_ORE)}, {1}, {4}, {5})},
    {TIN, Resource(TIN, {Object(TIN_ORE)}, {1}, {4}, {5})},
    {IRON, Resource(IRON, {Object(IRON_ORE)}, {10}, {20}, {10})},
    {GOLD, Resource(GOLD, {Object(GOLD_ORE)}, {20}, {30}, {15})}
};

#endif