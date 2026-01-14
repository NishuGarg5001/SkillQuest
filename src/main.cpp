#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <chrono>
#include <thread>
#include <cstdint>
#include <optional>
#include <string_view>
#include <unordered_map>

constexpr size_t SCREEN_WIDTH  = 120;
constexpr size_t SCREEN_HEIGHT = 27;
constexpr auto tick = std::chrono::milliseconds(600);
using high_clock = std::chrono::high_resolution_clock;

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
    MINING_STATE
};

enum ActionVerb : std::uint8_t
{
    NO_ACTION,
    MINE
};

enum Resources : std::uint8_t
{
    NO_RESOURCE,
    COPPER,
    TIN,
    IRON,
    GOLD
};

enum Objects : std::uint8_t
{
    NO_ITEM,
    COPPER_ORE,
    TIN_ORE,
    IRON_ORE,
    GOLD_ORE
};

enum Skills : std::uint8_t
{
    HEALTH,
    MINING
};

const std::unordered_map<std::string_view, ActionVerb> action_map = {
    {"mine", MINE}
};

const std::unordered_map<std::string_view, Resources> ores_map = {
    {"copper", COPPER},
    {"tin", TIN},
    {"iron", IRON},
    {"gold", GOLD}
};

const std::unordered_map<Objects, std::string_view> objects_map_inverse = {
    {COPPER_ORE, "copper ore"},
    {TIN_ORE, "tin ore"},
    {IRON_ORE, "iron ore"},
    {GOLD_ORE, "gold ore"}
};

struct Resource
{
    Resources name;
    Skills skill;
    Objects object_name;
    const uint8_t level;
    const uint32_t exp;
    
    explicit Resource(Resources name, Skills skill, Objects object_name, uint8_t level, uint32_t exp) noexcept :
    name(name),
    skill(skill),
    object_name(object_name),
    level(level),
    exp(exp){}
};

struct Object
{
    Objects name;

    explicit Object(Objects name) noexcept : name(name){}
};

class Location
{
    std::string name;
    const std::vector<Resource> resources;

    public:
        explicit Location(std::string name, std::vector<Resource> resources) : name(name), resources(resources){}

        const std::vector<Resource>& getResources() const noexcept
        {
            return resources;
        }
};

class Player 
{
    std::uint8_t health;
    std::uint8_t mining;
    std::vector<std::optional<Object>> inventory;
    PlayerState player_state;
    const Location* player_location;
    const Resource* resource_target;

    public:
        Player() noexcept: health(10), mining(1), inventory(), player_state(NONE), player_location(nullptr), resource_target(nullptr)
        {
            inventory.resize(28);
        }

        void moveTo(const Location& location)
        {
            player_location = &location;
        }

        const bool hasEnoughSkillLevel(Skills skill) const noexcept
        {
            switch(skill)
            {
                case MINING:
                {
                    if(mining >= resource_target->level)
                        return true;
                    break;
                }
            }
            return false;
        }

        const bool locationHasResource(Resources item) const
        {
            for(const Resource& resource : player_location->getResources())
                if(item == resource.name)
                    return true;
            return false;
        }

        void setResourceTarget(Resources item)
        {
            for(const Resource& resource : player_location->getResources())
                if(item == resource.name)
                    {
                        resource_target = &resource;
                        break;
                    }
        }

        PlayerState getAction() const noexcept
        {
            return player_state;
        }

        void startAction(PlayerState action) noexcept
        {
            player_state = action;
        }

        const bool addItem(Object item) //Cannot be Object& because resource needs to return object.name
        //and after move, item will be eaten up by slot and if item is a reference to object, object will not be able
        //to return object.name as it will become empty
        {
            for(auto& slot : inventory)
                if(!slot.has_value())
                {
                    slot.emplace(std::move(item));
                    return true;
                }
            return false;
        }

        Objects extractResource()
        {
            Object object(resource_target->object_name);
            if(addItem(object))
                return object.name;
            return NO_ITEM;
        }
};

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

class Game
{
    //invariants
    //0 < fps < 61

    uint8_t fps;
    std::vector<std::string> frame_buffer, history;
    std::string command;
    GameState game_state;
    std::vector<Location> locations;
    Player player;
    Menu main_menu, pause_menu, save_menu;

    public:
        explicit Game():
        fps(60),
        frame_buffer(std::vector<std::string>(SCREEN_HEIGHT, std::string(SCREEN_WIDTH, ' '))),
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
                                    history.push_back(command);
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

        void updateState()
        {
            switch(player.getAction())
            {
                case MINING_STATE:
                {
                    Objects object_name = player.extractResource();
                    if(object_name == NO_ITEM)
                    {
                        history.push_back("Your inventory is full!");
                        player.startAction(NONE);
                        return;
                    }
                    history.push_back(std::string("You mined a ").append(objects_map_inverse.find(object_name)->second).append("."));
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
            size_t pos = command.find(' ');
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
            std::cout<<"\x1b["<<SCREEN_HEIGHT<<";"<<command.size() + 1<<"H";
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

int main()
{
    Game game = Game();
    game.runGame();
    return 0;
}