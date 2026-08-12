//https://gradients.app/fr/new

#include "Game.h"

#include <cstdio>

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    Game game;

    if (!game.init())
    {
        std::printf(
            "Erreur initialisation du jeu.\n"
        );

        return 1;
    }

    game.run();

    game.shutdown();

    return 0;
}
