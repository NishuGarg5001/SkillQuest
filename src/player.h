#ifndef PLAYER_H
#define PLAYER_H

#include "location.h"

class Player 
{
    std::uint8_t health;
    std::uint8_t mining;
    std::array<std::optional<Object>, INVENTORY_SIZE> inventory;
    PlayerState player_state;
    const Location* player_location;
    const Resource* resource_target;

    public:
        Player() noexcept: health(10), mining(1), inventory(), player_state(NONE), player_location(nullptr), resource_target(nullptr)
        {}

        void moveTo(const Location& location)
        {
            player_location = &location;
        }

        const bool hasEnoughSkillLevel(Skills skill) const noexcept
        {
            switch(skill)
            {
                case MINING:
                {
                    if(mining >= resource_target->level)
                        return true;
                    break;
                }
            }
            return false;
        }

        const bool locationHasResource(Resources item) const
        {
            for(const Resource& resource : player_location->getResources())
                if(item == resource.name)
                    return true;
            return false;
        }

        void setResourceTarget(Resources item)
        {
            for(const Resource& resource : player_location->getResources())
                if(item == resource.name)
                    {
                        resource_target = &resource;
                        break;
                    }
        }

        PlayerState getAction() const noexcept
        {
            return player_state;
        }

        void startAction(PlayerState action) noexcept
        {
            player_state = action;
        }

        const bool addItem(Object item) //Cannot be Object& because resource needs to return object.name
        //and after move, item will be eaten up by slot and if item is a reference to object, object will not be able
        //to return object.name as it will become empty
        {
            for(auto& slot : inventory)
                if(!slot.has_value())
                {
                    slot.emplace(std::move(item));
                    return true;
                }
            return false;
        }

        Objects extractResource()
        {
            Object object(resource_target->object_name);
            if(addItem(object))
                return object.name;
            return NO_ITEM;
        }
};

#endif