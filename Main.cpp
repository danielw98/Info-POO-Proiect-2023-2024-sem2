#include "Game.h"

int main()
{
    constexpr unsigned int WINDOW_WIDTH  = 1600;  // 1024;
    constexpr unsigned int WINDOW_HEIGHT = 1200;  // 768;

    Game game(WINDOW_WIDTH, WINDOW_HEIGHT, "Game");
    game.Play();

    return 0;
}