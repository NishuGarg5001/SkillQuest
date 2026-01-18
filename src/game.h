#ifndef GAME_H
#define GAME_H

#include "menu.h"
#include "player.h"
#include "random.h"

uint8_t visible_length(const std::string& s)
{
    uint8_t len = 0;
    uint8_t virtual_size = static_cast<uint8_t>(s.size());
    for (uint8_t i = 0; i < virtual_size;)
    {
        if (s[i] == '\x1b' && i + 1 < virtual_size && s[i + 1] == '[')
        {
            i += 2;
            while (i < virtual_size && s[i] != 'm')
                ++i;
            if (i < virtual_size)
                ++i; // skip 'm'
        }
        else
        {
            ++len;
            ++i;
        }
    }
    return len;
}

class Game
{
    //invariants
    //0 < fps < 61
    //history.size() <= SCREEN_HEIGHT - 1

    uint8_t fps = 60;
    std::array<std::string, SCREEN_HEIGHT> frame_buffer;
    std::deque<std::string> history;
    std::string command = "";
    GameState game_state = MAIN;
    Player player = Player();
    Menu main_menu = Menu({"New Game", "Load Game", "Quit"}, 13);
    Menu pause_menu =  Menu({"Continue", "Save Game", "Quit to Main Menu"}, 21);
    Menu save_menu = Menu({"Slot 1", "Slot 2", "Slot 3"}, 10);
    const Resource* player_target = nullptr;
    const std::array<Resource, 4> game_resources =
    {
        Resource(COPPER, COPPER_ORE, 10),
        Resource(TIN, TIN_ORE, 10),
        Resource(IRON, IRON_ORE, 40),
        Resource(GOLD, GOLD_ORE, 40),
    };

    public:
        Game() noexcept {}

        void setFPS(uint8_t fps) noexcept
        {
            if(fps == 0)
                this->fps = 1;
            else if(fps > 60)
                this->fps = 60;
            else
                this->fps = fps;
        }

        void clearFrame() noexcept
        {
            for (std::string& row: frame_buffer)
                row.assign(SCREEN_WIDTH, ' ');
        }

        void handleInput()
        {
            if (_kbhit())
            {
                int key = _getch();
                if(key == KEY_EXTENDED1 || key == KEY_EXTENDED2)
                    key = _getch();
                switch(game_state)
                {
                    case MAIN:
                    {
                        switch(key)
                        {
                            case UP:
                            {
                                main_menu.moveUp();
                                break;
                            }
                            case DOWN:
                            {
                                main_menu.moveDown();
                                break;
                            }
                            case ENTER:
                            {
                                std::string_view main_menu_item_name = main_menu.currentItem();
                                if(main_menu_item_name == "New Game" || main_menu_item_name == "Load Game")
                                {
                                    game_state = RUNNING;
                                    if (main_menu_item_name == "New Game")
                                        newGame();
                                    else
                                        loadGame();
                                }
                                else
                                    game_state = QUIT;
                                break;
                            }
                        }
                        break;
                    }
                    case PAUSE:
                    {
                        switch(key)
                        {
                            case UP:
                            {
                                pause_menu.moveUp();
                                break;
                            }
                            case DOWN:
                            {
                                pause_menu.moveDown();
                                break;
                            }
                            case ENTER:
                            {
                                std::string_view pause_menu_item_name = pause_menu.currentItem();
                                if(pause_menu_item_name == "Continue")
                                    game_state = RUNNING;
                                else if(pause_menu_item_name == "Save Game")
                                    game_state = SAVE;
                                else
                                    game_state = MAIN;
                                break;
                            }
                            case ESC:
                            {
                                game_state = RUNNING;
                                break;
                            }
                        }
                        break;
                    }
                    case SAVE:
                    {
                        switch(key)
                        {
                            case UP:
                            {
                                save_menu.moveUp();
                                break;
                            }
                            case DOWN:
                            {
                                save_menu.moveDown();
                                break;
                            }
                            case ENTER:
                            {
                                saveGame();
                                game_state = RUNNING;
                                break;
                            }
                            case ESC:
                            {
                                game_state = PAUSE;
                                break;
                            }
                        }
                        break;
                    }
                    case RUNNING:
                    {
                        switch(key)
                        {
                            case ESC:
                            {
                                game_state = PAUSE;
                                break;
                            }
                            case ENTER:
                            {
                                auto parsed_command = parseCommand();
                                if(parsed_command.second != NO_RESOURCE)
                                    handleCommand(parsed_command);
                                    pushHistory(command);
                                clearCommand();
                                break;
                            }
                            default:
                            {
                                if ((key >= 32 && key <= 126) || key == 8)
                                    updateCommand(static_cast<char>(key));
                                break;
                            }
                        }
                    }
                }
            }
        }

        void newGame() noexcept
        {
            player = Player();
            history.clear();
        }

        void saveGame() const
        {
            //WIP
        }

        void loadGame()
        {
            history.clear();
        }

        void pushHistory(std::string text)
        {
            if(history.size() == HISTORY_LENGTH)
                history.pop_front();
            history.push_back(std::move(text));
        }

        bool setPlayerTarget(Resources item)
        {
            for (const Resource& resource : game_resources)
            {
                if(resource.name == item)
                {
                    player_target = &resource;
                    return true;
                }
            }
            player_target = nullptr;
            return false;
        }

        Objects extractResource()
        {
            if(player_target == nullptr)
                return NO_ITEM;
            if(random_int(0, player_target->gen_rate - 1) == 0)
                if(player.addItem(player_target->object))
                    return player_target->object.name;
            return NO_ITEM;
        }

