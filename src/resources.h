#ifndef RESOURCES_H
#define RESOURCES_H

#include "constants.h"

struct Resource
{
    Resources name;
    Skills skill;
    Objects object_name;
    const uint8_t level;
    const uint32_t exp;
    
    explicit Resource(Resources name, Skills skill, Objects object_name, uint8_t level, uint32_t exp) noexcept :
    name(name),
    skill(skill),
    object_name(object_name),
    level(level),
    exp(exp){}
};

struct Object
{
    Objects name;

    explicit Object(Objects name) noexcept : name(name){}
};

#endif