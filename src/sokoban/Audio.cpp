
#include "Audio.h"

#include <iostream>
#include <cstdio>


Audio::Audio()
    : m_initialized(false),
      m_music(nullptr),
      m_currentMod(0),
      m_moveSound(nullptr),
      m_pushSound(nullptr),
      m_completeSound(nullptr),
      m_musicVolume(64),
      m_soundVolume(96)
{
}


Audio::~Audio()
{
    close();
}


/*
 * ==========================================================
 * INITIALISATION
 * ==========================================================
 */

bool Audio::init()
{
    /*
     * SDL_mixer doit gérer les modules MOD.
     */

    int flags =
        MIX_INIT_MOD;


    int initialized =
        Mix_Init(flags);


    if ((initialized & flags) != flags)
    {
        std::cerr
            << "Erreur Mix_Init : "
            << Mix_GetError()
            << std::endl;

        return false;
    }


    /*
     * Ouvre le périphérique audio.
     */

    if (Mix_OpenAudio(
            22050,
            AUDIO_S16SYS,
            2,
            1024) < 0)
    {
        std::cerr
            << "Erreur Mix_OpenAudio : "
            << Mix_GetError()
            << std::endl;

        Mix_Quit();

        return false;
    }


    /*
     * Allocation des canaux de bruitage.
     */

    Mix_AllocateChannels(8);


    /*
     * ------------------------------------------------------
     * MUSIQUE
     * ------------------------------------------------------
     *
     * On démarre toujours avec soko01.mod.
     */

    m_currentMod = 0;


    if (!loadMusic(m_currentMod))
    {
        close();

        return false;
    }


    /*
     * ------------------------------------------------------
     * BRUITAGE DEPLACEMENT
     * ------------------------------------------------------
     */

    m_moveSound =
        Mix_LoadWAV("sounds/move.wav");


    if (m_moveSound == nullptr)
    {
        std::cerr
            << "Impossible de charger sounds/move.wav : "
            << Mix_GetError()
            << std::endl;

        close();

        return false;
    }


    /*
     * ------------------------------------------------------
     * BRUITAGE POUSSEE
     * ------------------------------------------------------
     */

    m_pushSound =
        Mix_LoadWAV("sounds/push.wav");


    if (m_pushSound == nullptr)
    {
        std::cerr
            << "Impossible de charger sounds/push.wav : "
            << Mix_GetError()
            << std::endl;

        close();

        return false;
    }


    /*
     * ------------------------------------------------------
     * BRUITAGE FIN DE NIVEAU
     * ------------------------------------------------------
     */

    m_completeSound =
        Mix_LoadWAV("sounds/complete.wav");


    if (m_completeSound == nullptr)
    {
        std::cerr
            << "Impossible de charger sounds/complete.wav : "
            << Mix_GetError()
            << std::endl;

        close();

        return false;
    }


    /*
     * ------------------------------------------------------
     * VOLUMES
     * ------------------------------------------------------
     */

    Mix_VolumeMusic(
        m_musicVolume
    );


    Mix_VolumeChunk(
        m_moveSound,
        m_soundVolume
    );


    Mix_VolumeChunk(
        m_pushSound,
        m_soundVolume
    );


    Mix_VolumeChunk(
        m_completeSound,
        m_soundVolume
    );


    m_initialized = true;


    std::cout
        << "INFO: Audio OK"
        << std::endl;


    std::cout
        << "INFO: Module musical : "
        << (m_currentMod + 1)
        << " / "
        << MAX_MOD
        << std::endl;


    return true;
}


/*
 * ==========================================================
 * CHARGEMENT D'UN MODULE
 * ==========================================================
 */

bool Audio::loadMusic(
    int modIndex)
{
    /*
     * Sécurité.
     */

    if (modIndex < 0 ||
        modIndex >= MAX_MOD)
    {
        return false;
    }


    /*
     * Si une musique est déjà chargée,
     * on la libère.
     */

    if (m_music != nullptr)
    {
        Mix_HaltMusic();

        Mix_FreeMusic(
            m_music
        );

        m_music = nullptr;
    }


    /*
     * Construction du nom du fichier.
     *
     * 0 -> soko01.mod
     * 1 -> soko02.mod
     * 2 -> soko03.mod
     */

    char filename[64];


    std::snprintf(
        filename,
        sizeof(filename),
        "music/soko%02d.mod",
        modIndex + 1
    );


    std::cout
        << "INFO: Chargement musique : "
        << filename
        << std::endl;


    m_music =
        Mix_LoadMUS(filename);


    if (m_music == nullptr)
    {
        std::cerr
            << "Impossible de charger "
            << filename
            << " : "
            << Mix_GetError()
            << std::endl;

        return false;
    }


    /*
     * Applique le volume actuel.
     */

    Mix_VolumeMusic(
        m_musicVolume
    );


    /*
     * Le module devient le module courant.
     */

    m_currentMod =
        modIndex;


    return true;
}


/*
 * ==========================================================
 * FERMETURE
 * ==========================================================
 */

