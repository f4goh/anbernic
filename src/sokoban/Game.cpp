#include "Game.h"
#include "Levels.h"

#include <iostream>
#include <SDL2/SDL.h>


Game::Game()
    : m_running(false),
      m_input(),
      m_renderer(),
      m_audio(),
      m_intro(),
      m_selLevel(),
      m_level(),
      m_currentLevel(0), 
      m_levelCompleted(false)
{
}


Game::~Game()
{
    shutdown();
}


/*
 * ============================================================
 * INITIALISATION
 * ============================================================
 */

bool Game::init()
{
    /*
     * Le Renderer initialise lui-même SDL,
     * la fenêtre 640x480 et le renderer SDL.
     */

    if (!m_renderer.init())
    {
        std::cerr
            << "ERROR: Renderer init failed"
            << std::endl;

        return false;
    }


    /*
     * Initialisation des entrées.
     *
     * La classe Input s'occupe de différencier
     * le PC et l'Anbernic.
     */

    if (!m_input.init())
    {
        std::cerr
            << "WARNING: Input init failed"
            << std::endl;
    }


    /*
     * Initialisation audio.
     *
     * Si l'audio n'est pas disponible,
     * le jeu continue sans son.
     */

    if (!m_audio.init())
    {
        std::cerr
            << "WARNING: Audio init failed"
            << std::endl;
    }
    else
    {
        /*
         * Lecture de :
         *
         * music/sokoban.mod
         */

        m_audio.playMusic();
    }

    /*
     * --------------------------------------------------------
     * INTRODUCTION
     * --------------------------------------------------------
     *
     * L'Intro utilise le renderer SDL
     * créé par Renderer.
     */

    if (!m_intro.init(
            m_renderer.getSDLRenderer()))
    {
        std::cerr
            << "ERROR: Intro init failed"
            << std::endl;

        shutdown();

        return false;
    }


    /*
     * --------------------------------------------------------
     * ATTENTE DE L'UTILISATEUR
     * --------------------------------------------------------
     *
     * PC :
     *     ESPACE
     *
     * Anbernic :
     *     bouton A
     *
     * Si false est retourné :
     *     l'utilisateur a demandé à quitter.
     */

    if (!m_intro.run(m_input))
    {
        shutdown();

        return false;
    }


/*
 * --------------------------------------------------------
 * SELECTION DU NIVEAU
 * --------------------------------------------------------
 *
 * L'écran de sélection utilise le même
 * renderer SDL que l'Intro.
 */

if (!m_selLevel.init(
        m_renderer.getSDLRenderer()))
{
    std::cerr
        << "ERROR: SelLevel init failed"
        << std::endl;

    shutdown();

    return false;
}


        /*
         * --------------------------------------------------------
         * ATTENTE DE LA SELECTION
         * --------------------------------------------------------
         *
         * PC :
         *     flèches
         *     RETURN
         *
         * Anbernic :
         *     D-PAD
         *     bouton B
         *
         * Le niveau sélectionné est retourné
         * dans selectedLevel.
         */

        int selectedLevel = 0;

        if (!m_selLevel.run(
                m_input,
                selectedLevel))
        {
            shutdown();

            return false;
        }


        /*
         * --------------------------------------------------------
         * CHARGEMENT DU NIVEAU CHOISI
         * --------------------------------------------------------
         */

        if (!loadLevel(selectedLevel))
        {
            std::cerr
                << "ERROR: Impossible de charger le niveau "
                << (selectedLevel + 1)
                << std::endl;

            shutdown();

            return false;
        }


        m_running = true;

        return true;


   //avant
/*
    if (!loadLevel(0))
    {
        std::cerr
            << "ERROR: Impossible de charger le niveau 1"
            << std::endl;

        shutdown();

        return false;
    }


    m_running = true;

    return true;
*/
}


/*
 * ============================================================
 * CHARGEMENT D'UN NIVEAU
 * ============================================================
 */

bool Game::loadLevel(
    int levelIndex)
{
    if (levelIndex < 0 ||
        levelIndex >= LEVEL_COUNT)
    {
        return false;
    }


    if (!m_level.load(
            LEVEL_DEFINITIONS[levelIndex]))
    {
        return false;
    }


    m_currentLevel =
        levelIndex;


    std::cout
        << "INFO: Niveau "
        << (m_currentLevel + 1)
        << " / "
        << LEVEL_COUNT
        << std::endl;


    return true;
}


/*
 * ============================================================
 * BOUCLE PRINCIPALE
 * ============================================================
 */

