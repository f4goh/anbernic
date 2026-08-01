#include <SDL2/SDL.h>
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

    SDL_AudioSpec wavSpec;
    Uint8 *wavBuffer = NULL;
    Uint32 wavLength = 0;

    if (!SDL_LoadWAV("test.wav", &wavSpec, &wavBuffer, &wavLength))
    {
        SDL_Log("SDL_LoadWAV : %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_AudioDeviceID dev =
        SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);

    if (dev == 0)
    {
        SDL_Log("SDL_OpenAudioDevice : %s", SDL_GetError());
        SDL_FreeWAV(wavBuffer);
        SDL_Quit();
        return 1;
    }

    SDL_QueueAudio(dev, wavBuffer, wavLength);

    SDL_PauseAudioDevice(dev, 0);

    while (SDL_GetQueuedAudioSize(dev) > 0)
    {
        SDL_Delay(100);
    }

    SDL_CloseAudioDevice(dev);
    SDL_FreeWAV(wavBuffer);

    SDL_Quit();

    return 0;
}

