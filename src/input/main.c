#include <SDL2/SDL.h>
#include <stdbool.h>

#include "input.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Erreur SDL_Init : %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Carre Rouge",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        SDL_Log("Erreur creation fenetre : %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL)
    {
        SDL_Log("Erreur creation renderer : %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (input_init())
        SDL_Log("Module input initialise.");
    else
        SDL_Log("Module input non disponible.");

    bool running = true;
    SDL_Event event;

    SDL_Rect square =
    {
        WINDOW_WIDTH / 2 - 50,
        WINDOW_HEIGHT / 2 - 50,
        100,
        100
    };

    while (running)
    {
        /*
         * Evenements SDL (PC)
         */
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        running = false;
                    break;
            }
        }

        /*
         * Mise a jour des controles RG40XX H
         */
        input_update();

        /*
         * Quitter avec le bouton MENU
         */
        if (input_down(INPUT_MENU))
            running = false;

        /*
         * Déplacer le carré avec le stick gauche
         */
        square.x += input_left_x() / 1024;
        square.y += input_left_y() / 1024;

        /*
         * Déplacement avec la croix directionnelle
         */
        if (input_dpad_x() == -1)
            square.x -= 3;

        if (input_dpad_x() == 1)
            square.x += 3;

        if (input_dpad_y() == -1)
            square.y -= 3;

        if (input_dpad_y() == 1)
            square.y += 3;

        /*
         * Empêcher le carré de sortir de l'écran
         */
        if (square.x < 0)
            square.x = 0;

        if (square.y < 0)
            square.y = 0;

        if (square.x > WINDOW_WIDTH - square.w)
            square.x = WINDOW_WIDTH - square.w;

        if (square.y > WINDOW_HEIGHT - square.h)
            square.y = WINDOW_HEIGHT - square.h;

        /*
         * Affichage
         */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &square);

        SDL_RenderPresent(renderer);
    }

    input_close();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
