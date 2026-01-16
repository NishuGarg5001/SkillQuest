#ifndef GAME_H
#define GAME_H

#include "menu.h"
#include "player.h"

class Game
{
    //invariants
    //0 < fps < 61
    //history.size() <= SCREEN_HEIGHT - 1

    uint8_t fps;
    std::array<std::string, SCREEN_HEIGHT> frame_buffer;
    std::deque<std::string> history;
    std::string command;
    GameState game_state;
    std::vector<Location> locations;
    Player player;
    Menu main_menu, pause_menu, save_menu;

    public:
        explicit Game():
        fps(60),
        history(),
        command(""),
        game_state(MAIN),
        locations(
            {
                Location("Dirtmound", {Resource(COPPER, MINING, COPPER_ORE, 1, 4), Resource(TIN, MINING, TIN_ORE, 1, 4)}),
                Location("Goldtown", {Resource(IRON, MINING, IRON_ORE, 10, 15), Resource(GOLD, MINING, GOLD_ORE, 20, 40)})
            }
        ),
        main_menu(Menu({"New Game", "Load Game", "Quit"}, 13)),
        pause_menu(Menu({"Continue", "Save Game", "Quit to Main Menu"}, 21)),
        save_menu(Menu({"Slot 1", "Slot 2", "Slot 3"}, 10)),
        player(Player()){}

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
            player.moveTo(locations[0]);
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
            if(history.size() == SCREEN_HEIGHT - 1)
                history.pop_front();
            history.push_back(std::move(text));
        }

        void updateState()
        {
            switch(player.getAction())
            {
                case MINING_STATE:
                {
                    Objects object_name = player.extractResource();
                    if(object_name == NO_ITEM)
                    {
                        pushHistory("Your inventory is full!");
                        player.startAction(NONE);
                        return;
                    }
                    pushHistory(std::string("You mined a ").append(objects_map_inverse.find(object_name)->second).append("."));
                    break;
                }
                case NONE:
                    break;
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
                if(isprint(x) && command.size() < SCREEN_WIDTH - 1)
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
            if(!player.locationHasResource(parsed_command.second))
            {
                command = "Location does not have the resource!";
                return;
            }
            player.setResourceTarget(parsed_command.second);
            switch(parsed_command.first)
            {
                case MINE:
                {
                    if(!player.hasEnoughSkillLevel(MINING))
                    {
                        command = "You do not have enough mining level!";
                        return;
                    }
                    player.startAction(MINING_STATE);
                    break;
                }
            }
        }

        void echo()
        {
            size_t hsize = history.size();
                for(size_t i = 0; i < hsize; i++)
                    frame_buffer[i] = history[i] + std::string(SCREEN_WIDTH - history[i].size(), ' ');
            frame_buffer[SCREEN_HEIGHT - 1] = command + std::string(SCREEN_WIDTH - command.size(), ' ');
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