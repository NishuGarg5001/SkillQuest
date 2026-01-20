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
    const Object object = Object(resource_to_object_mapping(name));
    const uint16_t drop_rate = object_drop_rates(name, object.name);

    explicit Resource(ResourceName name) noexcept :
    name(name)
    {}
};

struct DropResult
{
    const ObjectName obj_name;
    const Rarity rarity;

    explicit DropResult(ObjectName obj_name, Rarity rarity) noexcept :
    obj_name(obj_name),
    rarity(rarity)
    {}
};

#endif