#ifndef PLAYER_H
#define PLAYER_H

#include "resources.h"

class Player 
{
    std::unordered_map<std::string, std::pair<int, int>> player_skills;
    std::array<std::optional<Object>, INVENTORY_SIZE> inventory;
    std::array<std::optional<Tool>, NUM_TOOLS> toolbelt;
    std::array<std::optional<std::pair<Object, size_t>>, VAULT_SIZE> vault;
    std::string player_state;
    size_t inventory_occupancy;
    size_t vault_occupancy;

    public:
        void reset() noexcept
        {
            for(size_t i = 0; i<skills_list.size(); i++)
                player_skills[skills_list[i]] = {starting_levels[i], level_exp_mapping(starting_levels[i])};

            for (auto& slot : inventory)
                slot.reset();

            for (auto& slot : vault)
                slot.reset();

            for (auto& tool : toolbelt)
                tool.reset();

            inventory_occupancy = 0;
            vault_occupancy = 0;
            stop();
        }

        Player() noexcept
        {
            reset();
        }

        bool hasEnoughSkillLevel(const std::string& skill, int level) const noexcept
        {
            if(player_skills.at(skill).first >= level)
                return true;
            return false;
        }

        bool gainExperience(const std::string& skill, int exp) noexcept
        {
            player_skills[skill].second += exp;
            if(player_skills[skill].second < level_exp_mapping(player_skills[skill].first + 1))
                return false;
            levelUp(skill);
            return true;
        }

        size_t expProgress(const std::string& skill) const
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

        void levelUp(const std::string& skill) noexcept
        {
            player_skills[skill].first += 1;
        }

        int getLevel(const std::string& skill) const noexcept
        {
            return player_skills.at(skill).first;
        }

        const std::string& getAction() const noexcept
        {
            return player_state;
        }

        void startAction(std::string_view action) noexcept
        {
            player_state = action;
        }

        void stop() noexcept
        {
            player_state = invalid_action;
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

        bool hasInInventory(std::string_view item_name) const noexcept
        {
            for(const auto& slot : inventory)
                if(slot.has_value() && slot->name == item_name)
                    return true;
            return false;
        }

        bool hasInVault(std::string_view item_name) const noexcept
        {
            for(const auto& slot : vault)
                if(slot.has_value() && slot->first.name == item_name)
                    return true;
            return false;
        }

        bool isInventoryFull() const noexcept
        {
            return inventory_occupancy == INVENTORY_SIZE;
        }

        bool isVaultFull() const noexcept
        {
            return vault_occupancy == VAULT_SIZE;
        }

        void depositItem(std::string item_name, size_t quantity)
        {
            if(quantity == 0)
                return;
            size_t moved = 0;
            std::optional<size_t> vault_index;
            bool new_item = true;

            for (size_t j=0; j<VAULT_SIZE; j++)
            {
                if(!vault[j] && !vault_index)
                    vault_index = j;
                if (vault[j] && vault[j]->first.name == item_name)
                {
                    vault_index = j;
                    new_item = false;
                    break;
                }
            }

            for(size_t i=0; i<INVENTORY_SIZE && moved < quantity; i++)
            {
                if(inventory[i] && inventory[i]->name == item_name)
                {
                    if(moved == 0 && new_item)
                    {
                        vault[*vault_index].emplace(std::move(*inventory[i]), 0);
                        vault_occupancy++;
                    }
                    vault[*vault_index]->second++;
                    inventory[i].reset();
                    inventory_occupancy--;
                    moved++;
                }
            }
        }

        const std::array<std::optional<Object>, INVENTORY_SIZE>& getInventory() const noexcept
        {
            return inventory;
        }

        const std::array<std::optional<std::pair<Object, size_t>>, VAULT_SIZE>& getVault() const noexcept
        {
            return vault;
        }
};

#endif