#include "Input.h"

#include <SDL2/SDL.h>

#ifdef __aarch64__

#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

#define ANALOG_THRESHOLD 1500

static int g_fd = -1;

#endif


Input::Input()
    : m_quit(false),
      m_moveX(0),
      m_moveY(0),
      m_previousMove(false),
      m_buttonA(false),
      m_buttonB(false),
      m_buttonY(false),
      m_confirm(false),
      m_levelSelect(false),
      m_nextMusic(false),
      m_previousMusic(false)
{
}


Input::~Input()
{
    shutdown();
}


bool Input::init()
{
#ifdef __aarch64__

    g_fd =
        open(
            "/dev/input/event1",
            O_RDONLY |
            O_NONBLOCK
        );

    if (g_fd < 0)
        return false;

#endif

    return true;
}


void Input::shutdown()
{
#ifdef __aarch64__

    if (g_fd >= 0)
    {
        close(g_fd);
        g_fd = -1;
    }

#endif
}


void Input::update()
{
    m_moveX = 0;
    m_moveY = 0;
    m_restart = false;
    m_confirm = false;
    m_levelSelect = false;

    m_nextMusic = false;
    m_previousMusic = false;

    /*
        ==========================
        PC
        ==========================
    */

#ifndef __aarch64__

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            m_quit = true;
        }


        if (event.type ==
            SDL_KEYDOWN)
        {
            if (event.key.keysym.sym ==
                SDLK_ESCAPE)
            {
                m_quit = true;
            }

           if (event.key.keysym.sym ==
                SDLK_RETURN)
            {
                m_restart = true;
            }

            if (event.key.keysym.sym ==
                SDLK_RETURN)
            {
                m_confirm = true;
            }

            if (event.key.keysym.sym ==
                SDLK_BACKSPACE)
            {
                m_levelSelect = true;
            }


            if (event.key.keysym.sym == SDLK_PLUS ||
                event.key.keysym.sym == SDLK_KP_PLUS ||
                event.key.keysym.sym == SDLK_EQUALS)
            {
                m_nextMusic = true;
            }

            if (event.key.keysym.sym == SDLK_MINUS ||
                event.key.keysym.sym == SDLK_KP_MINUS)
            {
                m_previousMusic = true;
            }

            switch (
                event.key.keysym.sym)
            {
                case SDLK_UP:

                    m_moveY = -1;

                    break;


                case SDLK_DOWN:

                    m_moveY = 1;

                    break;


                case SDLK_LEFT:

                    m_moveX = -1;

                    break;


                case SDLK_RIGHT:

                    m_moveX = 1;

                    break;


                /*
                    ESPACE n'est pas traité ici.

                    Il sera traité par Intro.
                */

                default:

                    break;
            }
        }
    }

#else

    /*
        ==========================
        ANBERNIC
        ==========================
    */

    struct input_event ev;

    while (
        read(
            g_fd,
            &ev,
            sizeof(ev)
        ) == sizeof(ev))
    {
        if (ev.type == EV_KEY)
        {
            bool pressed =
                (ev.value != 0);


            /*
                ==========================
                Bouton A
                code 304
                ==========================

                IMPORTANT :

                A ne quitte plus le programme.

                On mémorise simplement son état
                afin que Intro puisse l'utiliser.
            */

            if (ev.code == 304)
            {
                m_buttonA = pressed;
            }

            if (ev.code == 305)
            {
                m_buttonB = pressed;
                if (pressed)
                    {
                        m_restart = true;
                    }
            }

            /*
             * Bouton Y
             * code 306
             */

            if (ev.code == 306)
            {
                m_buttonY = pressed;
            }


            /*
             * Bouton X
             * code 307
             *
             * X retourne à la sélection
             * des niveaux.
             */

            if (ev.code == 307 &&
                pressed)
            {
                m_levelSelect = true;
            }

            /*
             * L2
             * code 314
             *
             * Module précédent.
             */
            if (ev.code == 314 &&
                pressed)
            {
                m_previousMusic = true;
            }


            /*
             * R2
             * code 315
             *
             * Module suivant.
             */
            if (ev.code == 315 &&
                pressed)
            {
                m_nextMusic = true;
            }


            /*
                ==========================
                Bouton MENU
                code 354
                ==========================

                MENU permet de quitter le jeu.
            */

            if (ev.code == 354 &&
                pressed)
            {
                m_quit = true;
            }
        }


        else if (ev.type == EV_ABS)
        {
            /*
                ==========================
                D-PAD
                ==========================
            */

            if (ev.code == 16)
            {
                if (ev.value < 0)
                    m_moveX = -1;

                if (ev.value > 0)
                    m_moveX = 1;
            }


            if (ev.code == 17)
            {
                if (ev.value < 0)
                    m_moveY = -1;

                if (ev.value > 0)
                    m_moveY = 1;
            }


            /*
                ==========================
                Stick gauche
                ==========================
            */

            if (ev.code == 2)
            {
                if (ev.value <
                    -ANALOG_THRESHOLD)
                {
                    m_moveX = -1;
                }

                else if (
                    ev.value >
                    ANALOG_THRESHOLD)
                {
                    m_moveX = 1;
                }
            }


            if (ev.code == 3)
            {
                if (ev.value <
                    -ANALOG_THRESHOLD)
                {
                    m_moveY = -1;
                }

                else if (
                    ev.value >
                    ANALOG_THRESHOLD)
                {
                    m_moveY = 1;
                }
            }
        }
    }

#endif
}


bool Input::quitRequested() const
{
    return m_quit;
}


int Input::moveX() const
{
    return m_moveX;
}


int Input::moveY() const
{
    return m_moveY;
}


bool Input::movePressed()
{
    bool pressed =
        (m_moveX != 0 ||
         m_moveY != 0);


    /*
        Une seule case par appui.

        Cela évite que le personnage
        traverse toute la carte à
        cause de l'autorepeat clavier.
    */

    if (pressed && !m_previousMove)
    {
        m_previousMove = true;

        return true;
    }


    if (!pressed)
    {
        m_previousMove = false;
    }


    return false;
}


/*
    ==========================================================
    BOUTON A
    ==========================================================

    Utilisé par Intro.

    Sur PC cette fonction retourne false :
    le PC utilise ESPACE.

    Sur Anbernic elle retourne l'état
    du bouton A.
*/

bool Input::buttonA() const
{
#ifdef __aarch64__

    return m_buttonA;

#else

    return false;

#endif
}

/*
    ==========================================================
    BOUTON A
    ==========================================================

    Utilisé par Intro.

    Sur PC cette fonction retourne false :
    le PC utilise ESPACE.

    Sur Anbernic elle retourne l'état
    du bouton A.
*/

bool Input::buttonB() const
{
#ifdef __aarch64__

    return m_buttonB;

#else

    return false;

#endif
}


bool Input::buttonY() const
{
#ifdef __aarch64__

    return m_buttonY;

#else

    return false;

#endif
}

bool Input::restartRequested() const
{
    return m_restart;
}

bool Input::confirmPressed() const
{
    return m_confirm;
}

bool Input::levelSelectRequested() const
{
    return m_levelSelect;
}

bool Input::nextMusicRequested() const
{
    return m_nextMusic;
}


bool Input::previousMusicRequested() const
{
    return m_previousMusic;
}


