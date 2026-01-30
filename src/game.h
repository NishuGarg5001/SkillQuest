#ifndef GAME_H
#define GAME_H

#include "menu.h"
#include "player.h"
#include "random.h"

struct Command
{
    ActionVerb verb = INVALID;
    ResourceName target_resource = NO_RESOURCE;
    VerbObject verb_object = NO_OBJECT;

    explicit Command(ActionVerb verb, ResourceName target_resource, VerbObject verb_object) :
    verb(verb),
    target_resource(target_resource),
    verb_object(verb_object)
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
    int fps = 60;
    int frame_time = 1000 / fps;
    bool cursor_visible = false;
    Uint64 last_cursor_toggle = 0;
    std::deque<std::vector<Word>> text_buffer;
    std::string command = "";
    GameState game_state = GameState::MAIN;
    UIState ui_state = UIState::BLANK;
    UIState2 ui_state2 = UIState2::BLANK;
    Player player = Player();
    Menu main_menu = Menu({"New Game", "Load Game", "Quit"}, MAIN_MENU_BOX_WIDTH, MAIN_MENU_BOX_HEIGHT);
    Menu pause_menu =  Menu({"Continue", "Save Game", "Quit to Main Menu"}, PAUSE_MENU_BOX_WIDTH, PAUSE_MENU_BOX_HEIGHT);
    Menu save_menu = Menu({"Slot 1", "Slot 2", "Slot 3"}, SAVE_MENU_BOX_WIDTH, SAVE_MENU_BOX_HEIGHT);
    const Resource* player_target = nullptr;
    const std::array<Resource, 1> game_resources =
    {
        resource_list.at(COPPER)
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
                                        auto parsed_command_text = splitCommand();
                                        Command parsed_command = parseCommand(parsed_command_text);
                                        if (parsed_command.verb != ActionVerb::INVALID)
                                        {
                                            pushCommandToTextBuffer(std::move(parsed_command_text));
                                            handleCommand(parsed_command);
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
            text_buffer.clear();
            command.clear();
            ui_state = UIState::BLANK;
            ui_state2 = UIState2::BLANK;
            player.reset();
            player_target = nullptr;
        }

        void saveGame() const
        {

        }

        void loadGame()
        {

        }

        std::pair<std::string, std::string> splitCommand() const
        {
            size_t pos = command.find(' ');
            if(pos == std::string::npos)
                return {"", ""};
            std::string verb(command.data(), pos);
            std::string obj(command.data() + pos + 1);
            return {verb, obj};
        }

        Command parseCommand(std::pair<std::string_view, std::string_view> split_command) const
        {
            auto it = action_map.find(split_command.first);
            if(it == action_map.end())
                return Command(INVALID, NO_RESOURCE, NO_OBJECT);
            switch(it->second)
            {
                case MINE:
                {
                    auto it2 = ores_map.find(split_command.second);
                    if(it2 != ores_map.end())
                        return Command(it->second, it2->second, NO_OBJECT);
                    return Command(INVALID, NO_RESOURCE, NO_OBJECT);
                }
                case VIEW:
                {
                    if(split_command.second == "inventory")
                        return Command(it->second, NO_RESOURCE, INVENTORY);
                    if(split_command.second == "progress")
                        return Command(it->second, NO_RESOURCE, PROGRESS);
                }

            }
            return Command(INVALID, NO_RESOURCE, NO_OBJECT);
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

        void handleCommand(Command parsed_command)
        {
            if(parsed_command.verb == MINE) //expand to skill type command using ||
            {
                if(parsed_command.target_resource != NO_RESOURCE) //if it is a skill type command, also check for valid target resource
                    setPlayerTarget(parsed_command.target_resource);
                // add a condition to check for valid target item for example in case of burn logs skills can also be trained like this
                // later expand with if(player_target_resource || player_target_item) to handle both resource and item targets
                if(player_target)
                {
                    Skills skill = action_to_skill(parsed_command.verb);
                    if(!player.hasEnoughSkillLevel(skill, player_target->resource_min_level))
                    {
                        pushTextToTextBuffer({"You", "do", "not", "have", "enough", std::string(skill_to_verbose_l(skill)), "level!"},
                                            {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE});
                        return;
                    }
                    player.startAction(skill_to_playerstate(skill));
                }
            }
            else
            {
                switch(parsed_command.verb)
                {
                    case VIEW:
                    {
                        switch(parsed_command.verb_object)
                        {
                            case INVENTORY:
                                ui_state = UIState::UI_INVENTORY;
                                break;
                            case PROGRESS:
                                ui_state2 = UIState2::UI_PROGRESS;
                                break;
                        }
                        break;
                    }
                }
            }
        }

        std::vector<DropResult> extractResource() 
        //Extracts resource and adds it to inventory, returns name to updateState for verbose
        {
            if(player_target == nullptr)
                return {};
            //handle drop_rate = 0 case
            std::vector<DropResult> res = {};
            res.reserve(player_target->len);
            for (size_t i=0; i<player_target->len; i++)
            {
                if(random_int(0, player_target->drop_rates[i] - 1) == 0)
                        if(player.addItem(player_target->objects[i]))
                            res.emplace_back(player_target->objects[i].name, player_target->rarities[i], player_target->rarity_colors[i],
                            player_target->exps[i]);
            }
            if(!res.empty())
                return res;
            return {};
        }

        void updateState()
        {
            PlayerState action = player.getAction();
            if(action == MINING_STATE) //If a skill-based action
            {
                Skills skill = playerstate_to_skill(action);
                if(player_target)  //if player has a target resource, it is an extraction kind of skill task
                {
                    auto drop = extractResource();
                    if(drop.empty())
                    {
                        if(player.isInventoryFull())
                        {
                            pushTextToTextBuffer({"Your", "inventory", "is", "full!"}, {WHITE, WHITE, WHITE, WHITE});
                            player_target = nullptr;
                            player.startAction(NONE);
                            if(ui_state != UIState::BLANK)
                                ui_state = UIState::BLANK;
                            if(ui_state2 != UIState2::BLANK)
                                ui_state2 = UIState2::BLANK;
                        }
                        return;
                    }
                    else
                    {
                        for(size_t i=0; i<drop.size(); i++)
                        {
                            switch(action)
                            {
                                case MINING_STATE:
                                    pushTextToTextBuffer(
                                        {"You", "mined", "a", std::string(objects_map_inverse(drop[i].obj_name)) + "."}, 
                                        {WHITE, WHITE, WHITE, drop[i].rarity_color}
                                    );
                                    break;
                            }
                            std::string skill_str = std::string(skill_to_verbose_l(skill));
                            pushTextToTextBuffer(
                                        {"You", "gained", std::to_string(drop[i].exp), skill_str, "experience."},
                                        {WHITE, WHITE, WHITE, WHITE, WHITE}
                                    );
                            if(player.gainExperience(skill, drop[i].exp))
                                pushTextToTextBuffer(
                                    {"Congratulations!", "You", "have", "gained", "1", skill_str, "level.", "Your", skill_str, "level", "is", 
                                        "now",std::to_string(player.getLevel(skill)) + "."},
                                    std::vector<SDL_Color>(13, WHITE)
                                );
                        }
                    }
                }
                //else {} block to be added her for non-extraction skill based tasks
                /*
                else
                {
                    exp =
                }
                */
            }
            else
            {
                switch(action)
                {
                    case NONE:
                        break;
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

        void makeSpaceTextBuffer()
        {
            if(text_buffer.size() * static_cast<size_t>(FONT_SIZE) == HLINE_OFFSET)
            {
                text_buffer.pop_front();
                for (auto& line : text_buffer)
                    for (auto& word : line)
                        word.pos_y -= FONT_SIZE;
            }
        }

        void pushCommandToTextBuffer(std::pair<std::string, std::string> command)
        {
            makeSpaceTextBuffer();
            size_t w1 = getTextLen(command.first);
            size_t w2 = getTextLen(command.second);
            float x = 0.0f;
            float y = static_cast<float>(text_buffer.size()) * FONT_SIZE;
            float x2 = static_cast<float>(w1 + getTextLen(" "));
            auto& line = text_buffer.emplace_back();
            line.emplace_back(std::move(command.first), WHITE, x, y, w1);
            line.emplace_back(std::move(command.second), WHITE, x2, y, w2);
        }

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
                    float y = static_cast<float>(text_buffer.size() - 1) * FONT_SIZE;
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

        void drawText(const std::string& text, const float& x, const float& y, size_t w, const SDL_Color& color = WHITE) const
        {
            SDL_Surface* text_surface = TTF_RenderText_Blended(font, text.c_str(), text.size(), color);
            SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
            SDL_DestroySurface(text_surface);
            SDL_FRect dst {x, y, static_cast<float>(w), FONT_SIZE};
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
            if(ui_state2 == UIState2::UI_PROGRESS && player.getAction() != NONE)
            {
                Skills skill = playerstate_to_skill(player.getAction());
                std::string text = skill_to_verbose_u(skill) + " Lv." + std::to_string(player.getLevel(skill));
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
            if(ui_state == UIState::UI_INVENTORY)
            {
                SDL_SetRenderDrawColor(renderer, INVENTORY_LINE_COLOR.r, INVENTORY_LINE_COLOR.g, INVENTORY_LINE_COLOR.b, INVENTORY_LINE_COLOR.a);
                //horizontal lines
                for(size_t i = 0; i < INVENTORY_BOXES_PER_COL + 1; i++)
                {
                    float y = static_cast<float>(i * INVENTORY_BOX_HEIGHT + i * INVENTORY_LINE_WIDTH);
                    for(size_t w = 0; w < INVENTORY_LINE_WIDTH; w++)
                    {
                        float wf = static_cast<float>(w);
                        SDL_RenderLine(renderer, static_cast<float>(VLINE_OFFSET_RAW), y + wf, static_cast<float>(SCREEN_WIDTH - 1), y + wf);
                    }
                }
                //vertical lines
                for(size_t i = 0; i < INVENTORY_BOXES_PER_ROW + 1; i++)
                {
                    float x = static_cast<float>((VLINE_OFFSET_RAW - 2) + i * INVENTORY_BOX_WIDTH + i * INVENTORY_LINE_WIDTH + 1);
                    for(size_t w = 0; w < INVENTORY_LINE_WIDTH; w++)
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
                    float x = static_cast<float>((VLINE_OFFSET_RAW - 1) + col * (INVENTORY_LINE_WIDTH + INVENTORY_BOX_WIDTH) + INVENTORY_LINE_WIDTH);
                    float y = static_cast<float>(row * (INVENTORY_LINE_WIDTH + INVENTORY_BOX_HEIGHT) + INVENTORY_LINE_WIDTH);
                    SDL_FRect dst = {x, y, static_cast<float>(INVENTORY_BOX_WIDTH), static_cast<float>(INVENTORY_BOX_HEIGHT)};
                    SDL_RenderFillRect(renderer, &dst);
                    if(inventory[i].has_value())
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