#ifndef LOCATION_H
#define LOCATION_H

#include "resources.h"

class Location
{
    std::string name;
    const std::vector<Resource> resources;

    public:
        explicit Location(std::string name, std::vector<Resource> resources) : name(name), resources(resources){}

        const std::vector<Resource>& getResources() const noexcept
        {
            return resources;
        }
};

#endif