        void updateState()
        {
            PlayerState action = player.getAction();
            if(action == MINING_STATE) //If a skill-based action
            {
                Skills skill = playerstate_to_skill.at(action);
                uint32_t exp;
                if(player_target)  //if player has a target resource, it is an extraction kind of skill task
                {
                    Objects object_name = extractResource();
                    if(object_name == NO_ITEM)
                    {
                        if(player.isInventoryFull())
                        {
                            pushHistory("Your inventory is full!");
                            player_target = nullptr;
                            player.startAction(NONE);
                        }
                        return;
                    }
                    switch(action)
                    {
                        case MINING_STATE:
                            pushHistory(std::string("You mined a ") + BROWN + objects_map_inverse.at(object_name) + WHITE + ".");
                            break;
                    }
                    exp = xp_table.at(skill).at(object_name).second;
                }
                //else {} block to be added her for non-extraction skill based tasks
                /*
                else
                {
                    exp =
                }
                */
                player.gainExperience(skill, exp);
                std::string skill_str = skill_to_verbose.at(skill);
                pushHistory("You gained " + std::to_string(exp) + " " + skill_str + " experience.");
                if(player.levelUp(skill))
                    pushHistory("Congratulations! You have gained 1 " + skill_str + " level. Your " + skill_str + " level is now "
                    + std::to_string(player.getLevel(skill)) + ".");
            }
            else
            {
                switch(player.getAction())
                {
                    case NONE:
                        break;
                }
            }
        }

        void hideCursor() const noexcept
        {
            std::cout<<"\x1b[?25l";
        }

        void showCursor() const noexcept
        {
            std::cout<<"\x1b[?25h";
        }

        void clearCommand() noexcept
        {
            command = "";
        }

        void updateCommand(char x)
        {
            if(x != '\b')
            {
                if(isprint(x) && command.size() < COMMAND_SCREEN_WIDTH)
                    command.push_back(x);
            }
            else if(command.size() > 0)
                command.pop_back();
        }

        std::pair<ActionVerb, Resources> parseCommand() const noexcept
        {
            uint8_t pos = command.find(' ');
            if(pos == std::string::npos)
                return {NO_ACTION, NO_RESOURCE};
            std::string_view verb(command.data(), pos);
            std::string_view obj(command.data() + pos + 1);
            auto it = action_map.find(verb);
            if(it == action_map.end())
                return {NO_ACTION, NO_RESOURCE};
            switch(it->second)
            {
                case MINE:
                {
                    auto it2 = ores_map.find(obj);
                    if(it2 != ores_map.end())
                        return {it->second, it2->second};
                    return {it->second, NO_RESOURCE};
                    break;
                }
            }
            return {NO_ACTION, NO_RESOURCE};
        }

        void handleCommand(const std::pair<ActionVerb, Resources>& parsed_command) noexcept
        {
            if(setPlayerTarget(parsed_command.second))
            {
                Skills skill = action_to_skill.at(parsed_command.first);
                if(!player.hasEnoughSkillLevel(skill, resource_min_level.at(player_target->name)))
                {
                    pushHistory("You do not have enough " + skill_to_verbose.at(skill) + " level!");
                    return;
                }
                player.startAction(skill_to_playerstate.at(skill));
            }
        }

        void echo()
        {
            uint8_t hsize = static_cast<uint8_t>(history.size());
            if(hsize < HISTORY_LENGTH)
            {
                for(uint8_t i = 0; i < hsize; i++)
                    frame_buffer[i] = history[i] + std::string(COMMAND_SCREEN_WIDTH - visible_length(history[i]), ' ') + "|";
                for(uint8_t i = hsize; i < HISTORY_LENGTH; i++)
                    frame_buffer[i] = std::string(COMMAND_SCREEN_WIDTH, ' ') + "|";
            }
            else
                for(uint8_t i = 0; i < HISTORY_LENGTH; i++)
                    frame_buffer[i] = history[i] + std::string(COMMAND_SCREEN_WIDTH - visible_length(history[i]), ' ') + "|";

            frame_buffer[SCREEN_HEIGHT - 2] = std::string(COMMAND_SCREEN_WIDTH, '_') + "|";
            frame_buffer[SCREEN_HEIGHT - 1] = command + std::string(COMMAND_SCREEN_WIDTH - command.size(), ' ') + "|";
        }

        void renderFrame()
        {
            hideCursor();
            clearFrame();
            switch(game_state)
            {
                case MAIN:
                {
                    main_menu.renderMenu(frame_buffer);
                    break;
                }
                case PAUSE:
                {
                    pause_menu.renderMenu(frame_buffer);
                    break;
                }
                case SAVE:
                {
                    save_menu.renderMenu(frame_buffer);
                    break;
                }
                case RUNNING:
                {
                    echo();
                    break;
                }
            }
            std::cout<<"\x1b[H";
            for(std::string_view row: frame_buffer)
                std::cout<<row<<'\n';
            std::cout<<"\x1b["<<static_cast<int>(SCREEN_HEIGHT)<<";"<<command.size() + 1<<"H";
            if(game_state == RUNNING)
                showCursor();
        }

        void runGame()
        {
            auto prev = high_clock::now();
            auto accumulator = std::chrono::milliseconds(0);
            while(game_state != QUIT)
            {
                auto frameStart = high_clock::now();
                accumulator += std::chrono::duration_cast<std::chrono::milliseconds>(frameStart - prev);
                prev = frameStart;

                handleInput();
                while(accumulator >= tick)
                {
                    if(game_state == RUNNING) 
                        updateState();
                    accumulator -= tick;
                }
                renderFrame();

                auto frameDuration = std::chrono::milliseconds(1000 / fps);
                auto frameEnd = high_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);
                if(elapsed < frameDuration) std::this_thread::sleep_for(frameDuration - elapsed);
            }
        }
};

#endif