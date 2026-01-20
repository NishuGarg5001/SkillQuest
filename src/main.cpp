#include "game.h"
#include "consolesettings.h"

int main()
{
    #ifdef _WIN32
        enableANSI();
        enableUTF8();
    #endif
    
    Game game = Game();
    game.runGame();
    return 0;
}