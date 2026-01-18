#ifndef PLAYER_H
#define PLAYER_H

#include "resources.h"

class Player 
{
    std::unordered_map<Skills, std::pair<uint8_t, uint32_t>> player_skills = 
    {
        {HEALTH, {10, 0}},
        {MINING, {1, 0}}
    };
    std::array<std::optional<Object>, INVENTORY_SIZE> inventory;
    PlayerState player_state = NONE;
    uint8_t inventory_occupancy = 0;

    public:
        Player() noexcept {}

        const bool hasEnoughSkillLevel(Skills skill, uint8_t level) const noexcept
        {
            if(player_skills.at(skill).first >= level)
                return true;
            return false;
        }

        void gainExperience(Skills skill, uint32_t exp) noexcept
        {
            player_skills[skill].second += exp;
        }

        const bool levelUp(Skills skill) noexcept
        {
            if(player_skills.at(skill).second < level_exp_mapping.at(player_skills.at(skill).first + 1))
                return false;
            player_skills[skill].first += 1;
            return true;
        }

        uint8_t getLevel(Skills skill) const noexcept
        {
            return player_skills.at(skill).first;
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
                    inventory_occupancy++;
                    return true;
                }
            return false;
        }

        const bool isInventoryFull()
        {
            return inventory_occupancy == INVENTORY_SIZE;
        }
};

#endif