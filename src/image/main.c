#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define INPUT_DEVICE "/dev/input/event1"

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("SDL_Init : %s", SDL_GetError());
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        SDL_Log("IMG_Init : %s", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "PNG Viewer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0);

    if (!window)
    {
        SDL_Log("SDL_CreateWindow : %s", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer)
    {
        SDL_Log("SDL_CreateRenderer : %s", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Surface *surface = IMG_Load("image.png");

    if (!surface)
    {
        SDL_Log("IMG_Load : %s", IMG_GetError());

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();

        return 1;
    }

    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);

    if (!texture)
    {
        SDL_Log("SDL_CreateTextureFromSurface : %s", SDL_GetError());

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();

        return 1;
    }

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    /* Ouverture du contrôleur */

    int fd = open(INPUT_DEVICE, O_RDONLY);

    if (fd < 0)
    {
        perror("open");

        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        IMG_Quit();
        SDL_Quit();

        return 1;
    }

    struct input_event ev;

    /* Attente du bouton MENU */

    while (1)
    {
        if (read(fd, &ev, sizeof(ev)) != sizeof(ev))
            continue;

        if (ev.type == EV_KEY &&
            ev.code == 354 &&      /* MENU */
            ev.value == 1)         /* Press */
        {
            break;
        }
    }

    close(fd);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
