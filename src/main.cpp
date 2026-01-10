#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <chrono>
#include <thread>
#include <cstdint>

constexpr size_t SCREEN_WIDTH  = 60;
constexpr size_t SCREEN_HEIGHT = 20;

enum InputKey
{
    KEY_EXTENDED1 = 0,
    KEY_EXTENDED2 = 224,
    UP = 72,
    DOWN = 80,
    ENTER = 13,
    ESC = 27
};

enum GameState : std::uint8_t
{
    QUIT,
    RUNNING,
    MAIN,
    PAUSE,
    SAVE
};

enum PlayerState : std::uint8_t
{
    NONE,
    MINING
};

class Player 
{
    std::uint8_t health;
    std::uint8_t mining;
    PlayerState player_state;

    public:
        explicit Player() : health(10), mining(1), player_state(NONE){}
};

class Menu
{
    //invariants
    //index < items.size()
    //0 < items.size <= SCREEN_HEIGHT
    //items[i].size() <= SCREEN_WIDTH - 4 for all 0<=i<items.size()
    //implies SCREEN_WIDTH > 3
    std::vector<std::string> items;
    size_t index;

    public:
        explicit Menu(std::vector<std::string> items) : items(std::move(items)), index(0)
        {
            if(this->items.empty())
                throw std::invalid_argument("Menu cannot be empty");
            if(this->items.size() > SCREEN_HEIGHT)
                throw std::invalid_argument("Menu cannot be bigger than screen height");
            if(SCREEN_WIDTH < 4)
                throw std::invalid_argument("Screen width too small to render menu");
            for(size_t i=0; i<this->items.size(); i++)
                if(this->items[i].size() > SCREEN_WIDTH - 4)
                    throw std::invalid_argument("Menu item cannot be bigger than screen width");
        }

        size_t getSize() const noexcept
        {
            return items.size();
        }

        const std::string& currentItem() const noexcept
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

        void renderMenu(std::vector<std::string>& frame_buffer) const noexcept
        {
            for(size_t i=0; i<items.size(); i++)
            {
                std::string x = (i==index?"":"    ")+items[i];
                frame_buffer[i] = x + std::string(SCREEN_WIDTH - x.size(), ' ');
            }
        }
};

class Game
{
    int fps;
    std::vector<std::string> frame_buffer, history;
    std::string command;
    GameState game_state;
    Player player;
    Menu main_menu, pause_menu, save_menu;

    public:
        explicit Game():
        fps(60),
        frame_buffer(std::vector<std::string>(SCREEN_HEIGHT, std::string(SCREEN_WIDTH, ' '))),
        history(),
        command(""),
        game_state(MAIN),
        main_menu(Menu({"New Game", "Load Game", "Quit"})),
        pause_menu(Menu({"Continue", "Save Game", "Quit to Main Menu"})),
        save_menu(Menu({"Slot 1", "Slot 2", "Slot 3"})),
        player(Player()){}

        void setFPS(int fps)
        {
            if(0 < fps < 61)
                this->fps = fps;
            else
                this->fps = 60;
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
                                std::string main_menu_item_name = main_menu.currentItem();
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
                                std::string pause_menu_item_name = pause_menu.currentItem();
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
                                updateHistory();
                                break;
                            }
                            default:
                            {
                                updateCommand(static_cast<char>(key));
                                break;
                            }
                        }
                    }
                }
            }
        }

        void newGame()
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

        void updateState()
        {
            //WIP
        }

        void hideCursor() const
        {
            std::cout<<"\x1b[?25l";
        }

        void showCursor() const
        {
            std::cout<<"\x1b[?25h";
        }

        void clearCommand()
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

        void echo()
        {
            size_t hsize = history.size();
            if(hsize < SCREEN_HEIGHT)
            {
                for(size_t i=0; i<hsize; i++)
                    frame_buffer[i] = history[i] + std::string(SCREEN_WIDTH - history[i].size(), ' ');
                frame_buffer[hsize] = command + std::string(SCREEN_WIDTH - command.size(), ' ');
            }
            else
            {
                size_t start = hsize - (SCREEN_HEIGHT - 1);
                for(size_t i=0; i<SCREEN_HEIGHT - 1; i++)
                    frame_buffer[i] = history[start + i] + std::string(SCREEN_WIDTH - history[start + i].size(), ' ');
                frame_buffer[SCREEN_HEIGHT - 1] = command + std::string(SCREEN_WIDTH - command.size(), ' ');
            }
        }

        void updateHistory()
        {
            history.push_back(command);
            clearCommand();
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
            for(const std::string& row: frame_buffer)
                std::cout<<row<<"\n";
            std::cout<<"\x1b["<<std::min(history.size(), SCREEN_HEIGHT - 1) + 1<<";"<<command.size() + 1<<"H";
            if(game_state == RUNNING)
                showCursor();
        }

        void runGame()
        {
            while(game_state != QUIT)
            {
                auto frameStart = std::chrono::high_resolution_clock::now();

                handleInput();
                if(game_state == RUNNING) updateState();
                renderFrame();

                std::chrono::milliseconds frameDuration(1000 / fps);
                auto frameEnd = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);
                if(elapsed < frameDuration) std::this_thread::sleep_for(frameDuration - elapsed);
            }
        }
};

int main()
{
    Game game = Game();
    game.runGame();
    return 0;
}