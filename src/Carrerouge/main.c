#include <SDL2/SDL.h>
#include <stdbool.h>

#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

/* Retourne true si le bouton demandé vient d'être appuyé */
static bool button_pressed(int js, int button)
{
    if (js < 0)
        return false;

    struct js_event e;

    while (read(js, &e, sizeof(e)) == sizeof(e))
    {
        /* Ignore le flag d'initialisation */
        e.type &= ~JS_EVENT_INIT;

        if (e.type == JS_EVENT_BUTTON &&
            e.number == button &&
            e.value == 1)
        {
            return true;
        }
    }

    return false;
}

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

    /* Ouvre le joystick Linux si disponible */
    int js = open("/dev/input/js0", O_RDONLY | O_NONBLOCK);

    if (js >= 0)
    {
        SDL_Log("Joystick Linux ouvert.");
    }
    else
    {
        SDL_Log("Aucun joystick Linux detecte.");
    }

    bool running = true;
    SDL_Event event;

    SDL_Rect square = {
        WINDOW_WIDTH / 2 - 50,
        WINDOW_HEIGHT / 2 - 50,
        100,
        100};

    while (running)
    {
        /* Gestion des événements SDL (PC) */
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        running = false;
                    }
                    break;
            }
        }

        /* Gestion du bouton 3 sur le RG40XX H */
        if (button_pressed(js, 3))
        {
            SDL_Log("Bouton 3 appuye.");
            running = false;
        }

        /* Fond noir */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        /* Carré rouge */
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &square);

        SDL_RenderPresent(renderer);
    }

    if (js >= 0)
        close(js);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