void Audio::close()
{
    /*
     * Bruitage déplacement.
     */

    if (m_moveSound != nullptr)
    {
        Mix_FreeChunk(
            m_moveSound
        );

        m_moveSound = nullptr;
    }


    /*
     * Bruitage poussée.
     */

    if (m_pushSound != nullptr)
    {
        Mix_FreeChunk(
            m_pushSound
        );

        m_pushSound = nullptr;
    }


    /*
     * Bruitage fin.
     */

    if (m_completeSound != nullptr)
    {
        Mix_FreeChunk(
            m_completeSound
        );

        m_completeSound = nullptr;
    }


    /*
     * Musique.
     */

    if (m_music != nullptr)
    {
        Mix_HaltMusic();

        Mix_FreeMusic(
            m_music
        );

        m_music = nullptr;
    }


    /*
     * Fermeture SDL_mixer.
     */

    if (m_initialized)
    {
        Mix_CloseAudio();

        Mix_Quit();

        m_initialized = false;
    }
    else
    {
        /*
         * Sécurité si init() a échoué
         * avant m_initialized = true.
         */

        Mix_CloseAudio();

        Mix_Quit();
    }
}


/*
 * ==========================================================
 * MUSIQUE
 * ==========================================================
 */

void Audio::playMusic()
{
    if (!m_initialized)
        return;


    if (m_music == nullptr)
        return;


    /*
     * -1 = boucle infinie.
     */

    if (Mix_PlayMusic(
            m_music,
            -1) == -1)
    {
        std::cerr
            << "Erreur lecture musique : "
            << Mix_GetError()
            << std::endl;
    }
}


/*
 * ==========================================================
 * MUSIQUE SUIVANTE
 * ==========================================================
 */

void Audio::nextMusic()
{
    if (!m_initialized)
        return;


    int nextMod =
        m_currentMod + 1;


    /*
     * Après le dernier module,
     * retour au premier.
     */

    if (nextMod >= MAX_MOD)
    {
        nextMod = 0;
    }


    if (!loadMusic(nextMod))
    {
        std::cerr
            << "ERROR: Impossible de charger le module "
            << (nextMod + 1)
            << std::endl;

        return;
    }


    /*
     * Lance immédiatement le nouveau module.
     */

    playMusic();
}


/*
 * ==========================================================
 * MUSIQUE PRECEDENTE
 * ==========================================================
 */

void Audio::previousMusic()
{
    if (!m_initialized)
        return;


    int previousMod =
        m_currentMod - 1;


    /*
     * Avant le premier module,
     * retour au dernier.
     */

    if (previousMod < 0)
    {
        previousMod =
            MAX_MOD - 1;
    }


    if (!loadMusic(previousMod))
    {
        std::cerr
            << "ERROR: Impossible de charger le module "
            << (previousMod + 1)
            << std::endl;

        return;
    }


    /*
     * Lance immédiatement le nouveau module.
     */

    playMusic();
}


/*
 * ==========================================================
 * MODULE ACTUEL
 * ==========================================================
 */

int Audio::currentMusic() const
{
    /*
     * Retourne 1, 2, 3...
     */

    return m_currentMod + 1;
}


/*
 * ==========================================================
 * ARRET MUSIQUE
 * ==========================================================
 */

void Audio::stopMusic()
{
    if (!m_initialized)
        return;


    Mix_HaltMusic();
}


/*
 * ==========================================================
 * BRUITAGE DEPLACEMENT
 * ==========================================================
 */

void Audio::playMove()
{
    if (!m_initialized)
        return;


    if (m_moveSound == nullptr)
        return;


    Mix_PlayChannel(
        -1,
        m_moveSound,
        0
    );
}


/*
 * ==========================================================
 * BRUITAGE POUSSEE
 * ==========================================================
 */

void Audio::playPush()
{
    if (!m_initialized)
        return;


    if (m_pushSound == nullptr)
        return;


    Mix_PlayChannel(
        -1,
        m_pushSound,
        0
    );
}


/*
 * ==========================================================
 * BRUITAGE FIN DE NIVEAU
 * ==========================================================
 */

void Audio::playComplete()
{
    if (!m_initialized)
        return;


    if (m_completeSound == nullptr)
        return;


    Mix_PlayChannel(
        -1,
        m_completeSound,
        0
    );
}


/*
 * ==========================================================
 * VOLUME MUSIQUE
 * ==========================================================
 */

void Audio::setMusicVolume(
    int volume)
{
    if (volume < 0)
        volume = 0;


    if (volume > MIX_MAX_VOLUME)
        volume = MIX_MAX_VOLUME;


    m_musicVolume =
        volume;


    if (m_initialized)
    {
        Mix_VolumeMusic(
            m_musicVolume
        );
    }
}


/*
 * ==========================================================
 * VOLUME BRUITAGES
 * ==========================================================
 */

void Audio::setSoundVolume(
    int volume)
{
    if (volume < 0)
        volume = 0;


    if (volume > MIX_MAX_VOLUME)
        volume = MIX_MAX_VOLUME;


    m_soundVolume =
        volume;


    if (m_initialized)
    {
        if (m_moveSound != nullptr)
        {
            Mix_VolumeChunk(
                m_moveSound,
                m_soundVolume
            );
        }


        if (m_pushSound != nullptr)
        {
            Mix_VolumeChunk(
                m_pushSound,
                m_soundVolume
            );
        }


        if (m_completeSound != nullptr)
        {
            Mix_VolumeChunk(
                m_completeSound,
                m_soundVolume
            );
        }
    }
}


