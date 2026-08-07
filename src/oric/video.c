#include "video.h"

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define ORIC_WIDTH   320
#define ORIC_HEIGHT  224

#define WIN_WIDTH    640
#define WIN_HEIGHT   480

static SDL_Window   *window;
static SDL_Renderer *renderer;
static SDL_Texture  *texture;

/* Buffer Oric */
//static uint8_t framebuffer[ORIC_WIDTH * ORIC_HEIGHT];
static uint32_t framebuffer[ORIC_WIDTH * ORIC_HEIGHT];

/* Buffer 32 bits envoyé au GPU */
//static uint32_t rgbbuffer[ORIC_WIDTH * ORIC_HEIGHT];

static Ula *video_ula;

/*
static const uint32_t palette[8] =
{
    0xFF000000,
    0xFFFF0000,
    0xFF00FF00,
    0xFFFFFF00,
    0xFF0000FF,
    0xFFFF00FF,
    0xFF00FFFF,
    0xFFFFFFFF
};
*/

bool video_init(Ula *u)
{
    video_ula = u;

    if (SDL_Init(SDL_INIT_VIDEO |
                 SDL_INIT_TIMER |
                 SDL_INIT_AUDIO) != 0)
        return false;

    SDL_Log("Nombre de drivers audio : %d",
            SDL_GetNumAudioDrivers());

    SDL_Log("Driver audio courant : %s",
            SDL_GetCurrentAudioDriver());

    printf("SDL: create window\n");

    window = SDL_CreateWindow(
        "Oric SDL2",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIN_WIDTH,
        WIN_HEIGHT,
        SDL_WINDOW_SHOWN);

    printf("window=%p error=%s\n",
           window,
           SDL_GetError());

    if (!window)
        return false;

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED);

    printf("renderer=%p error=%s\n",
           renderer,
           SDL_GetError());

    if (!renderer)
        return false;

    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer, &info);

    printf("Renderer : %s\n", info.name);
    printf("Flags    : %u\n", info.flags);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        ORIC_WIDTH,
        ORIC_HEIGHT);

    if (!texture)
        return false;

    memset(framebuffer, 0, sizeof(framebuffer));
    //memset(rgbbuffer, 0, sizeof(rgbbuffer));

    return true;
}

void video_display(void)
{
    // Génération image Oric
    for (int y = 0; y < ORIC_HEIGHT; y++)
    {
        ula_scanline(
            video_ula,
            y,
            framebuffer + y * ORIC_WIDTH);
    }

    ula_endframe(video_ula);

   // Conversion palette -> ARGB 
/*
    const uint8_t *src = framebuffer;
    uint32_t *dst = rgbbuffer;

    for (int i = 0; i < ORIC_WIDTH * ORIC_HEIGHT; i++)
        *dst++ = palette[*src++ & 7];
*/

SDL_UpdateTexture(
    texture,
    NULL,
    framebuffer,
    ORIC_WIDTH*sizeof(uint32_t));
/*
    SDL_UpdateTexture(
        texture,
        NULL,
        rgbbuffer,
        ORIC_WIDTH * sizeof(uint32_t));
*/
    SDL_Rect dstRect =
    {
        0,
        16,         // marge haute comme avant
        640,
        448
    };

    SDL_RenderClear(renderer);

    SDL_RenderCopy(
        renderer,
        texture,
        NULL,
        &dstRect);

    SDL_RenderPresent(renderer);
}



/*
void video_display(void)
{
    // Génération image Oric
    for (int y = 0; y < ORIC_HEIGHT; y++)
    {
        ula_scanline(
            video_ula,
            y,
            framebuffer + y * ORIC_WIDTH);
    }

    ula_endframe(video_ula);

   // Conversion palette -> ARGB 
    const uint8_t *src = framebuffer;
    uint32_t *dst = rgbbuffer;

    for (int i = 0; i < ORIC_WIDTH * ORIC_HEIGHT; i++)
        *dst++ = palette[*src++ & 7];

    SDL_UpdateTexture(
        texture,
        NULL,
        rgbbuffer,
        ORIC_WIDTH * sizeof(uint32_t));

    SDL_Rect dstRect =
    {
        0,
        16,         // marge haute comme avant
        640,
        448
    };

    SDL_RenderClear(renderer);

    SDL_RenderCopy(
        renderer,
        texture,
        NULL,
        &dstRect);

    SDL_RenderPresent(renderer);
}
*/


void video_shutdown(void)
{
    if (texture)
        SDL_DestroyTexture(texture);

    if (renderer)
        SDL_DestroyRenderer(renderer);

    if (window)
        SDL_DestroyWindow(window);

    SDL_Quit();
}
