
#include "SelLevel.h"

#include <iostream>
#include <cstdio>
#include <fstream>
#include <cstring>

#ifdef __aarch64__
#include <unistd.h>
#endif


/*
 * ==========================================================
 * CONSTRUCTEUR
 * ==========================================================
 */

SelLevel::SelLevel()
    : m_renderer(nullptr),
      m_font(nullptr),
      m_initialized(false),
      m_selectedLevel(0),
      m_saveFile("levels_save.txt")
{
    /*
     * Par défaut, aucun niveau n'est terminé.
     */
    for (int i = 0;
         i < SELECTABLE_LEVEL_COUNT;
         ++i)
    {
        m_completedLevels[i] = false;
    }
}


/*
 * ==========================================================
 * DESTRUCTEUR
 * ==========================================================
 */

SelLevel::~SelLevel()
{
    shutdown();
}


/*
 * ==========================================================
 * INITIALISATION
 * ==========================================================
 */

bool SelLevel::init(
    SDL_Renderer* renderer)
{
    m_renderer = renderer;

    if (m_renderer == nullptr)
    {
        std::cerr
            << "SelLevel : renderer invalide"
            << std::endl;

        return false;
    }


    /*
     * ------------------------------------------------------
     * SDL_ttf
     * ------------------------------------------------------
     */

    if (TTF_Init() == -1)
    {
        std::cerr
            << "SelLevel : TTF_Init impossible : "
            << TTF_GetError()
            << std::endl;

        return false;
    }


    /*
     * ------------------------------------------------------
     * POLICE
     * ------------------------------------------------------
     */

    if (!loadFont())
    {
        std::cerr
            << "SelLevel : impossible de charger la police"
            << std::endl;

        TTF_Quit();

        return false;
    }


    /*
     * ------------------------------------------------------
     * SAUVEGARDE DES NIVEAUX
     * ------------------------------------------------------
     */

    /*
     * Si le fichier n'existe pas,
     * on le crée automatiquement.
     */
    std::ifstream file(m_saveFile);

    if (!file.good())
    {
        file.close();

        if (!createSaveFile())
        {
            std::cerr
                << "SelLevel : impossible de créer "
                << "le fichier de sauvegarde : "
                << m_saveFile
                << std::endl;

            TTF_CloseFont(m_font);
            m_font = nullptr;

            TTF_Quit();

            return false;
        }
    }
    else
    {
        file.close();
    }


    /*
     * Charge les niveaux déjà terminés.
     */
    if (!loadCompletedLevels())
    {
        std::cerr
            << "SelLevel : impossible de charger "
            << "la sauvegarde des niveaux"
            << std::endl;

        TTF_CloseFont(m_font);
        m_font = nullptr;

        TTF_Quit();

        return false;
    }


    m_selectedLevel = 0;

    m_initialized = true;

    return true;
}


/*
 * ==========================================================
 * CHARGEMENT DE LA POLICE
 * ==========================================================
 */

bool SelLevel::loadFont()
{
#ifdef __aarch64__

    /*
     * ------------------------------------------------------
     * ANBERNIC
     * ------------------------------------------------------
     */

    const char* fontPath =
        "/usr/share/fonts/liberation/LiberationSans-Regular.ttf";

#else

    /*
     * Police fournie avec le projet pour Linux PC.
     */

    const char* fontPath =
        "font/DejaVuSans.ttf";

#endif


    m_font =
        TTF_OpenFont(
            fontPath,
            22
        );


    if (m_font == nullptr)
    {
        std::cerr
            << "SelLevel : impossible de charger : "
            << fontPath
            << std::endl;

        std::cerr
            << "SDL_ttf : "
            << TTF_GetError()
            << std::endl;

        return false;
    }


    return true;
}


/*
 * ==========================================================
 * CREATION DU FICHIER DE SAUVEGARDE
 * ==========================================================
 */

bool SelLevel::createSaveFile()
{
    std::ofstream file(
        m_saveFile,
        std::ios::out |
        std::ios::trunc
    );

    if (!file.is_open())
    {
        return false;
    }


    /*
     * 50 niveaux non terminés.
     *
     * Exemple :
     *
     * 00000000000000000000000000000000000000000000000000
     */

    for (int i = 0;
         i < SELECTABLE_LEVEL_COUNT;
         ++i)
    {
        file << '0';
    }

    file << '\n';

    file.close();

    return true;
}


/*
 * ==========================================================
 * CHARGEMENT DES NIVEAUX TERMINES
 * ==========================================================
 */

