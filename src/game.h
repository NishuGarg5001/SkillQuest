#ifndef GAME_H
#define GAME_H

#include "menu.h"
#include "player.h"
#include "random.h"

struct Command
{
    int action_index;
    std::optional<int> ui_index;
    std::optional<int> item_index;

    explicit Command(int action_index, std::optional<int> ui_index, std::optional<int> item_index = std::nullopt) noexcept : 
    action_index(action_index),
    ui_index(ui_index),
    item_index(item_index)
    {}
};

struct Word
{
    const std::string text;
    const SDL_Color color;
    const float pos_x;
    float pos_y;
    const size_t width;

    explicit Word(std::string text, SDL_Color color, float x, float y, size_t width) :
    text(std::move(text)),
    color(color),
    pos_x(x),
    pos_y(y),
    width(width)
    {}
};

class Game
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;
    int fps;
    int frame_time;
    bool cursor_visible;
    Uint64 last_cursor_toggle;
    std::deque<std::vector<Word>> text_buffer;
    std::string command;
    GameState game_state = GameState::MAIN;
    std::string ui_state;
    std::string ui_state2;
    Player player = Player();
    Menu main_menu = Menu({"New Game", "Load Game", "Quit"}, MAIN_MENU_BOX_WIDTH, MAIN_MENU_BOX_HEIGHT);
    Menu pause_menu =  Menu({"Continue", "Save Game", "Quit to Main Menu"}, PAUSE_MENU_BOX_WIDTH, PAUSE_MENU_BOX_HEIGHT);
    Menu save_menu = Menu({"Slot 1", "Slot 2", "Slot 3"}, SAVE_MENU_BOX_WIDTH, SAVE_MENU_BOX_HEIGHT);
    const Resource* player_resource_target;
    const Object* player_item_target;
    //name, objects, min_levels, exps, drop_rates
    const std::unordered_map<std::string_view, Resource> game_resources
    {
        {"ground", Resource("ground", {object_list.at("stone"), object_list.at("stick")}, {1, 1}, {4, 4}, {5, 5})},
        {"copper", Resource("copper", {object_list.at("copper ore")}, {1}, {4}, {5})},
        {"tin", Resource("tin", {object_list.at("tin ore")}, {1}, {4}, {5})},
        {"iron", Resource("iron", {object_list.at("iron ore")}, {10}, {20}, {10})},
        {"gold", Resource("gold", {object_list.at("gold ore")}, {20}, {30}, {15})}
    };

    public:
        Game(){}

        void handleInput()
        {
            SDL_Event event;
            while(SDL_PollEvent(&event))
            {
                (game_state == GameState::RUNNING) ? SDL_StartTextInput(window) : SDL_StopTextInput(window);
                switch(game_state)
                {
                    case GameState::MAIN:
                    {
                        if(event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat)
                        {
                            switch(event.key.key)
                            {
                                case SDLK_UP:
                                {
                                    main_menu.moveUp();
                                    break;
                                }
                                case SDLK_DOWN:
                                {
                                    main_menu.moveDown();
                                    break;
                                }
                                case SDLK_RETURN:
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
                        }
                        break;
                    }
                    case GameState::PAUSE:
                    {
                        if(event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat)
                        {
                            switch(event.key.key)
                            {
                                case SDLK_UP:
                                {
                                    pause_menu.moveUp();
                                    break;
                                }
                                case SDLK_DOWN:
                                {
                                    pause_menu.moveDown();
                                    break;
                                }
                                case SDLK_RETURN:
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
                                case SDLK_ESCAPE:
                                {
                                    game_state = GameState::RUNNING;
                                    break;
                                }
                            }
                        }
                        break;
                    }
                    case GameState::SAVE:
                    {
                        if(event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat)
                        {
                            switch(event.key.key)
                            {
                                case SDLK_UP:
                                {
                                    save_menu.moveUp();
                                    break;
                                }
                                case SDLK_DOWN:
                                {
                                    save_menu.moveDown();
                                    break;
                                }
                                case SDLK_RETURN:
                                {
                                    saveGame();
                                    game_state = GameState::RUNNING;
                                    break;
                                }
                                case SDLK_ESCAPE:
                                {
                                    game_state = GameState::PAUSE;
                                    break;
                                }
                            }
                        }
                        break;
                    }
                    case GameState::RUNNING:
                    {
                        if(event.type == SDL_EVENT_TEXT_INPUT)
                        {
                            if(getTextLen(command) + getTextLen(event.text.text) <= VLINE_OFFSET)
                                command += event.text.text;
                        }

                        else if(event.type == SDL_EVENT_KEY_DOWN)
                        {
                            if(!event.key.repeat)
                            {
                                switch(event.key.key)
                                {
                                    case SDLK_ESCAPE:
                                    {
                                        game_state = GameState::PAUSE;
                                        break;
                                    }
                                    case SDLK_RETURN:
                                    {
                                        auto parsed_command_text = splitCommand(command);
                                        auto packet = validateCommand(parsed_command_text);
                                        if (packet)
                                        {
                                            pushTextToTextBuffer(parsed_command_text, std::vector<SDL_Color>(parsed_command_text.size(), WHITE));
                                            handleCommand(*packet, parsed_command_text);
                                        }
                                        command.clear();
                                        break;
                                    }
                                    default:
                                    {
                                        break;
                                    }
                                }
                            }

                            if(event.key.key == SDLK_BACKSPACE)
                            {
                                if(!command.empty())
                                    command.pop_back();
                            }
                        }
                        break;
                    }
                }
                if(event.type == SDL_EVENT_QUIT)
                    game_state = GameState::QUIT;
            }
        }

        void newGame() noexcept
        {
            SDL_RenderClear(renderer);
            fps = 60;
            frame_time = 1000 / fps;
            cursor_visible = false;
            last_cursor_toggle = 0;
            text_buffer.clear();
            command.clear();
            ui_state = "inventory";
            ui_state2 = invalid_ui_state;
            player.reset();
            player_resource_target = nullptr;
            player_item_target = nullptr;
        }

        void saveGame() const
        {

        }

        void loadGame()
        {

        }

        std::vector<std::string> splitCommand(std::string command) const
        {
            std::vector<std::string> res;
            size_t pos = command.find(' ');
            while(pos != std::string::npos)
            {
                res.push_back(command.substr(0, pos));
                command.erase(0, pos + 1);
                pos = command.find(' ');
            }
            if(!command.empty())
                res.push_back(command);
            return res;
        }

        std::string join(const std::vector<std::string>& split_command) const
        {
            std::string res;
            for(size_t i=1; i<split_command.size()-1; i++)
                res += split_command[i] + ((i == split_command.size() - 2) ? "" : " ");
            return res;
        }

        std::optional<int> isValidAction(std::string_view input) const
        {
            for(int i=0; i<valid_actions.size(); i++)
                if(input == valid_actions[i])
                    return i;
            return std::nullopt; 
        }

        bool isValidForage(std::string_view input) const
        {
            for(size_t i=0; i<valid_forage_sources.size(); i++)
                if(input == valid_forage_sources[i])
                    return true;
            return false;
        }

        bool isValidOre(std::string_view input) const
        {
            for(size_t i=0; i<valid_mining_sources.size(); i++)
                if(input == valid_mining_sources[i])
                    return true;
            return false;
        }

        std::optional<int> isValidUI(std::string_view input) const
        {
            for(size_t i=0; i<valid_ui_states.size(); i++)
                if(input == valid_ui_states[i])
                    return static_cast<int>(i);
            return std::nullopt;
        }

        bool isValidNumber(std::string_view input) const
        {
            for(char c : input)
                if(!std::isdigit(c))
                    return false;
            return true;
        }

        std::optional<int> isValidItem(std::string_view input) const
        {
            for(size_t i=0; i<valid_items.size(); i++)
                if(input == valid_items[i])
                    return static_cast<int>(i);
            return std::nullopt;
        }

        std::optional<Command> validateCommand(const std::vector<std::string>& split_command)
        {
            std::optional<int> action_index = isValidAction(split_command[0]);
            if(!action_index)
                return std::nullopt;
            switch(*action_index)
            {
                case 0:
                {
                    return Command(*action_index, std::nullopt, std::nullopt);
                }
                case 1:
                {
                    if(split_command.size() < 2 || !isValidOre(split_command[1]))
                        return std::nullopt;
                    return Command(*action_index, std::nullopt, std::nullopt);
                }

                case 2:
                {
                    if(split_command.size() < 2 || !isValidForage(split_command[1]))
                        return std::nullopt;
                    return Command(*action_index, std::nullopt, std::nullopt);
                }

                case 3:
                {
                    if(split_command.size() < 2)
                        return std::nullopt;
                    std::optional<int> ui_index = isValidUI(split_command[1]);
                    if(!ui_index)
                        return std::nullopt;
                    return Command(*action_index, ui_index, std::nullopt);
                }

                case 4:
                {
                    if(ui_state2 != valid_ui_states[2] || split_command.size() < 3 || !isValidNumber(split_command[split_command.size() - 1]))
                        return std::nullopt;
                    std::string item = join(split_command);
                    std::optional<int> item_index = isValidItem(item);
                    if(!item_index || !player.hasInInventory(item))
                        return std::nullopt;
                    return Command(*action_index, std::nullopt, item_index);
                }
            }
            return std::nullopt;
        }

        bool setPlayerTarget(const std::string& item)
        {
            auto it = game_resources.find(item);
            if (it != game_resources.end())
            {
                player_resource_target = &it->second;
                return true;
            }
            player_resource_target = nullptr;
            return false;
        }

        void handleCommand(const Command& packet, const std::vector<std::string>& split_command)
        {
            /*pushTextToTextBuffer({split_command[0]},
                                                {WHITE});
            pushTextToTextBuffer({split_command[1]},
                                                {WHITE});
            */
            switch(packet.action_index)
            {
                case 0:
                {
                    player.stop();
                    player_resource_target = nullptr;
                }
                break;

                case 1:
                case 2:
                {
                    ui_state2 = "";
                    setPlayerTarget(split_command[1]);
                    if(player_resource_target)
                    {
                        std::string skill = action_to_skill(split_command[0]);
                        if(!player.hasEnoughSkillLevel(skill, player_resource_target->resource_min_level))
                        {
                            pushTextToTextBuffer({"You", "do", "not", "have", "enough", skill, "level!"},
                                                {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE});
                            return;
                        }
                        player.startAction(skill);
                        pushTextToTextBuffer({"You", "started", "to", split_command[0], split_command[1] + "..."},
                                            {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE});
                    }
                }
                break;

                case 3:
                {
                    switch(*packet.ui_index)
                    {
                        case 0:
                        ui_state2 = split_command[1];
                        break;
                        case 1:
                        ui_state = split_command[1];
                        break;
                        case 2:
                        {
                            player.stop();
                            player_resource_target = nullptr;
                            ui_state2 = split_command[1];
                        }
                        break;
                    }
                }
                break;

                case 4:
                {
                    std::string item = valid_items[*packet.item_index];
                    if(player.isVaultFull() && !player.hasInVault(item))
                    {
                        pushTextToTextBuffer({"Your", "vault", "is", "full!"}, {WHITE, WHITE, WHITE, WHITE});
                        return;
                    }
                    player.depositItem(item, std::stoi(split_command.back()));
                }
                break;
            }
        }

        std::vector<DropResult> extractResource() 
        //Extracts resource and adds it to inventory, returns name to updateState for verbose
        {
            if(player_resource_target == nullptr)
                return {};
            //handle drop_rate = 0 case
            std::vector<DropResult> res = {};
            res.reserve(player_resource_target->len);
            for (size_t i=0; i<player_resource_target->len; i++)
            {
                if(random_int(0, player_resource_target->drop_rates[i] - 1) == 0)
                        if(player.addItem(player_resource_target->objects[i]))
                            res.emplace_back(player_resource_target->objects[i].name, player_resource_target->rarities[i], player_resource_target->rarity_colors[i],
                            player_resource_target->exps[i]);
            }
            if(!res.empty())
                return res;
            return {};
        }

        void stopExtraction()
        {
            pushTextToTextBuffer({"Your", "inventory", "is", "full!"}, {WHITE, WHITE, WHITE, WHITE});
            player_resource_target = nullptr;
            player.stop();
            ui_state = invalid_ui_state;
            ui_state2 = invalid_ui_state;
        }

        void gainExperience(const std::string& action, int exp)
        {
            pushTextToTextBuffer
                (
                    {"You", "gained", std::to_string(exp), action, "experience."},
                    {WHITE, WHITE, WHITE, WHITE, WHITE}
                );

            if(player.gainExperience(action, exp))
                pushTextToTextBuffer
                (
                    {"Congratulations!", "You", "have", "gained", "1", action, "level.", "Your", action, "level", "is", 
                        "now", std::to_string(player.getLevel(action)) + "."},
                    std::vector<SDL_Color>(13, WHITE)
                );
        }

        void updateState()
        {
            std::string action = player.getAction();
            if(player_resource_target)  //if player has a target resource, it is an extraction kind of skill task
            {
                auto drop = extractResource();
                if(drop.empty() && player.isInventoryFull())
                {
                    stopExtraction();
                    return;
                }
                for(size_t i=0; i<drop.size(); i++)
                {
                    if(action == "mining")
                        pushTextToTextBuffer(
                            {"You", "mined", "a", std::string(drop[i].obj_name) + "."}, 
                            {WHITE, WHITE, WHITE, drop[i].rarity_color}
                        );
                    else if(action == "foraging")
                        pushTextToTextBuffer(
                            {"You", "found", "a", std::string(drop[i].obj_name) + "."}, 
                            {WHITE, WHITE, WHITE, drop[i].rarity_color}
                        );
                    gainExperience(action, drop[i].exp);
                }
            }
        }

        size_t getTextLen(const std::string& text) const noexcept
        {
            int w, h;
            w = h = 0;
            TTF_GetStringSize(font, text.c_str(), text.size(), &w, &h);
            return static_cast<size_t>(w);
        }

        void shiftTextUp()
        {
            for (auto& line : text_buffer)
                for (auto& word : line)
                    word.pos_y -= FONT_SIZE;
        }

        void makeSpaceTextBuffer()
        {
            if(text_buffer.size() == NUM_LINES)
                text_buffer.pop_front();
            shiftTextUp();
        }

        /*void pushCommandToTextBuffer(std::vector<std::string> command)
        {
            makeSpaceTextBuffer();
            size_t w1 = getTextLen(command[0]);
            size_t w2 = getTextLen(command[1]);
            float x = 0.0f;
            float y = static_cast<float>(text_buffer.size()) * FONT_SIZE;
            float x2 = static_cast<float>(w1 + getTextLen(" "));
            auto& line = text_buffer.emplace_back();
            line.emplace_back(std::move(command[0]), WHITE, x, y, w1);
            line.emplace_back(std::move(command[1]), WHITE, x2, y, w2);
        }*/

        void pushTextToTextBuffer(const std::vector<std::string>& words, const std::vector<SDL_Color>& colors)
        {
            makeSpaceTextBuffer();
            text_buffer.emplace_back();
            const size_t space_size = getTextLen(" ");
            size_t buffer_counter = 0;
            for (size_t i = 0; i < words.size(); i++)
            {
                size_t w = getTextLen(words[i]);
                if(buffer_counter + w <= VLINE_OFFSET)
                {
                    float x = static_cast<float>(buffer_counter);
                    float y = static_cast<float>(HLINE_OFFSET - FONT_SIZE);
                    text_buffer.back().emplace_back(words[i], colors[i], x, y, w);
                    buffer_counter += w + space_size;
                }
                else
                {
                    makeSpaceTextBuffer();
                    text_buffer.emplace_back();
                    buffer_counter = 0;
                    i--;
                }
            }
        }

        void drawText(const std::string& text, const float& x, const float& y, size_t w, const SDL_Color& color = WHITE,
        const float& h = FONT_SIZE) const
        {
            SDL_Surface* text_surface = TTF_RenderText_Blended(font, text.c_str(), text.size(), color);
            SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
            SDL_DestroySurface(text_surface);
            SDL_FRect dst {x, y, static_cast<float>(w), h};
            SDL_RenderTexture(renderer, text_texture, nullptr, &dst);
            SDL_DestroyTexture(text_texture);
        }

        void echoText() const
        {
            for(const std::vector<Word>& word_line : text_buffer)
                for(const Word& word : word_line)
                    drawText(word.text, word.pos_x, word.pos_y, word.width, word.color);
            drawText(command, 0.0f, static_cast<float>(HLINE_OFFSET), getTextLen(command));
        }

        void drawCursor() const
        {
            if(cursor_visible)
            {
                SDL_FRect cursor_rect {static_cast<float>(getTextLen(command)), static_cast<float>(HLINE_OFFSET), 
                    static_cast<float>(CURSOR_WIDTH), static_cast<float>(FONT_SIZE)};
                SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
                SDL_RenderFillRect(renderer, &cursor_rect);
            }
        }

        void renderUI()
        {
            if(ui_state == "inventory")
            {
                SDL_SetRenderDrawColor(renderer, INVENTORY_LINE_COLOR.r, INVENTORY_LINE_COLOR.g, INVENTORY_LINE_COLOR.b, INVENTORY_LINE_COLOR.a);
                //horizontal lines
                for(size_t i = 0; i < INVENTORY_BOXES_PER_COL + 1; i++)
                {
                    float y = static_cast<float>(i * INVENTORY_BOX_HEIGHT + i * LINE_WIDTH);
                    for(size_t w = 0; w < LINE_WIDTH; w++)
                    {
                        float wf = static_cast<float>(w);
                        SDL_RenderLine(renderer, static_cast<float>(VLINE_OFFSET_RAW), y + wf, static_cast<float>(SCREEN_WIDTH - 1), y + wf);
                    }
                }
                //vertical lines
                for(size_t i = 0; i < INVENTORY_BOXES_PER_ROW + 1; i++)
                {
                    float x = static_cast<float>((VLINE_OFFSET_RAW - 2) + i * INVENTORY_BOX_WIDTH + i * LINE_WIDTH + 1);
                    for(size_t w = 0; w < LINE_WIDTH; w++)
                    {
                        if(!(i == 0 && w == 0))
                        {
                            float wf = static_cast<float>(w);
                            SDL_RenderLine(renderer, x + wf, 0.0f, x + wf, static_cast<float>(INVENTORY_END - 1));
                        }
                    }
                }
                //inventory boxes
                SDL_SetRenderDrawColor(renderer, INVENTORY_BOX_COLOR.r, INVENTORY_BOX_COLOR.g, INVENTORY_BOX_COLOR.b, INVENTORY_BOX_COLOR.a);
                auto inventory = player.getInventory();
                for (size_t i = 0; i < INVENTORY_SIZE; i++)
                {
                    size_t row = i / INVENTORY_BOXES_PER_ROW;
                    size_t col = i % INVENTORY_BOXES_PER_ROW;
                    float x = static_cast<float>((VLINE_OFFSET_RAW - 1) + col * (LINE_WIDTH + INVENTORY_BOX_WIDTH) + LINE_WIDTH);
                    float y = static_cast<float>(row * (LINE_WIDTH + INVENTORY_BOX_HEIGHT) + LINE_WIDTH);
                    SDL_FRect dst = {x, y, static_cast<float>(INVENTORY_BOX_WIDTH), static_cast<float>(INVENTORY_BOX_HEIGHT)};
                    SDL_RenderFillRect(renderer, &dst);
                    if(inventory[i])
                    {
                        SDL_Texture* tex = IMG_LoadTexture(renderer, inventory[i]->path.c_str());
                        if(!tex)
                        {
                            std::cerr<<"Failed to create window: "<<SDL_GetError()<< "\n";
                            game_state = GameState::QUIT;
                            break;
                        }
                        
                        SDL_RenderTexture(renderer, tex, nullptr, &dst);
                        SDL_DestroyTexture(tex);
                    }
                }
            }

            if(ui_state2 == "progress")
            {
                if(player.getAction() != invalid_action)
                {
                    std::string skill = player.getAction();
                    std::string skill_upper = skill;
                    skill_upper[0] = std::toupper(skill_upper[0]);
                    std::string text = skill_upper + " Lv." + std::to_string(player.getLevel(skill));
                    drawText(text, static_cast<float>(VLINE_OFFSET_RAW), static_cast<float>(HLINE_OFFSET - FONT_SIZE), 
                            getTextLen(text));
                    size_t num_rects = std::min(player.expProgress(skill), PROGRESSBAR_PARTITIONS);
                    for(size_t i = 0; i < num_rects; i++)
                    {
                        SDL_FRect progress_rect {static_cast<float>(VLINE_OFFSET_RAW + i * (PROGRESS_BAR_WIDTH + PROGRESSBAR_SPACING)), 
                                                static_cast<float>(HLINE_OFFSET),
                                                static_cast<float>(PROGRESS_BAR_WIDTH),
                                                static_cast<float>(FONT_SIZE / 2.0f)};
                        SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
                        SDL_RenderFillRect(renderer, &progress_rect);
                    }
                }
            }
            
            else if(ui_state2 == "vault")
            {
                SDL_SetRenderDrawColor(renderer, INVENTORY_LINE_COLOR.r, INVENTORY_LINE_COLOR.g, INVENTORY_LINE_COLOR.b, INVENTORY_LINE_COLOR.a);
                //horizontal lines
                for(size_t i = 0; i < VAULT_BOXES_PER_COL + 1; i++)
                {
                    float y = static_cast<float>(INVENTORY_END + INVENTORY_VAULT_V_OFFSET + i * VAULT_BOX_HEIGHT + i * LINE_WIDTH);
                    for(size_t w = 0; w < LINE_WIDTH; w++)
                    {
                        float wf = static_cast<float>(w);
                        SDL_RenderLine(renderer, static_cast<float>(VLINE_OFFSET_RAW), y + wf, static_cast<float>(SCREEN_WIDTH - 1), y + wf);
                    }
                }
                //vertical lines
                for(size_t i = 0; i < VAULT_BOXES_PER_ROW + 1; i++)
                {
                    float x = static_cast<float>((VLINE_OFFSET_RAW - 2) + i * VAULT_BOX_WIDTH + i * LINE_WIDTH + 1);
                    for(size_t w = 0; w < LINE_WIDTH; w++)
                    {
                        if(!(i == 0 && w == 0))
                        {
                            float wf = static_cast<float>(w);
                            SDL_RenderLine(renderer, x + wf, INVENTORY_END + INVENTORY_VAULT_V_OFFSET, x + wf, static_cast<float>(VAULT_END - 1));
                        }
                    }
                }
                //vault boxes
                SDL_SetRenderDrawColor(renderer, INVENTORY_BOX_COLOR.r, INVENTORY_BOX_COLOR.g, INVENTORY_BOX_COLOR.b, INVENTORY_BOX_COLOR.a);
                auto vault = player.getVault();
                for (size_t i = 0; i < VAULT_SIZE; i++)
                {
                    size_t row = i / VAULT_BOXES_PER_ROW;
                    size_t col = i % VAULT_BOXES_PER_ROW;
                    float x = static_cast<float>((VLINE_OFFSET_RAW - 1) + col * (LINE_WIDTH + VAULT_BOX_WIDTH) + LINE_WIDTH);
                    float y = static_cast<float>(row * (LINE_WIDTH + VAULT_BOX_HEIGHT) + INVENTORY_END + INVENTORY_VAULT_V_OFFSET + LINE_WIDTH);
                    SDL_FRect dst = {x, y, static_cast<float>(VAULT_BOX_WIDTH), static_cast<float>(VAULT_BOX_HEIGHT)};
                    SDL_RenderFillRect(renderer, &dst);
                    if(vault[i])
                    {
                        SDL_Texture* tex = IMG_LoadTexture(renderer, vault[i]->first.path.c_str());
                        if(!tex)
                        {
                            std::cerr<<"Failed to create window: "<<SDL_GetError()<< "\n";
                            game_state = GameState::QUIT;
                            break;
                        }
                        
                        SDL_RenderTexture(renderer, tex, nullptr, &dst);
                        SDL_DestroyTexture(tex);

                        std::string text = std::to_string(vault[i]->second);
                        drawText(text, x + dst.w - getTextLen(text), y, getTextLen(text), WHITE, FONT_SIZE/1.5f);
                    }
                }
            }
        }

        void renderFrame()
        {
            SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
            SDL_RenderClear(renderer);
            switch(game_state)
            {
                case GameState::MAIN:
                {
                    main_menu.renderMenu(renderer, font);
                    break;
                }
                case GameState::PAUSE:
                {
                    pause_menu.renderMenu(renderer, font);
                    break;
                }
                case GameState::SAVE:
                {
                    save_menu.renderMenu(renderer, font);
                    break;
                }
                case GameState::RUNNING:
                {
                    Uint64 now = SDL_GetTicks();
                    if (now - last_cursor_toggle >= CURSOR_BLINK_TIME)
                    {
                        cursor_visible = !cursor_visible;
                        last_cursor_toggle = now;
                    }
                    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
                    SDL_RenderLine(renderer, 0, HLINE_OFFSET - 1, VLINE_OFFSET_RAW - 1, HLINE_OFFSET - 1); //hline
                    SDL_RenderLine(renderer, VLINE_OFFSET_RAW - 1, 0, VLINE_OFFSET_RAW - 1, SCREEN_HEIGHT - 1); //vline
                    echoText();
                    drawCursor();
                    renderUI();
                    break;
                }
                default:
                    break;
            }
            SDL_RenderPresent(renderer);
        }

        int runGame()
        {
            if(!SDL_Init(SDL_INIT_VIDEO))
            {
                std::cerr<<"Failed to initialize SDL: "<<SDL_GetError()<<"\n";
                return 1;
            }

            window = SDL_CreateWindow("Start", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
            if(!window)
            {
                std::cerr<<"Failed to create window: "<<SDL_GetError()<< "\n";
                SDL_Quit();
                return 2;
            }

            renderer = SDL_CreateRenderer(window, nullptr);
            if (!renderer) 
            {
                std::cerr << "Failed to create renderer: " << SDL_GetError() << "\n";
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 3;
            }

            if (!TTF_Init())
            {
                std::cerr << "Failed to initialize TTF: " << SDL_GetError() << "\n";
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 4;
            }

            font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
            if (!font) 
            {
                std::cerr << "Failed to open font: " << SDL_GetError() << "\n";
                TTF_Quit();
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 5;
            }

            Uint64 last = SDL_GetTicks();
            Uint64 accumulator = 0;

            while(game_state != GameState::QUIT)
            {
                Uint64 current = SDL_GetTicks();
                Uint64 delta = current - last;
                last = current;
                accumulator += delta;

                handleInput();
                while(game_state == GameState::RUNNING && accumulator >= TICK)
                {
                    updateState();
                    accumulator -= TICK;
                }

                renderFrame();

                Uint64 frame_time_elapsed = SDL_GetTicks() - current;
                if(frame_time_elapsed < frame_time)
                    SDL_Delay(frame_time - frame_time_elapsed);
            }

            TTF_CloseFont(font);
            TTF_Quit();
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 0;
        }
};

#endif