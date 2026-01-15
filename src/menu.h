#ifndef MENU_H
#define MENU_H

#include "constants.h"
#include <vector>

class Menu
{
    //invariants
    //index < items.size() --> this condition has to be maintained internally after constructor call
    //0 < items.size() <= SCREEN_HEIGHT
    //0 < menu_box_width <= SCREEN_WIDTH
    //menu_box_height = items.size() + 2
    //items[i].size() <= menu_box_width - 4 for all 0<=i<items.size()
    //implies menu_box_width > 4
    const std::vector<std::string> items;
    size_t index;
    const size_t menu_box_width, menu_box_height;

    public:
        explicit Menu(std::vector<std::string> items, size_t menu_box_width) : 
        items(std::move(items)),
        index(0),
        menu_box_width(menu_box_width),
        menu_box_height(this->items.size() + 2)
        {
            if(this->items.empty())
                throw std::invalid_argument("Menu cannot be empty");
            if(this->items.size() > SCREEN_HEIGHT)
                throw std::invalid_argument("Menu box height cannot be more than screen height");
            if(this->menu_box_width == 0)
                throw std::invalid_argument("Menu box width cannot be 0");
            if(this->menu_box_width > SCREEN_WIDTH)
                throw std::invalid_argument("Menu box width cannot be more than screen width");
            if(this->menu_box_width < 5)
                throw std::invalid_argument("Menu box width too small to render any menu item");
            for(size_t i=0; i<this->items.size(); i++)
                if(this->items[i].size() > menu_box_width - 4)
                    throw std::invalid_argument("This menu item is bigger than menu box width");
        }

        size_t getSize() const noexcept
        {
            return items.size();
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
            if(index < items.size() - 1)
                index++;
        }

        void renderMenu(std::vector<std::string>& frame_buffer) const
        {
            const size_t x_pad = (SCREEN_WIDTH  - menu_box_width)  / 2;
            const size_t y_pad = (SCREEN_HEIGHT - menu_box_height) / 2;

            for (size_t row = 0; row < menu_box_height; row++)
            {
                size_t i = y_pad + row;
                if (row == 0 || row == menu_box_height - 1)
                {
                    frame_buffer[i] =
                    std::string(x_pad, ' ') +
                    std::string(menu_box_width, '-') +
                    std::string(SCREEN_WIDTH - x_pad - menu_box_width, ' ');
                }
                else
                {
                    const size_t item_idx = row - 1;
                    std::string q = (index == item_idx ? "->" : "") + items[item_idx];
                    size_t content_width = menu_box_width - 2;
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