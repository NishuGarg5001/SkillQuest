#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <chrono>
#include <thread>
#include <cstdint>

constexpr size_t SCREEN_WIDTH  = 120;
constexpr size_t SCREEN_HEIGHT = 27;

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
    //0 < menu_box_width <= SCREEN_WIDTH
    //0 < menu_box_height <= SCREEN_HEIGHT
    //0 < items.size <= menu_box_height - 2
    //implies menu_box_height > 2
    //items[i].size() <= menu_box_width - 4 for all 0<=i<items.size()
    //implies menu_box_width > 4
    std::vector<std::string> items;
    size_t index, menu_box_width, menu_box_height;

    public:
        explicit Menu(std::vector<std::string> items, size_t menu_box_width, size_t menu_box_height) : 
        items(std::move(items)),
        index(0),
        menu_box_width(menu_box_width),
        menu_box_height(menu_box_height)
        {
            if(this->items.empty())
                throw std::invalid_argument("Menu cannot be empty");
            if(this->menu_box_width == 0)
                throw std::invalid_argument("Menu box width cannot be 0");
            if(this->menu_box_width > SCREEN_WIDTH)
                throw std::invalid_argument("Menu box width cannot be more than screen width");
            if(menu_box_height == 0)
                throw std::invalid_argument("Menu box height cannot be 0");
            if(this->menu_box_height > SCREEN_HEIGHT)
                throw std::invalid_argument("Menu box height cannot be more than screen height");
            if(this->menu_box_width < 5)
                throw std::invalid_argument("Menu box width too small to render any menu item");
            for(size_t i=0; i<this->items.size(); i++)
                if(this->items[i].size() > menu_box_width - 4)
                    throw std::invalid_argument("Menu item cannot be bigger than menu box width");
            if(this->menu_box_height < 3)
                throw std::invalid_argument("Menu box height too small to render any menu");
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

class Game
{
    //invariants
    //0 < fps < 61

    uint8_t fps;
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
        main_menu(Menu({"New Game", "Load Game", "Quit"}, 13, 5)),
        pause_menu(Menu({"Continue", "Save Game", "Quit to Main Menu"}, 21, 5)),
        save_menu(Menu({"Slot 1", "Slot 2", "Slot 3"}, 10, 5)),
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

        void updateState()
        {
            //WIP
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

        void echo()
        {
            size_t hsize = history.size();
            if(hsize < SCREEN_HEIGHT)
            {
                for(size_t i=0; i<hsize; i++)
                    frame_buffer[i] = history[i] + std::string(SCREEN_WIDTH - history[i].size(), ' ');
            }
            else
            {
                size_t start = hsize - (SCREEN_HEIGHT - 1);
                for(size_t i=0; i<SCREEN_HEIGHT - 1; i++)
                    frame_buffer[i] = history[start + i] + std::string(SCREEN_WIDTH - history[start + i].size(), ' ');
            }
            frame_buffer[SCREEN_HEIGHT - 1] = command + std::string(SCREEN_WIDTH - command.size(), ' ');
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
            std::cout<<"\x1b["<<SCREEN_HEIGHT<<";"<<command.size() + 1<<"H";
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