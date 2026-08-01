#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        SDL_Log("SDL_Init : %s", SDL_GetError());
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        SDL_Log("Mix_OpenAudio : %s", Mix_GetError());
        SDL_Quit();
        return 1;
    }

    Mix_Music *music = Mix_LoadMUS("test.mp3");

    if (!music)
    {
        SDL_Log("Mix_LoadMUS : %s", Mix_GetError());
        Mix_CloseAudio();
        SDL_Quit();
        return 1;
    }

    if (Mix_PlayMusic(music, 1) == -1)
    {
        SDL_Log("Mix_PlayMusic : %s", Mix_GetError());
        Mix_FreeMusic(music);
        Mix_CloseAudio();
        SDL_Quit();
        return 1;
    }

    while (Mix_PlayingMusic())
    {
        SDL_Delay(100);
    }

    Mix_FreeMusic(music);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}

