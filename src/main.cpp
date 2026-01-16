#include "game.h"
#include "enableansi.h"

int main()
{
    #ifdef _WIN32
        enableANSI();
    #endif
    
    Game game = Game();
    game.runGame();
    return 0;
}