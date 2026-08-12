
#ifndef SELLEVEL_H
#define SELLEVEL_H

#include "Input.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SELECTABLE_LEVEL_COUNT 50

class SelLevel
{
public:

    SelLevel();
    ~SelLevel();

    bool init(
        SDL_Renderer* renderer
    );

    void shutdown();

    /*
     * Affiche l'écran de sélection
     * et attend que le joueur choisisse
     * un niveau.
     *
     * PC :
     *     flèches
     *     RETURN
     *
     * Anbernic :
     *     D-PAD
     *     bouton B
     *
     * Retourne false si le joueur quitte.
     *
     * Le niveau sélectionné est retourné
     * dans levelIndex.
     */
    bool run(
        Input& input,
        int& levelIndex
    );

    /*
     * Marque un niveau comme terminé.
     *
     * levelIndex :
     *     0 = LEVEL01
     *     1 = LEVEL02
     *     ...
     *     49 = LEVEL50
     *
     * Sauvegarde immédiatement dans le fichier.
     */
    bool setLevelCompleted(
        int levelIndex
    );

    /*
     * Permet de savoir si un niveau
     * a déjà été terminé.
     */
    bool isLevelCompleted(
        int levelIndex
    ) const;

private:

    SDL_Renderer* m_renderer;

    TTF_Font* m_font;

    bool m_initialized;

    int m_selectedLevel;

    /*
     * Tableau contenant l'état des 50 niveaux.
     *
     * false = non terminé
     * true  = terminé
     */
    bool m_completedLevels[SELECTABLE_LEVEL_COUNT];

    /*
     * Fichier de sauvegarde.
     */
    const char* m_saveFile;

    /*
     * Affichage.
     */
    void render();

    /*
     * Dessine un texte.
     */
    void renderText(
        const char* text,
        int x,
        int y,
        SDL_Color color
    );

    /*
     * Chemin de la police.
     */
    bool loadFont();

    /*
     * Détection du choix.
     */
    bool enterPressed();

    /*
     * Charge les niveaux terminés
     * depuis le fichier.
     */
    bool loadCompletedLevels();

    /*
     * Sauvegarde les niveaux terminés
     * dans le fichier.
     */
    bool saveCompletedLevels();

    /*
     * Crée le fichier de sauvegarde
     * avec tous les niveaux à 0.
     */
    bool createSaveFile();
};

#endif


