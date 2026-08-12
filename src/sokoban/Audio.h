
#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>


/*
 * Nombre maximum de modules MOD.
 *
 * Pour le moment :
 *
 * soko01.mod
 * soko02.mod
 * soko03.mod
 */
#define MAX_MOD 5


class Audio
{
public:

    Audio();

    ~Audio();


    /*
     * Initialise SDL_mixer et charge
     * les sons ainsi que le premier module.
     */
    bool init();


    /*
     * Libère tous les éléments audio.
     */
    void close();


    /*
     * ------------------------------------------------------
     * MUSIQUE
     * ------------------------------------------------------
     */

    /*
     * Lance le module actuellement sélectionné.
     */
    void playMusic();


    /*
     * Arrête la musique.
     */
    void stopMusic();


    /*
     * Module suivant.
     *
     * Exemple :
     *
     * 01 -> 02
     * 02 -> 03
     * 03 -> 01
     */
    void nextMusic();


    /*
     * Module précédent.
     *
     * Exemple :
     *
     * 01 -> 03
     * 03 -> 02
     * 02 -> 01
     */
    void previousMusic();


    /*
     * Retourne le numéro du module actuel.
     *
     * 1 = soko01.mod
     * 2 = soko02.mod
     * etc.
     */
    int currentMusic() const;


    /*
     * ------------------------------------------------------
     * BRUITAGES
     * ------------------------------------------------------
     */

    void playMove();

    void playPush();

    void playComplete();


    /*
     * ------------------------------------------------------
     * VOLUMES
     * ------------------------------------------------------
     */

    void setMusicVolume(int volume);

    void setSoundVolume(int volume);


private:

    bool m_initialized;


    /*
     * Musique MOD actuellement chargée.
     */
    Mix_Music* m_music;


    /*
     * Numéro du module actuellement sélectionné.
     *
     * 0 = soko01.mod
     * 1 = soko02.mod
     * 2 = soko03.mod
     */
    int m_currentMod;


    /*
     * Bruitages WAV.
     */
    Mix_Chunk* m_moveSound;

    Mix_Chunk* m_pushSound;

    Mix_Chunk* m_completeSound;


    int m_musicVolume;

    int m_soundVolume;


    /*
     * Charge un module MOD.
     */
    bool loadMusic(int modIndex);
};

#endif


