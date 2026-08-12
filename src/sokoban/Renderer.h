#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Level.h"

class Renderer
{
public:

    Renderer();
    ~Renderer();

    bool init();
    void shutdown();

    void render(const Level& level, bool showBravo = false);

    /* * Affiche l'écran "BRAVO" * après la réussite d'un niveau. */ 

    void renderBravo();

    /*
     * Permet à Intro d'utiliser
     * le SDL_Renderer créé ici.
     */
    SDL_Renderer* getSDLRenderer() const;


private:

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;

    SDL_Texture* m_tileset;
    SDL_Texture* m_playerTexture;

    /* * Texture de l'écran de victoire. * * images/bravo.png * 640 x 82 */ 
    SDL_Texture* m_bravoTexture;


    int m_cameraX;
    int m_cameraY;

    static constexpr int SCREEN_WIDTH = 640;
    static constexpr int SCREEN_HEIGHT = 480;
    static constexpr int TILE_SIZE = 32;

    bool loadTextures();

    void updateCamera(
        const Level& level
    );

    void drawTile(
        uint8_t tile,
        int x,
        int y
    );

    void drawPlayer(
        const Player& player,
        int x,
        int y
    );
};

#endif
