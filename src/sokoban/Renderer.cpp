#include "Renderer.h"

#include <cstdio>

Renderer::Renderer()
    : m_window(nullptr),
      m_renderer(nullptr),
      m_tileset(nullptr),
      m_playerTexture(nullptr),
      m_bravoTexture(nullptr),
      m_cameraX(0),
      m_cameraY(0)
{
}

Renderer::~Renderer()
{
    shutdown();
}

bool Renderer::init()
{
    if (SDL_Init(
        SDL_INIT_VIDEO |
        SDL_INIT_AUDIO |
        SDL_INIT_EVENTS) != 0)
    {
        std::printf(
            "SDL_Init error: %s\n",
            SDL_GetError()
        );

        return false;
    }

    if (!(IMG_Init(
        IMG_INIT_PNG) &
        IMG_INIT_PNG))
    {
        std::printf(
            "IMG_Init error: %s\n",
            IMG_GetError()
        );

        return false;
    }

    m_window =
        SDL_CreateWindow(
            "Sokoban",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            0
        );

    if (!m_window)
    {
        std::printf(
            "SDL_CreateWindow error: %s\n",
            SDL_GetError()
        );

        return false;
    }

    m_renderer =
        SDL_CreateRenderer(
            m_window,
            -1,
            SDL_RENDERER_ACCELERATED |
            SDL_RENDERER_PRESENTVSYNC
        );

    if (!m_renderer)
    {
        std::printf(
            "SDL_CreateRenderer error: %s\n",
            SDL_GetError()
        );

        return false;
    }

    if (!loadTextures())
        return false;

    return true;
}

bool Renderer::loadTextures()
{
    SDL_Surface* surface;

    surface =
        IMG_Load(
            "images/tileset.png"
        );

    if (!surface)
    {
        std::printf(
            "Erreur tileset.png : %s\n",
            IMG_GetError()
        );

        return false;
    }

    m_tileset =
        SDL_CreateTextureFromSurface(
            m_renderer,
            surface
        );

    SDL_FreeSurface(surface);

    if (!m_tileset)
        return false;

    surface =
        IMG_Load(
            "images/perso.png"
        );

    if (!surface)
    {
        std::printf(
            "Erreur perso.png : %s\n",
            IMG_GetError()
        );

        return false;
    }

    m_playerTexture =
        SDL_CreateTextureFromSurface(
            m_renderer,
            surface
        );

    SDL_FreeSurface(surface);

    if (!m_playerTexture)
        return false;


    /*
     * --------------------------------------------------------
     * IMAGE DE VICTOIRE
     * --------------------------------------------------------
     *
     * images/bravo.png
     *
     * Taille :
     * 640 x 82
     */

    surface =
        IMG_Load(
            "images/bravo.png"
        );

    if (!surface)
    {
        std::printf(
            "Erreur bravo.png : %s\n",
            IMG_GetError()
        );

        return false;
    }


    m_bravoTexture =
        SDL_CreateTextureFromSurface(
            m_renderer,
            surface
        );


    SDL_FreeSurface(surface);


    if (!m_bravoTexture)
    {
        std::printf(
            "Erreur texture bravo.png : %s\n",
            SDL_GetError()
        );

        return false;
    }


    return true;
}

void Renderer::shutdown()
{

    if (m_bravoTexture)
    {
        SDL_DestroyTexture(
            m_bravoTexture
        );

        m_bravoTexture = nullptr;
    }


    if (m_playerTexture)
    {
        SDL_DestroyTexture(
            m_playerTexture
        );

        m_playerTexture = nullptr;
    }

    if (m_tileset)
    {
        SDL_DestroyTexture(
            m_tileset
        );

        m_tileset = nullptr;
    }

    if (m_renderer)
    {
        SDL_DestroyRenderer(
            m_renderer
        );

        m_renderer = nullptr;
    }

    if (m_window)
    {
        SDL_DestroyWindow(
            m_window
        );

        m_window = nullptr;
    }

    IMG_Quit();
    SDL_Quit();
}

void Renderer::updateCamera(
    const Level& level)
{
    /*
        Pour commencer, on ne fait pas
        de scrolling complexe.

        On place simplement le niveau
        à son origine.

        Comme l'écran fait 640x480,
        cela permet de tester facilement
        les différentes origines.
    */

    m_cameraX = 0;
    m_cameraY = 0;

    /*
        Si le niveau dépasse l'écran,
        on pourra ajouter le scrolling
        plus tard.
    */
}

