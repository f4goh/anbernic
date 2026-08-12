#ifndef INTRO_H
#define INTRO_H

#include "Input.h"
#include "Audio.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Intro
{
public:

    Intro();
    ~Intro();

    bool init(
        SDL_Renderer* renderer
    );

    void shutdown();

    /*
     * Affiche l'écran d'introduction et attend
     * la validation du joueur.
     *
     * PC :
     *     ESPACE
     *
     * Anbernic :
     *     bouton A
     *
     * Retourne true lorsque le joueur veut
     * démarrer le jeu.
     */
    bool run(
        Input& input
    );


private:

    SDL_Renderer* m_renderer;

    SDL_Texture* m_background;

    bool m_initialized;


    bool loadBackground();

    void render();

    bool spacePressed();

};

#endif
