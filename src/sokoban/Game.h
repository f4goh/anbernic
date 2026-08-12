#ifndef GAME_H
#define GAME_H

#include "Input.h"
#include "Renderer.h"
#include "Level.h"
#include "Audio.h"
#include "Intro.h"
#include "SelLevel.h"

#define DELAY_SPLASH_BRAVO 2000

class Game
{
public:

    Game();
    ~Game();

    bool init();
    void run();
    void shutdown();


private:

    bool m_running;

    Input m_input;
    Renderer m_renderer;
    Audio m_audio;
    Intro m_intro;
    SelLevel m_selLevel;
    Level m_level;
    int m_currentLevel;
    bool m_levelCompleted;


    /*
     * Charge un niveau.
     */
    bool loadLevel(
        int levelIndex
    );


    /*
     * Mise à jour du jeu.
     */
    void update();


    /*
     * Tente un déplacement.
     */
    void tryMove(
        int dx,
        int dy
    );


    /*
     * Passe au niveau suivant.
     */
    void nextLevel();
};

#endif
