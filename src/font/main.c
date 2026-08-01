#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define INPUT_DEVICE "/dev/input/event1"

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("%s", SDL_GetError());
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        SDL_Log("%s", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    if (TTF_Init() == -1)
    {
        SDL_Log("%s", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "PNG",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0);

    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Surface *img = IMG_Load("image.png");
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, img);
    SDL_FreeSurface(img);

    /* Chargement de la police */

    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/liberation/LiberationSans-Regular.ttf", 48);

    if (!font)
    {
        SDL_Log("%s", TTF_GetError());
        return 1;
    }

    /* Style gras */
    TTF_SetFontStyle(font, TTF_STYLE_BOLD);

    /* Couleur rouge */
    SDL_Color red = {255, 0, 0, 255};

    SDL_Surface *txtSurface =
        TTF_RenderUTF8_Blended(font, "Test", red);

    SDL_Texture *txtTexture =
        SDL_CreateTextureFromSurface(renderer, txtSurface);



    SDL_Rect txtRect;

    txtRect.x = 20;
    txtRect.y = 20;
    txtRect.w = txtSurface->w;
    txtRect.h = txtSurface->h;

    SDL_FreeSurface(txtSurface);

    /* Dessin */

    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, tex, NULL, NULL);

    SDL_RenderCopy(renderer, txtTexture, NULL, &txtRect);

    SDL_RenderPresent(renderer);

    /* Attente du bouton MENU */

    int fd = open(INPUT_DEVICE, O_RDONLY);

    struct input_event ev;

    while (1)
    {
        if (read(fd, &ev, sizeof(ev)) != sizeof(ev))
            continue;

        if (ev.type == EV_KEY &&
            ev.code == 354 &&
            ev.value == 1)
            break;
    }

    close(fd);

    SDL_DestroyTexture(txtTexture);
    SDL_DestroyTexture(tex);

    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
