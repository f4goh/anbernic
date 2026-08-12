#include "Intro.h"

#include <iostream>


Intro::Intro()
    : m_renderer(nullptr),
      m_background(nullptr),
      m_initialized(false)
{
}


Intro::~Intro()
{
    shutdown();
}


/*
 * ============================================================
 * INITIALISATION
 * ============================================================
 */

bool Intro::init(
    SDL_Renderer* renderer)
{
    m_renderer = renderer;

    if (m_renderer == nullptr)
    {
        std::cerr
            << "Intro : renderer invalide"
            << std::endl;

        return false;
    }


    /*
     * --------------------------------------------------------
     * IMAGE D'INTRODUCTION
     * --------------------------------------------------------
     *
     *
     * Exemple :
     *
     *     m_background = IMG_LoadTexture(
     *         m_renderer,
     *         "images/intro.png"
     *     );
     *
     * L'image devra idéalement être en 640x480.
     *
     * --------------------------------------------------------
     */

    
    m_background = IMG_LoadTexture(
        m_renderer,
        "images/intro.png"
    );

    if (m_background == nullptr)
    {
        std::cerr
            << "Intro : impossible de charger images/intro.png"
            << std::endl;

        return false;
    }
    


    m_initialized = true;

    return true;
}


/*
 * ============================================================
 * ARRET
 * ============================================================
 */

void Intro::shutdown()
{
    if (m_background != nullptr)
    {
        SDL_DestroyTexture(
            m_background
        );

        m_background = nullptr;
    }


    m_renderer = nullptr;

    m_initialized = false;
}


/*
 * ============================================================
 * AFFICHAGE
 * ============================================================
 */

void Intro::render()
{
    if (m_renderer == nullptr)
    {
        return;
    }


    /*
     * Fond noir pour le moment.
     *
     * Lorsque l'image sera ajoutée,
     * elle sera affichée ici.
     */

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
     * --------------------------------------------------------
     * IMAGE D'INTRODUCTION
     * --------------------------------------------------------
     *
     * Préparé pour une image 640x480.
     */

    
    if (m_background != nullptr)
    {
        SDL_Rect destination;

        destination.x = 0;
        destination.y = 0;
        destination.w = 640;
        destination.h = 480;

        SDL_RenderCopy(
            m_renderer,
            m_background,
            nullptr,
            &destination
        );
    }
    


    SDL_RenderPresent(
        m_renderer
    );
}


/*
 * ============================================================
 * ESPACE PC
 * ============================================================
 */

bool Intro::spacePressed()
{
    const Uint8* keyboard =
        SDL_GetKeyboardState(nullptr);

    return keyboard[SDL_SCANCODE_SPACE];
}



/*
 * ============================================================
 * BOUCLE INTRO
 * ============================================================
 */

bool Intro::run(
    Input& input)
{
    if (!m_initialized)
    {
        return false;
    }


    bool previousSpace = false;
    bool previousA = false;


    while (true)
    {
        SDL_Event event;


        /*
         * Gestion des événements SDL.
         */

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                return false;
            }


            /*
             * ESC permet également de quitter
             * l'introduction sur PC.
             */

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    return false;
                }
            }
        }


        /*
         * Mise à jour de l'Input.
         *
         * Cela permet notamment de lire
         * le contrôleur Anbernic.
         */

        input.update();


        /*
         * Bouton A.
         *
         * Sera activé avec Input::buttonA()
         */

        const bool a = input.buttonA();



        /*
         * ESPACE PC.
         */

        const bool space =
            spacePressed();


        /*
         * Détection d'une nouvelle pression.
         */

        if (space && !previousSpace)
        {
            return true;
        }


        if (a && !previousA)
        {
            return true;
        }


        previousSpace = space;
        previousA = a;


        /*
         * Affichage.
         */

        render();


        SDL_Delay(8);
    }
}
