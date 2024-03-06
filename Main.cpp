#include "Game.h"

int main()
{
    constexpr unsigned int WINDOW_WIDTH  = 1024;
    constexpr unsigned int WINDOW_HEIGHT = 768;
    Game game(WINDOW_WIDTH, WINDOW_HEIGHT, "Bric-a-Brac");
    game.Play();

    return 0;
}