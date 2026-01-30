#ifndef PLAYER_H
#define PLAYER_H

#include "resources.h"

class Player 
{
    std::unordered_map<Skills, std::pair<int, int>> player_skills =
    {
        {HEALTH, {10, 1154}},
        {MINING, {1, 0}}
    };
    std::array<std::optional<Object>, INVENTORY_SIZE> inventory;
    std::array<std::optional<Tool>, NUM_TOOLS> toolbelt;
    PlayerState player_state = NONE;
    int inventory_occupancy = 0;

    public:
        Player() noexcept
        {}

        bool hasEnoughSkillLevel(Skills skill, int level) const noexcept
        {
            if(player_skills.at(skill).first >= level)
                return true;
            return false;
        }

        bool gainExperience(Skills skill, int exp) noexcept
        {
            player_skills[skill].second += exp;
            if(player_skills[skill].second < level_exp_mapping(player_skills[skill].first + 1))
                return false;
            levelUp(skill);
            return true;
        }

        size_t expProgress(Skills skill) const
        {
            //ceil(a/b) = (a + b - 1)/b for unsigned integers
            int player_exp = player_skills.at(skill).second;
            if(player_skills.at(skill).first > 1)
                player_exp -= level_exp_mapping(player_skills.at(skill).first);
            int required_exp = level_exp_mapping(player_skills.at(skill).first + 1) - level_exp_mapping(player_skills.at(skill).first);
            if (player_exp == 0 || required_exp == 0)
                return 0;
            return (player_exp * PROGRESSBAR_PARTITIONS + required_exp - 1)/required_exp;
        }

        void levelUp(Skills skill) noexcept
        {
            player_skills[skill].first += 1;
        }

        int getLevel(Skills skill) const noexcept
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

        bool isInventoryFull() const noexcept
        {
            return inventory_occupancy == INVENTORY_SIZE;
        }

        const std::array<std::optional<Object>, INVENTORY_SIZE>& getInventory() const noexcept
        {
            return inventory;
        }

        void reset() noexcept
        {
            player_skills = 
            {
                {HEALTH, {10, 0}},
                {MINING, {1, 0}}
            };

            for (auto& slot : inventory)
                slot.reset();

            for (auto& tool : toolbelt)
                tool.reset();

            inventory_occupancy = 0;
            player_state = NONE;
        }
};

#endif