void Game::run()
{

    while (m_running)
    {
        m_input.update();


        /*
         * ------------------------------------------------------
         * QUITTER LE JEU
         * ------------------------------------------------------
         */

        if (m_input.quitRequested())
        {
            m_running = false;
            break;
        }

         /*
         * Changement de musique.
         */

        if (m_input.previousMusicRequested())
        {
            m_audio.previousMusic();
        }


        if (m_input.nextMusicRequested())
        {
            m_audio.nextMusic();
        }



        /*
         * ------------------------------------------------------
         * RETOUR A LA SELECTION DES NIVEAUX
         * ------------------------------------------------------
         *
         * PC :
         *     BACKSPACE
         *
         * Anbernic :
         *     X
         */

        if (m_input.levelSelectRequested())
        {
            int selectedLevel =
                m_currentLevel;


            /*
             * Affiche à nouveau le sélecteur.
             */

            if (!m_selLevel.run(
                    m_input,
                    selectedLevel))
            {
                /*
                 * ESC / MENU :
                 * quitter le jeu.
                 */

                m_running = false;

                break;
            }


            /*
             * Charge le niveau choisi.
             */

            if (!loadLevel(selectedLevel))
            {
                std::cerr
                    << "ERROR: Impossible de charger le niveau "
                    << (selectedLevel + 1)
                    << std::endl;

                m_running = false;

                break;
            }


            /*
             * Le niveau vient d'être sélectionné.
             * On passe directement à la frame suivante.
             */

            continue;
        }


        /*
         * ------------------------------------------------------
         * REINITIALISER LE NIVEAU
         * ------------------------------------------------------
         *
         * PC :
         *     RETURN
         *
         * Anbernic :
         *     B
         */

        if (m_input.restartRequested())
        {
            if (!loadLevel(m_currentLevel))
            {
                std::cerr
                    << "ERROR: Impossible de réinitialiser le niveau "
                    << (m_currentLevel + 1)
                    << std::endl;

                m_running = false;

                break;
            }

            continue;
        }


        /*
         * ------------------------------------------------------
         * MISE A JOUR DU JEU
         * ------------------------------------------------------
         */

        update();


        /*
         * ------------------------------------------------------
         * AFFICHAGE
         * ------------------------------------------------------
         */

        m_renderer.render(
            m_level,
            m_levelCompleted
        );

        if (m_levelCompleted)
        {
            SDL_Delay(DELAY_SPLASH_BRAVO);

            m_levelCompleted = false;

            nextLevel();

            continue;
        }


        SDL_Delay(8);
    }


}


/*
 * ============================================================
 * MISE À JOUR
 * ============================================================
 */

void Game::update()
{
    /*
     * Aucun déplacement demandé.
     */

    if (!m_input.movePressed())
    {
        return;
    }


    const int dx =
        m_input.moveX();

    const int dy =
        m_input.moveY();


    /*
     * Sécurité.
     */

    if (dx == 0 &&
        dy == 0)
    {
        return;
    }


    tryMove(
        dx,
        dy
    );
}


/*
 * ============================================================
 * DEPLACEMENT
 * ============================================================
 */

void Game::tryMove(
    int dx,
    int dy)
{
    /*
     * Le Level détermine le résultat :
     *
     * Blocked
     * Moved
     * Pushed
     */

    const MoveResult result =
        m_level.movePlayer(
            dx,
            dy
        );


    switch (result)
    {
        /*
         * ----------------------------------------------------
         * Déplacement impossible
         * ----------------------------------------------------
         */

        case MoveResult::Blocked:
        {
            /*
             * Aucun son.
             */

            return;
        }


        /*
         * ----------------------------------------------------
         * Déplacement normal
         * ----------------------------------------------------
         */

        case MoveResult::Moved:
        {
            /*
             * sounds/move.wav
             */

            //m_audio.playMove();

            break;
        }


        /*
         * ----------------------------------------------------
         * Poussée d'une caisse
         * ----------------------------------------------------
         */

        case MoveResult::Pushed:
        {
            /*
             * sounds/push.wav
             */

            m_audio.playPush();

            break;
        }
    }


    /*
     * Vérification de la fin du niveau.
     */

    if (m_level.isCompleted())
    {

       /*
         * Le niveau vient d'être réussi.
         *
         * m_currentLevel :
         *
         *     0  = LEVEL01
         *     1  = LEVEL02
         *     ...
         *     49 = LEVEL50
         *
         * La méthode sauvegarde immédiatement
         * l'information dans levels_save.txt.
         */

        if (!m_selLevel.setLevelCompleted(m_currentLevel))
        {
            std::cerr
                << "WARNING: Impossible de sauvegarder "
                << "le niveau "
                << (m_currentLevel + 1)
                << std::endl;
        }


        /*
         * sounds/complete.wav
         */

        m_audio.playComplete();


        /*
         * Petit délai pour laisser entendre
         * le son de fin.
         */

        //SDL_Delay(500);


        /*
         * Niveau suivant.
         */

        //nextLevel();
        m_levelCompleted = true;
    }
}


/*
 * ============================================================
 * NIVEAU SUIVANT
 * ============================================================
 */

void Game::nextLevel()
{
    const int nextLevel =
        m_currentLevel + 1;


    /*
     * Pour l'instant les trois niveaux
     * utilisent le même LEVEL_01.
     *
     * Une fois le dernier terminé,
     * on recommence au niveau 1.
     */

    if (nextLevel >= LEVEL_COUNT)
    {
        loadLevel(0);

        return;
    }


    /*
     * Charge le niveau suivant.
     */

    if (!loadLevel(nextLevel))
    {
        std::cerr
            << "ERROR: Impossible de charger le niveau "
            << (nextLevel + 1)
            << std::endl;

        m_running = false;

        return;
    }
}


/*
 * ============================================================
 * ARRET
 * ============================================================
 */

void Game::shutdown()
{
    /*
     * On arrête la boucle.
     */

    m_running = false;


    /*
     * Audio.
     */

    m_audio.close();

    m_selLevel.shutdown();

    /*
     * Input.
     */

    m_input.shutdown();


    /*
     * Renderer.
     */

    m_renderer.shutdown();
}