bool SelLevel::loadCompletedLevels()
{
    /*
     * Par sécurité :
     * tous les niveaux sont d'abord considérés
     * comme non terminés.
     */

    for (int i = 0;
         i < SELECTABLE_LEVEL_COUNT;
         ++i)
    {
        m_completedLevels[i] = false;
    }


    std::ifstream file(m_saveFile);

    if (!file.is_open())
    {
        return false;
    }


    std::string data;

    /*
     * Lecture du premier élément du fichier.
     */
    file >> data;

    file.close();


    /*
     * Le fichier doit contenir au minimum
     * 50 caractères.
     */
    if (data.length() < SELECTABLE_LEVEL_COUNT)
    {
        std::cerr
            << "SelLevel : fichier de sauvegarde "
            << "invalide ou incomplet"
            << std::endl;

        return false;
    }


    /*
     * Lecture des 50 niveaux.
     */
    for (int i = 0;
         i < SELECTABLE_LEVEL_COUNT;
         ++i)
    {
        if (data[i] == '1')
        {
            m_completedLevels[i] = true;
        }
        else
        {
            m_completedLevels[i] = false;
        }
    }


    return true;
}


/*
 * ==========================================================
 * SAUVEGARDE DES NIVEAUX TERMINES
 * ==========================================================
 */

bool SelLevel::saveCompletedLevels()
{
    std::ofstream file(
        m_saveFile,
        std::ios::out |
        std::ios::trunc
    );

    if (!file.is_open())
    {
        std::cerr
            << "SelLevel : impossible d'ouvrir "
            << "le fichier de sauvegarde"
            << std::endl;

        return false;
    }


    /*
     * Écrit les 50 niveaux.
     */
    for (int i = 0;
         i < SELECTABLE_LEVEL_COUNT;
         ++i)
    {
        if (m_completedLevels[i])
        {
            file << '1';
        }
        else
        {
            file << '0';
        }
    }

    file << '\n';

    file.close();

    return true;
}


/*
 * ==========================================================
 * MARQUER UN NIVEAU COMME TERMINE
 * ==========================================================
 */

bool SelLevel::setLevelCompleted(
    int levelIndex)
{
    /*
     * Vérification de l'index.
     */
    if (levelIndex < 0 ||
        levelIndex >= SELECTABLE_LEVEL_COUNT)
    {
        std::cerr
            << "SelLevel : index de niveau invalide : "
            << levelIndex
            << std::endl;

        return false;
    }


    /*
     * Déjà terminé ?
     *
     * On peut directement retourner true.
     */
    if (m_completedLevels[levelIndex])
    {
        return true;
    }


    /*
     * Marque le niveau comme terminé.
     */
    m_completedLevels[levelIndex] = true;


    /*
     * Sauvegarde immédiatement.
     */
    if (!saveCompletedLevels())
    {
        /*
         * Si la sauvegarde échoue,
         * on remet l'état précédent.
         */
        m_completedLevels[levelIndex] = false;

        return false;
    }


    return true;
}


/*
 * ==========================================================
 * SAVOIR SI UN NIVEAU EST TERMINE
 * ==========================================================
 */

bool SelLevel::isLevelCompleted(
    int levelIndex) const
{
    if (levelIndex < 0 ||
        levelIndex >= SELECTABLE_LEVEL_COUNT)
    {
        return false;
    }


    return m_completedLevels[levelIndex];
}


/*
 * ==========================================================
 * ARRET
 * ==========================================================
 */

void SelLevel::shutdown()
{
    if (m_font != nullptr)
    {
        TTF_CloseFont(m_font);

        m_font = nullptr;
    }


    /*
     * TTF_Quit correspond au TTF_Init()
     * effectué dans init().
     */

    if (m_initialized)
    {
        TTF_Quit();
    }


    m_renderer = nullptr;

    m_initialized = false;
}


/*
 * ==========================================================
 * AFFICHAGE D'UN TEXTE
 * ==========================================================
 */

void SelLevel::renderText(
    const char* text,
    int x,
    int y,
    SDL_Color color)
{
    if (m_renderer == nullptr ||
        m_font == nullptr)
    {
        return;
    }


    SDL_Surface* surface =
        TTF_RenderUTF8_Blended(
            m_font,
            text,
            color
        );


    if (surface == nullptr)
    {
        return;
    }


    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(
            m_renderer,
            surface
        );


    if (texture == nullptr)
    {
        SDL_FreeSurface(surface);

        return;
    }


    SDL_Rect destination;

    destination.x = x;
    destination.y = y;
    destination.w = surface->w;
    destination.h = surface->h;


    SDL_FreeSurface(surface);


    SDL_RenderCopy(
        m_renderer,
        texture,
        nullptr,
        &destination
    );


    SDL_DestroyTexture(texture);
}


/*
 * ==========================================================
 * AFFICHAGE
 * ==========================================================
 */

