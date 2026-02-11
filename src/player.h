#ifndef PLAYER_H
#define PLAYER_H

#include "resources.h"

class Player 
{
    std::array<std::optional<Object>, INVENTORY_SIZE> inventory;
    PlayerState player_state;
    size_t inventory_occupancy;
    size_t vault_occupancy;

    public:
        void reset() noexcept
        {

            for (auto& slot : inventory)
                slot.reset();

            inventory_occupancy = 0;
            stopAction();
        }

        Player() noexcept
        {
            reset();
        }

        PlayerState getAction() const noexcept
        {
            return player_state;
        }

        void startAction(PlayerState action) noexcept
        {
            player_state = action;
        }

        void stopAction() noexcept
        {
            player_state = IDLE;
        }

        bool addItem(Object item) //Cannot be Object& because resource needs to return object.name
        //and after move, item will be eaten up by slot and if item is a reference to object, object will not be able
        //to return object.name as it will become empty
        {
            for(auto& slot : inventory)
                if(!slot.has_value())
                {
                    slot.emplace(std::move(item));
                    inventory_occupancy++;
                    return true;
                }
            return false;
        }

        bool hasInInventory(ObjectName item_name) const noexcept
        {
            for(const auto& slot : inventory)
                if(slot.has_value() && slot->name == item_name)
                    return true;
            return false;
        }

        bool isInventoryFull() const noexcept
        {
            return inventory_occupancy == INVENTORY_SIZE;
        }

        const std::array<std::optional<Object>, INVENTORY_SIZE>& getInventory() const noexcept
        {
            return inventory;
        }
};

#endif