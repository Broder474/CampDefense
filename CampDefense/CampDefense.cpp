#include "Game.h"

Game* game = new Game();

int main(int argc, char** argv)
{
    game->loop();
    return 0;
}