void SelLevel::render()
{
    if (m_renderer == nullptr)
    {
        return;
    }


    /*
     * Fond noir.
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
     * ------------------------------------------------------
     * COULEURS
     * ------------------------------------------------------
     */

    SDL_Color white =
    {
        255,
        255,
        255,
        255
    };


    SDL_Color green =
    {
        0,
        255,
        0,
        255
    };


    SDL_Color yellow =
    {
        255,
        220,
        0,
        255
    };


    /*
     * ------------------------------------------------------
     * TITRE
     * ------------------------------------------------------
     */

    renderText(
        "LEVEL SELECT",
        20,
        15,
        white
    );


    /*
     * ------------------------------------------------------
     * 50 NIVEAUX
     * ------------------------------------------------------
     *
     * 5 colonnes
     * 10 lignes
     *
     * LEVEL01 ... LEVEL50
     */

    const int columns = 5;
    const int rows = 10;

    const int columnWidth = 125;
    const int rowHeight = 36;

    const int startX = 20;
    const int startY = 70;


    /*
     * Évite un warning compilateur
     * concernant columns qui n'était pas utilisé.
     */
    (void)columns;


    for (int i = 0;
         i < SELECTABLE_LEVEL_COUNT;
         ++i)
    {
        /*
         * Organisation colonne par colonne :
         *
         * colonne 0 : 01 -> 10
         * colonne 1 : 11 -> 20
         * etc.
         */

        const int column =
            i / rows;


        const int row =
            i % rows;


        const int x =
            startX +
            column * columnWidth;


        const int y =
            startY +
            row * rowHeight;


        /*
         * Numéro du niveau.
         */

        char levelName[32];


        std::snprintf(
            levelName,
            sizeof(levelName),
            "LEVEL%02d",
            i + 1
        );


        /*
         * --------------------------------------------------
         * COULEUR DU NIVEAU
         * --------------------------------------------------
         *
         * Blanc :
         *     niveau non terminé
         *
         * Vert :
         *     niveau terminé
         *
         * Jaune :
         *     niveau actuellement sélectionné
         */

        SDL_Color levelColor;


        if (i == m_selectedLevel)
        {
            levelColor = yellow;
        }
        else if (m_completedLevels[i])
        {
            levelColor = green;
        }
        else
        {
            levelColor = white;
        }


        /*
         * Curseur.
         */

        if (i == m_selectedLevel)
        {
            renderText(
                ">",
                x,
                y,
                yellow
            );
        }


        renderText(
            levelName,
            x + 20,
            y,
            levelColor
        );
    }


    /*
     * ------------------------------------------------------
     * INFORMATION
     * ------------------------------------------------------
     */

    renderText(
        "ARROWS : SELECT     ENTER / B : PLAY",
        20,
        440,
        white
    );


    SDL_RenderPresent(
        m_renderer
    );
}


/*
 * ==========================================================
 * RETURN PC
 * ==========================================================
 */

bool SelLevel::enterPressed()
{
#ifndef __aarch64__

    const Uint8* keyboard =
        SDL_GetKeyboardState(nullptr);


    return keyboard[
        SDL_SCANCODE_RETURN
    ];

#else

    return false;

#endif
}


/*
 * ==========================================================
 * BOUCLE DE SELECTION
 * ==========================================================
 */

bool SelLevel::run(
    Input& input,
    int& levelIndex)
{
    if (!m_initialized)
    {
        return false;
    }


    /*
     * Position initiale du curseur.
     *
     * Lors du premier lancement :
     * levelIndex = 0
     *
     * Lors d'un retour depuis le jeu :
     * levelIndex = niveau actuel.
     */

    m_selectedLevel =
        levelIndex;


    /*
     * ------------------------------------------------------
     * BOUCLE DE SELECTION
     * ------------------------------------------------------
     */

    while (true)
    {
        /*
         * --------------------------------------------------
         * INPUT
         * --------------------------------------------------
         *
         * IMPORTANT :
         *
         * Input::update() est le seul endroit
         * qui lit SDL_PollEvent().
         */

        input.update();


        /*
         * --------------------------------------------------
         * QUITTER
         * --------------------------------------------------
         */

        if (input.quitRequested())
        {
            return false;
        }


        /*
         * --------------------------------------------------
         * DEPLACEMENT
         * --------------------------------------------------
         */

        if (input.movePressed())
        {
            const int dx =
                input.moveX();


            const int dy =
                input.moveY();


            /*
             * Haut
             */

            if (dy < 0)
            {
                m_selectedLevel -= 1;
            }


            /*
             * Bas
             */

            else if (dy > 0)
            {
                m_selectedLevel += 1;
            }


            /*
             * Gauche
             */

            else if (dx < 0)
            {
                m_selectedLevel -= 10;
            }


            /*
             * Droite
             */

            else if (dx > 0)
            {
                m_selectedLevel += 10;
            }


            /*
             * ------------------------------------------------
             * BOUCLAGE
             * ------------------------------------------------
             */

            if (m_selectedLevel < 0)
            {
                m_selectedLevel =
                    SELECTABLE_LEVEL_COUNT - 1;
            }


            if (m_selectedLevel >=
                SELECTABLE_LEVEL_COUNT)
            {
                m_selectedLevel = 0;
            }
        }


        /*
         * --------------------------------------------------
         * VALIDATION
         * --------------------------------------------------
         *
         * PC :
         *     RETURN
         *
         * Anbernic :
         *     B
         */

        if (input.confirmPressed())
        {
            levelIndex =
                m_selectedLevel;

            return true;
        }


        if (input.buttonB())
        {
            levelIndex =
                m_selectedLevel;

            return true;
        }


        /*
         * --------------------------------------------------
         * AFFICHAGE
         * --------------------------------------------------
         */

        render();


        SDL_Delay(8);
    }
}


