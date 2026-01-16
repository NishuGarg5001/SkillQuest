#ifndef MENU_H
#define MENU_H

#include "constants.h"

class Menu
{
    //invariants
    //menu_size > 0
    //index < menu_size --> this condition has to be maintained internally after constructor call
    //menu_box_height = menu_size + 2
    //2 < menu_box_height <= SCREEN_HEIGHT to accomodate at least 1 menu item
    //4 < menu_box_width <= SCREEN_WIDTH to accomodate at least 1 char
    //items[i].size() <= menu_box_width - 4 for all 0<=i<menu_size because menu item width shouldn't exceed menu box width
    uint8_t index;
    const std::vector<std::string> items;
    const uint8_t menu_size, menu_box_width, menu_box_height;

    public:
        explicit Menu(std::vector<std::string> items, uint8_t menu_box_width) : 
        index(0),
        items(std::move(items)),
        menu_size(static_cast<uint8_t>(this->items.size())),
        menu_box_width(menu_box_width),
        menu_box_height(menu_size + 2)
        {
            if(this->items.empty())
                throw std::invalid_argument("Menu cannot be empty");
            if(menu_box_height > SCREEN_HEIGHT)
                throw std::invalid_argument("Menu box height cannot be more than screen height");
            if(menu_box_height < 3)
                throw std::invalid_argument("Menu box height too small to accomodate any menu item");
            if(this->menu_box_width > SCREEN_WIDTH)
                throw std::invalid_argument("Menu box width cannot be more than screen width");
            if(this->menu_box_width < 5)
                throw std::invalid_argument("Menu box width too small to render any menu item's text");
            for(uint8_t i = 0; i < menu_size; i++)
                if(this->items[i].size() > this->menu_box_width - 4)
                    throw std::invalid_argument("This menu item is bigger than menu box width");
        }

        std::string_view currentItem() const noexcept
        {
            return items[index];
        }

        void moveUp() noexcept
        {
            if(index > 0)
                index--;
        }

        void moveDown() noexcept
        {
            if(index < menu_size - 1)
                index++;
        }

        void renderMenu(std::array<std::string, SCREEN_HEIGHT>& frame_buffer) const
        {
            const uint8_t x_pad = (SCREEN_WIDTH  - menu_box_width)  / 2;
            const uint8_t y_pad = (SCREEN_HEIGHT - menu_box_height) / 2;

            for (uint8_t row = 0; row < menu_box_height; row++)
            {
                uint8_t i = y_pad + row;
                if (row == 0 || row == menu_box_height - 1)
                {
                    frame_buffer[i] =
                    std::string(x_pad, ' ') +
                    std::string(menu_box_width, '-') +
                    std::string(SCREEN_WIDTH - x_pad - menu_box_width, ' ');
                }
                else
                {
                    const uint8_t item_idx = row - 1;
                    std::string q = (index == item_idx ? "->" : "") + items[item_idx];
                    uint8_t content_width = menu_box_width - 2;
                    frame_buffer[i] =
                    std::string(x_pad, ' ') +
                    "|" + q +
                    std::string(content_width - q.size(), ' ') +
                    "|" +
                    std::string(SCREEN_WIDTH - x_pad - menu_box_width, ' ');
                }
            }
        }
};

#endif