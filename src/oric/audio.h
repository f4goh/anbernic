#pragma once

#include <stdint.h>
#include <stdbool.h>


#define AY38910_CLOCK    1000000
#define SAMPLE_FREQUENCY 22050


bool audioInit(int psgClock, int sampleRate);

void audioShutdown(void);


/*
    Appelé automatiquement par SDL
*/
void audioCallback(void *userdata,
                   uint8_t *stream,
                   int len);



/*
    Accès PSG
    équivalent de ton Pico
*/
uint8_t audioReadPsg0(void);

void audioWritePsg0(uint16_t addr, uint8_t val);



/*
    Sauvegarde état PSG
*/
uint32_t audioSize(void);

uint8_t *audioSave(void);

void audioLoad(const uint8_t *buffer);
