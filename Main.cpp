#include "Game.h"

int main()
{
    constexpr unsigned int WINDOW_WIDTH  = 1024;
    constexpr unsigned int WINDOW_HEIGHT = 768;
    constexpr unsigned int WINDOW_POS_X = 360;
    constexpr unsigned int WINDOW_POS_Y = 50;
    constexpr char GAME_NAME[] = "Candy Crush";
    Game game{ WINDOW_POS_X, WINDOW_POS_Y, WINDOW_WIDTH, WINDOW_HEIGHT, GAME_NAME };
    game.Play();

    return 0;
}