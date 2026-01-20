#ifndef GAME_H
#define GAME_H

#include "menu.h"
#include "player.h"
#include "random.h"

size_t visible_length(const std::string& s)
{
    size_t len = 0;
    size_t virtual_size = s.size();
    for (size_t i = 0; i < virtual_size;)
    {
        if (s[i] == '\x1b' && i + 1 < virtual_size && s[i + 1] == '[')
        {
            i += 2;
            while (i < virtual_size && s[i] != 'm')
                ++i;
            if (i < virtual_size)
                ++i;
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
    //history.size() < SCREEN_HEIGHT - 1
    //history[i].size() <= COMMAND_SCREEN_WIDTH

    uint8_t fps = 60;
    std::array<std::string, SCREEN_HEIGHT> frame_buffer;
    std::deque<std::string> history;
    std::string command = "";
    GameState game_state = GameState::MAIN;
    Player player = Player();
    Menu main_menu = Menu({"New Game", "Load Game", "Quit"}, MAIN_MENU_BOX_WIDTH);
    Menu pause_menu =  Menu({"Continue", "Save Game", "Quit to Main Menu"}, PAUSE_MENU_BOX_WIDTH);
    Menu save_menu = Menu({"Slot 1", "Slot 2", "Slot 3"}, SAVE_MENU_BOX_WIDTH);
    const Resource* player_target = nullptr;
    const std::array<Resource, 4> game_resources =
    {
        Resource(ResourceName::COPPER),
        Resource(ResourceName::TIN),
        Resource(ResourceName::IRON),
        Resource(ResourceName::GOLD)
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

        uint8_t readRawInput()
        {
            int key = _getch();
            if(key == CommandKey::RAW1 || key == CommandKey::RAW2)
                return static_cast<uint8_t>(_getch());
            return static_cast<uint8_t>(key);
        }

        void handleInput()
        {
            if (_kbhit())
            {
                uint8_t key = readRawInput();
                switch(game_state)
                {
                    case GameState::MAIN:
                    {
                        switch(key)
                        {
                            case CommandKey::UP:
                            {
                                main_menu.moveUp();
                                break;
                            }
                            case CommandKey::DOWN:
                            {
                                main_menu.moveDown();
                                break;
                            }
                            case CommandKey::ENTER:
                            {
                                std::string_view main_menu_item_name = main_menu.currentItem();
                                if(main_menu_item_name == "New Game" || main_menu_item_name == "Load Game")
                                {
                                    game_state = GameState::RUNNING;
                                    if (main_menu_item_name == "New Game")
                                        newGame();
                                    else
                                        loadGame();
                                }
                                else
                                    game_state = GameState::QUIT;
                                break;
                            }
                        }
                        break;
                    }
                    case GameState::PAUSE:
                    {
                        switch(key)
                        {
                            case CommandKey::UP:
                            {
                                pause_menu.moveUp();
                                break;
                            }
                            case CommandKey::DOWN:
                            {
                                pause_menu.moveDown();
                                break;
                            }
                            case CommandKey::ENTER:
                            {
                                std::string_view pause_menu_item_name = pause_menu.currentItem();
                                if(pause_menu_item_name == "Continue")
                                    game_state = GameState::RUNNING;
                                else if(pause_menu_item_name == "Save Game")
                                    game_state = GameState::SAVE;
                                else
                                    game_state = GameState::MAIN;
                                break;
                            }
                            case CommandKey::ESC:
                            {
                                game_state = GameState::RUNNING;
                                break;
                            }
                        }
                        break;
                    }
                    case GameState::SAVE:
                    {
                        switch(key)
                        {
                            case CommandKey::UP:
                            {
                                save_menu.moveUp();
                                break;
                            }
                            case CommandKey::DOWN:
                            {
                                save_menu.moveDown();
                                break;
                            }
                            case CommandKey::ENTER:
                            {
                                saveGame();
                                game_state = GameState::RUNNING;
                                break;
                            }
                            case CommandKey::ESC:
                            {
                                game_state = GameState::PAUSE;
                                break;
                            }
                        }
                        break;
                    }
                    case GameState::RUNNING:
                    {
                        switch(key)
                        {
                            case CommandKey::ESC:
                            {
                                game_state = GameState::PAUSE;
                                break;
                            }
                            case CommandKey::ENTER:
                            {
                                std::pair<ActionVerb, ResourceName> parsed_command = parseCommand();
                                if(parsed_command.second != ResourceName::NO_RESOURCE)
                                    handleCommand(parsed_command);
                                    pushHistory(command);
                                clearCommand();
                                break;
                            }
                            default:
                            {
                                if ((key >= 32 && key <= 126) || key == TextualKey::BACKSPACE)
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
            player.reset();
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

        void makeSpaceHistory()
        {
            if(history.size() == HISTORY_LENGTH)
                history.pop_front();
        }

        void pushHistory(std::string text)
        {
            while(text.size() > COMMAND_SCREEN_WIDTH)
            {
                makeSpaceHistory();
                history.push_back(text.substr(0, COMMAND_SCREEN_WIDTH));
                text.erase(0, COMMAND_SCREEN_WIDTH);
            }
            makeSpaceHistory();
            history.push_back(std::move(text));
        }

        bool setPlayerTarget(ResourceName item)
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

        DropResult extractResource() //Extracts resource and adds it to inventory, returns name to updateState for verbose
        {
            if(player_target == nullptr)
                return DropResult(ObjectName::NO_ITEM, Rarity::ALWAYS);
            //handle drop_rate = 0 case
            if(random_int(0, player_target->drop_rate - 1) == 0)
                if(player.addItem(player_target->object))
                    return DropResult(player_target->object.name, drop_rate_to_rarity(player_target->drop_rate));
            return DropResult(ObjectName::NO_ITEM, Rarity::ALWAYS);
        }

        void updateState()
        {
            PlayerState action = player.getAction();
            if(action == PlayerState::MINING_STATE) //If a skill-based action
            {
                Skills skill = playerstate_to_skill(action);
                uint32_t exp;
                if(player_target)  //if player has a target resource, it is an extraction kind of skill task
                {
                    DropResult drop = extractResource();
                    if(drop.obj_name == ObjectName::NO_ITEM)
                    {
                        if(player.isInventoryFull())
                        {
                            pushHistory("Your inventory is full!");
                            player_target = nullptr;
                            player.startAction(PlayerState::NONE);
                        }
                        return;
                    }
                    switch(action)
                    {
                        case PlayerState::MINING_STATE:
                            pushHistory(std::string("You mined a ") + rarity_to_color(drop.rarity) + std::string(objects_map_inverse(drop.obj_name)) + RESET + ".");
                            break;
                    }
                    exp = xp_table(skill, drop.obj_name).second;
                }
                //else {} block to be added her for non-extraction skill based tasks
                /*
                else
                {
                    exp =
                }
                */
                std::string skill_str = std::string(skill_to_verbose(skill));
                pushHistory("You gained " + std::to_string(exp) + " " + skill_str + " experience.");
                if(player.gainExperience(skill, exp))
                    pushHistory("Congratulations! You have gained 1 " + skill_str + " level. Your " + skill_str + " level is now "
                    + std::to_string(player.getLevel(skill)) + ".");
            }
            else
            {
                switch(player.getAction())
                {
                    case PlayerState::NONE:
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

        std::pair<ActionVerb, ResourceName> parseCommand() const noexcept
        {
            size_t pos = command.find(' ');
            if(pos == std::string::npos)
                return {ActionVerb::NO_ACTION, ResourceName::NO_RESOURCE};
            std::string_view verb(command.data(), pos);
            std::string_view obj(command.data() + pos + 1);
            auto it = action_map.find(verb);
            if(it == action_map.end())
                return {ActionVerb::NO_ACTION, ResourceName::NO_RESOURCE};
            switch(it->second)
            {
                case ActionVerb::MINE:
                {
                    auto it2 = ores_map.find(obj);
                    if(it2 != ores_map.end())
                        return {it->second, it2->second};
                    return {ActionVerb::NO_ACTION, ResourceName::NO_RESOURCE};
                    break;
                }
            }
            return {ActionVerb::NO_ACTION, ResourceName::NO_RESOURCE};
        }

        void handleCommand(std::pair<ActionVerb, ResourceName> parsed_command) noexcept
        {
            if(setPlayerTarget(parsed_command.second))
            {
                Skills skill = action_to_skill(parsed_command.first);
                if(!player.hasEnoughSkillLevel(skill, resource_min_level(player_target->name)))
                {
                    pushHistory("You do not have enough " + std::string(skill_to_verbose(skill)) + " level!");
                    return;
                }
                player.startAction(skill_to_playerstate(skill));
            }
        }

        void echo()
        {
            size_t hsize = history.size();
            if(hsize < HISTORY_LENGTH)
            {
                for(size_t i = 0; i < hsize; i++)
                    frame_buffer[i] = history[i] + std::string(COMMAND_SCREEN_WIDTH - visible_length(history[i]), ' ') + "|";
                for(size_t i = hsize; i < HISTORY_LENGTH; i++)
                    frame_buffer[i] = std::string(COMMAND_SCREEN_WIDTH, ' ') + "|";
            }
            else
                for(size_t i = 0; i < HISTORY_LENGTH; i++)
                    frame_buffer[i] = history[i] + std::string(COMMAND_SCREEN_WIDTH - visible_length(history[i]), ' ') + "|";

            frame_buffer[SCREEN_HEIGHT - 2].replace(0, COMMAND_SCREEN_WIDTH + 1, std::string(COMMAND_SCREEN_WIDTH, '_') + "|");
            frame_buffer[SCREEN_HEIGHT - 1].replace(0, COMMAND_SCREEN_WIDTH + 1, command + std::string(COMMAND_SCREEN_WIDTH - command.size(), 
            ' ') + "|");

            if(player.getAction() == PlayerState::MINING_STATE)
            {
                Skills skill = playerstate_to_skill(player.getAction());
                std::string_view skill_v = skill_to_verbose(skill);
                frame_buffer[SCREEN_HEIGHT - 2].replace(COMMAND_SCREEN_WIDTH + 1, skill_v.size(), skill_v);
                size_t prog = player.expProgress(skill);
                frame_buffer[SCREEN_HEIGHT - 1].replace(COMMAND_SCREEN_WIDTH + 1, PROGRESSBAR_PARTITIONS + 1, std::string(prog, '.') +
                std::string(PROGRESSBAR_PARTITIONS - prog, ' ') + "|");
            }
        }

        void renderFrame()
        {
            hideCursor();
            clearFrame();
            switch(game_state)
            {
                case GameState::MAIN:
                {
                    main_menu.renderMenu(frame_buffer);
                    break;
                }
                case GameState::PAUSE:
                {
                    pause_menu.renderMenu(frame_buffer);
                    break;
                }
                case GameState::SAVE:
                {
                    save_menu.renderMenu(frame_buffer);
                    break;
                }
                case GameState::RUNNING:
                {
                    echo();
                    break;
                }
            }
            std::cout<<"\x1b[H";
            for(std::string_view row: frame_buffer)
                std::cout<<row<<'\n';
            std::cout<<"\x1b["<<static_cast<int>(SCREEN_HEIGHT)<<";"<<command.size() + 1<<"H";
            if(game_state == GameState::RUNNING)
                showCursor();
        }

        void runGame()
        {
            auto prev = high_clock::now();
            auto accumulator = std::chrono::milliseconds(0);
            while(game_state != GameState::QUIT)
            {
                auto frameStart = high_clock::now();
                accumulator += std::chrono::duration_cast<std::chrono::milliseconds>(frameStart - prev);
                prev = frameStart;

                handleInput();
                while(accumulator >= tick)
                {
                    if(game_state == GameState::RUNNING) 
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