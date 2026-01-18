#ifndef RESOURCES_H
#define RESOURCES_H

#include "constants.h"

struct Object
{
    Objects name;

    explicit Object(Objects name) noexcept : name(name){}
};

struct Resource
{
    Resources name;
    Object object;
    int gen_rate; //gen rate = 40 means 1/40 gen rate per tick
    
    explicit Resource(Resources name, Objects object_name, int gen_rate) noexcept :
    name(name),
    object(Object(object_name)),
    gen_rate(gen_rate)
    {}
};

#endif