void Renderer::drawTile(
    uint8_t tile,
    int x,
    int y)
{
    if (tile > 24)
        return;

    /*
        Tileset 160x160.
        5 colonnes x 5 lignes.
    */

    int sourceX =
        (tile % 5) * TILE_SIZE;

    int sourceY =
        (tile / 5) * TILE_SIZE;

    SDL_Rect src;

    src.x = sourceX;
    src.y = sourceY;
    src.w = TILE_SIZE;
    src.h = TILE_SIZE;

    SDL_Rect dst;

    dst.x = x;
    dst.y = y;
    dst.w = TILE_SIZE;
    dst.h = TILE_SIZE;

    SDL_RenderCopy(
        m_renderer,
        m_tileset,
        &src,
        &dst
    );
}

void Renderer::drawPlayer(
    const Player& player,
    int x,
    int y)
{
    uint8_t tile =
        player.tile();

    /*
        perso.png :

        256 x 192

        8 colonnes
        6 lignes

        tile = 0..47
    */

    int sourceX =
        (tile % 8) * TILE_SIZE;

    int sourceY =
        (tile / 8) * TILE_SIZE;

    SDL_Rect src;

    src.x = sourceX;
    src.y = sourceY;
    src.w = TILE_SIZE;
    src.h = TILE_SIZE;

    SDL_Rect dst;

    dst.x = x;
    dst.y = y;
    dst.w = TILE_SIZE;
    dst.h = TILE_SIZE;

    SDL_RenderCopy(
        m_renderer,
        m_playerTexture,
        &src,
        &dst
    );
}

void Renderer::render(const Level& level,bool showBravo)
{
    updateCamera(level);

    SDL_SetRenderDrawColor(
        m_renderer,
        0,
        0,
        0,
        255
    );

    SDL_RenderClear(
        m_renderer
    );

    /*
        ==========================
        1. DECOR
        ==========================
    */

    for (int y = 0;
         y < level.height();
         ++y)
    {
        for (int x = 0;
             x < level.width();
             ++x)
        {
            uint8_t tile =
                level.tile(x, y);

            if (tile == 255)
                continue;

            int screenX =
                level.originX() +
                x * TILE_SIZE -
                m_cameraX;

            int screenY =
                level.originY() +
                y * TILE_SIZE -
                m_cameraY;

            drawTile(
                tile,
                screenX,
                screenY
            );
        }
    }

    /*
        ==========================
        2. CAISSES
        ==========================
    */

    for (const Crate& crate :
         level.crates())
    {
        int screenX =
            level.originX() +
            crate.x() * TILE_SIZE -
            m_cameraX;

        int screenY =
            level.originY() +
            crate.y() * TILE_SIZE -
            m_cameraY;

        drawTile(
            crate.tile(),
            screenX,
            screenY
        );
    }

    /*
        ==========================
        3. JOUEUR
        ==========================
    */

    int playerX =
        level.originX() +
        level.player().x() *
        TILE_SIZE -
        m_cameraX;

    int playerY =
        level.originY() +
        level.player().y() *
        TILE_SIZE -
        m_cameraY;

    drawPlayer(
        level.player(),
        playerX,
        playerY
    );

    /*
        ==========================
        4. SPLASH BRAVO
        ==========================
    */

    if (showBravo)
    {
        SDL_Rect dst;

        dst.x = 0;
        dst.y = (SCREEN_HEIGHT - 82) / 2;
        dst.w = 640;
        dst.h = 82;

        SDL_RenderCopy(
            m_renderer,
            m_bravoTexture,
            nullptr,
            &dst
        );
    }

    SDL_RenderPresent(
        m_renderer
    );
}

SDL_Renderer* Renderer::getSDLRenderer() const
{
    return m_renderer;
}



void Renderer::renderBravo()
{
    /*
     * --------------------------------------------------------
     * BRAVO
     * --------------------------------------------------------
     *
     * IMPORTANT :
     *
     * On ne fait PAS de SDL_RenderClear().
     *
     * Le niveau vient juste d'être dessiné
     * par render().
     *
     * On ajoute simplement bravo.png
     * par-dessus le niveau.
     */


    SDL_Rect dst;

    /*
     * bravo.png :
     *
     * 640 x 82
     *
     * Écran :
     *
     * 640 x 480
     *
     * Centrage vertical.
     */

    dst.x = 0;

    dst.y =
        (SCREEN_HEIGHT - 82) / 2;

    dst.w = 640;
    dst.h = 82;


    /*
     * Dessine BRAVO par-dessus
     * ce qui est déjà présent à l'écran.
     */

    SDL_RenderCopy(
        m_renderer,
        m_bravoTexture,
        nullptr,
        &dst
    );


    /*
     * Affiche le niveau + BRAVO.
     */

    SDL_RenderPresent(
        m_renderer
    );
}



