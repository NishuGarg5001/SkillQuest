#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <chrono>
#include <thread>

constexpr int KEY_EXTENDED1 = 0;
constexpr int KEY_EXTENDED2 = 224;
constexpr int UP = 72;
constexpr int DOWN = 80;
constexpr int ENTER = 13;
constexpr int ESC = 27;
constexpr int SCREEN_WIDTH  = 60;
constexpr int SCREEN_HEIGHT = 20;

enum game_state
{
    QUIT,
    RUNNING,
    MAIN,
    PAUSE,
    SAVE
};

class Player 
{
    int health;

    public:
        Player() : health(10){}
};

class Menu
{
    std::vector<std::string> items;
    int index;

    public:
        Menu(std::vector<std::string> items) : items(std::move(items)), index(0){}

        void setIndex(int index)
        {
            this->index = index;
        }

        int getIndex() const
        {
            return index;
        }

        int getSize() const
        {
            return static_cast<int>(items.size());
        }

        const std::string& currentItem() const
        {
            return items[index];
        }

        void moveUp()
        {
            if(index > 0) index--;
        }

        void moveDown()
        {
            if(index < getSize() - 1) index++;
        }

        void renderMenu(std::vector<std::string>& frame_buffer) const
        {
            
            int count = 0;
            for(std::string& row: frame_buffer)
            {   
                if(count == static_cast<int>(items.size())) break;
                std::string x = (count==index?"":"    ")+items[count];
                x = x + std::string(SCREEN_WIDTH - x.size(), ' ');
                row = x;
                count++;
            }

        }
};

class Game
{
    int fps;
    std::vector<std::string> frame_buffer;
    game_state state;
    Player player;
    Menu main_menu, pause_menu, save_menu;

    public:
        Game(int fps,
            std::vector<std::string> frame_buffer,
            game_state state,
            Menu main_menu,
            Menu pause_menu,
            Menu save_menu,
            Player player):
        fps(fps),
        frame_buffer(std::move(frame_buffer)),
        state(state),
        main_menu(std::move(main_menu)),
        pause_menu(std::move(pause_menu)),
        save_menu(std::move(save_menu)),
        player(std::move(player)){}

        void setFPS(int fps)
        {
            if(fps < 61) this->fps = fps;
            else this->fps = 60;
        }

        void clearFrame()
        {
            for (std::string& row: frame_buffer)
                row.assign(SCREEN_WIDTH, ' ');
        }

        void handleInput()
        {
            if (_kbhit())
            {
                int key = _getch();
                if(key == KEY_EXTENDED1 || key == KEY_EXTENDED2) key = _getch();
                switch(state)
                {
                    case MAIN:
                    {
                        if(key == UP) main_menu.moveUp();
                        else if(key == DOWN) main_menu.moveDown();
                        else if(key == ENTER)
                        {
                            std::string main_menu_item_name = main_menu.currentItem();
                            if(main_menu_item_name == "New Game" || main_menu_item_name == "Load Game")
                            {
                                state = RUNNING;
                                if (main_menu_item_name == "New Game") startNewGame();
                                else loadGame();
                            }
                            else state = QUIT;
                        }
                        break;
                    }
                
                    case PAUSE:
                    {
                        if(key == UP) pause_menu.moveUp();
                        else if(key == DOWN) pause_menu.moveDown();
                        else if(key == ENTER)
                        {
                            std::string pause_menu_item_name = pause_menu.currentItem();
                            if(pause_menu_item_name == "Continue") state = RUNNING;
                            else if(pause_menu_item_name == "Save Game") state = SAVE;
                            else state = MAIN;
                        }
                        else if(key == ESC) state = RUNNING;
                        break;
                    }

                    case SAVE:
                    {
                        if(key == UP) save_menu.moveUp();
                        else if(key == DOWN) save_menu.moveDown();
                        else if(key == ENTER)
                        {
                            saveGame(save_menu.getIndex());
                            state = RUNNING;
                        }
                        else if(key == ESC) state = PAUSE;
                        break;
                    }

                    case RUNNING:
                    {
                        if(key == ESC) state = PAUSE;
                        break;
                    }
                }
            }
        }

        void startNewGame()
        {
            //WIP
        }

        void saveGame(int index) const
        {
            //WIP
        }

        void loadGame()
        {
            //WIP
        }

        void updateState()
        {
            //WIP
        }

        void showCursor() const
        {
            std::cout << "\x1b[?25h";
        }

        void hideCursor() const
        {
            std::cout << "\x1b[?25l";
        }

        void renderFrame()
        {
            clearFrame();
            switch(state)
            {
                case MAIN:
                    main_menu.renderMenu(frame_buffer);
                    break;
                case PAUSE:
                    pause_menu.renderMenu(frame_buffer);
                    break;
                case SAVE:
                    save_menu.renderMenu(frame_buffer);
                    break;
                case RUNNING:
                    //WIP
                    break;
                case QUIT:
                    break;
            }
            std::cout << "\x1b[H";
            for(const std::string& row: frame_buffer)
                std::cout<<row<<"\n";
        }

        void runGame()
        {
            while(true)
            {
                auto frameStart = std::chrono::high_resolution_clock::now();

                handleInput();
                hideCursor();
                if(state == RUNNING) updateState();
                renderFrame();
                if(state == QUIT) break;

                std::chrono::milliseconds frameDuration(1000 / fps);
                auto frameEnd = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);
                if(elapsed < frameDuration)
                {
                    std::this_thread::sleep_for(frameDuration - elapsed);
                }
            }
        }
};

int main()
{
    Game game(
        60,
        std::vector<std::string>(SCREEN_HEIGHT, std::string(SCREEN_WIDTH, ' ')),
        MAIN,
        Menu({"New Game", "Load Game", "Quit"}),
        Menu({"Continue", "Save Game", "Quit to Main Menu"}),
        Menu({"Slot 1", "Slot 2", "Slot 3"}),
        Player());
    game.runGame();
    return 0;
}