#ifndef GAME_H
#define GAME_H

#include "menu.h"
#include "game_screen.h"
#include "text_screen.h"
#include "icon_screen.h"
#include "ui_screen.h"
#include "resources.h"
#include "player.h"

class Game
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    TTF_Font *font = nullptr;
    int fps = 60;
    int frame_time = 1000/fps;
    GameState game_state = GameState::MAIN;
    Menu main_menu = Menu({"New Game", "Load Game", "Quit"}, MAIN_MENU_BOX_WIDTH, MAIN_MENU_BOX_HEIGHT);
    Menu pause_menu =  Menu({"Continue", "Save Game", "Quit to Main Menu"}, PAUSE_MENU_BOX_WIDTH, PAUSE_MENU_BOX_HEIGHT);
    Menu save_menu = Menu({"Slot 1", "Slot 2", "Slot 3"}, SAVE_MENU_BOX_WIDTH, SAVE_MENU_BOX_HEIGHT);
    GameScreen game_screen = GameScreen(GS_X, GS_Y, GS_W, GS_H);
    TextScreen text_screen = TextScreen(TS_X, TS_Y, TS_W, TS_H);
    IconScreen icons_screen = IconScreen(IS_X, IS_Y, IS_W, IS_H);
    UIScreen ui_screen = UIScreen(UIS_X, UIS_Y, UIS_W, UIS_H);
    Player player = Player();

    public:
        Game(){}

        void handleInput()
        {
            SDL_Event event;
            while(SDL_PollEvent(&event))
            {
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
                        if(event.type == SDL_EVENT_KEY_DOWN)
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
                                    case SDLK_M:
                                    {
                                        game_screen.setPlayerTarget(COPPER);
                                        ui_screen.setState(UIState::INVENTORY);
                                        player.startAction(MINING);
                                        text_screen.startedMining(game_screen.getPlayerTarget()->name_str, font);
                                        break;
                                    }
                                    default:
                                    {
                                        break;
                                    }
                                }
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
            player.reset();
            game_screen.stopExtraction();
            ui_screen.setState(UIState::NONE);
            text_screen.clearTextBuffer();
            text_screen.pushTextToTextBuffer({"Welcome", "to", "SkillQuest!"}, {WHITE, WHITE, WHITE}, font);
        }

        void saveGame() const
        {

        }

        void loadGame()
        {

        }

        void updateState()
        {
            switch(player.getAction())
            {
                case IDLE:
                {
                    break;
                }
                case MINING:
                {
                    if(game_screen.getPlayerTarget())
                    {
                        auto drop = game_screen.extractResource(player);
                        if(drop.empty() && player.isInventoryFull())
                        {
                            text_screen.inventoryFull(font);
                            game_screen.stopExtraction();
                            player.stopAction();
                            return;
                        }
                        for(size_t i=0; i<drop.size(); i++)
                            text_screen.mineSuccess(drop[i].obj_name_str, drop[i].rarity_color, font);
                    }
                    break;
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
                    game_screen.render(renderer);
                    text_screen.render(renderer, font);
                    icons_screen.render(renderer);
                    ui_screen.render(renderer, player);
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