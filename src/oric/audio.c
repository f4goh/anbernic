#include "audio.h"

#include "emu2149.h"


#include <SDL2/SDL.h>

#include <stdlib.h>
#include <string.h>

#define AUDIO_GAIN 2


//#include "music.h"



static PSG *psg0 = NULL;


/*
    registre sélectionné AY
*/
static uint8_t psg0Reg = 0;



/*
    Filtre simple identique à ton Pico
*/
#define ALPHA 60

static int32_t lp = 0;



static SDL_AudioDeviceID audioDevice;



//--------------------------------------------------
// Création PSG
//--------------------------------------------------

static PSG *createPSG(int psgClock, int sampleRate)
{
    PSG *psg;


    psg = PSG_new(psgClock, sampleRate);

    if(psg)
    {
        PSG_setVolumeMode(
            psg,
            EMU2149_VOL_AY_3_8910);

        PSG_reset(psg);
    }


    return psg;
}



//--------------------------------------------------
// Callback SDL audio
//--------------------------------------------------

void audioCallback(void *userdata,
                   uint8_t *stream,
                   int len)
{
    (void)userdata;


    int16_t *samples = (int16_t *)stream;


    int count = len / sizeof(int16_t);



    for(int i=0;i<count;i++)
    {

        /*
            Génération AY
        */
        int16_t sample = PSG_calc(psg0);



        /*
            filtre simple
        */

        lp += (ALPHA * ((int32_t)sample - lp)) >> 8;

        int32_t s = lp * AUDIO_GAIN;

        if (s > 32767) s = 32767;
        if (s < -32768) s = -32768;

        samples[i] = (int16_t)s;


       // samples[i]=(int16_t)lp;
    }
}



//--------------------------------------------------
// Initialisation SDL audio
//--------------------------------------------------

bool audioInit(int psgClock, int sampleRate)
{

    SDL_AudioSpec want;
    SDL_AudioSpec have;


    memset(&want,0,sizeof(want));


    psg0=createPSG(
        psgClock,
        sampleRate);



    if(psg0==NULL)
        return false;



    want.freq=sampleRate;

    want.format=AUDIO_S16SYS;

    want.channels=1;

    /*
        taille tampon
        environ 8 ms à 31 kHz
    */
    want.samples=256;

    want.callback=audioCallback;



    audioDevice =
        SDL_OpenAudioDevice(
            NULL,
            0,
            &want,
            &have,
            0);



    if(audioDevice==0)
    {
        SDL_Log(
          "Erreur audio SDL : %s",
          SDL_GetError());

        PSG_delete(psg0);
        psg0=NULL;

        return false;
    }



    SDL_Log(
        "AY audio : %d Hz",
        have.freq);



    SDL_PauseAudioDevice(
        audioDevice,
        0);



    return true;
}



//--------------------------------------------------

void audioShutdown(void)
{

    if(audioDevice)
    {
        SDL_CloseAudioDevice(audioDevice);
        audioDevice=0;
    }


    if(psg0)
    {
        PSG_delete(psg0);
        psg0=NULL;
    }
}



//--------------------------------------------------
// Accès registres AY
//--------------------------------------------------


uint8_t audioReadPsg0(void)
{
    return PSG_readReg(
        psg0,
        psg0Reg);
}




void audioWritePsg0(uint16_t addr,
                    uint8_t val)
{

    /*
       adresse paire :
       sélection registre

       adresse impaire :
       écriture registre
    */

    if((addr & 1)==0)
    {
        psg0Reg=val;
    }
    else
    {
        PSG_writeReg(
            psg0,
            psg0Reg,
            val);
    }
}



//--------------------------------------------------
// Save state
//--------------------------------------------------


uint32_t audioSize(void)
{
    return sizeof(savePSG);
}



uint8_t *audioSave(void)
{
    return (uint8_t *)psg0;
}



void audioLoad(const uint8_t *buffer)
{
    memcpy(
        psg0,
        buffer,
        sizeof(savePSG));
}
