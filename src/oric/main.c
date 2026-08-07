

#include "bus.h"

int main(void)
{

    busInit();

    // it's go time!
    busMainLoop();

    //while (1) { }

    return 0;

}



/*
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "ula.h"
#include "video.h"
#include "audio.h"
#include "music.h"
#include "input.h"


#include "videoMemHires.h"
//#include "videoMemLores.h"



uint8_t ram[65536];


#define ORIC_MONITOR_SIZE 0x0460

uint8_t ramMoni[ORIC_MONITOR_SIZE];

static void music_update(void)
{
    static uint32_t music_pos = 0;


    for(uint8_t reg=0; reg<16; reg++)
    {
        uint8_t val = music[music_pos++];

        audioWritePsg0(0, reg);
        audioWritePsg0(1, val);
    }


    if(music_pos >= LENGTH)
    {
        music_pos=0;
    }
}


int main(void)
{

    Ula ula;


    memset(ram,0,sizeof(ram));


    ula_init(&ula);
    ula_reset(&ula);

  
    memcpy(ram+0x9800,
           hires,
           0x2800);


    ula.pattr=PATTR_HIRES;



    if(!video_init(&ula))
        return -1;

    audioInit(AY38910_CLOCK, SAMPLE_FREQUENCY);

    if(!input_init())
    {
        printf("Input non disponible\n");
    }
    else
    {
        printf("Input OK\n");
    }


    bool quit=false;



    while(!quit)
    {

        SDL_Event e;


        while(SDL_PollEvent(&e))
        {
            if(e.type==SDL_QUIT)
                quit=true;


            if(e.type==SDL_KEYDOWN)
            {
                if(e.key.keysym.sym==SDLK_ESCAPE)
                    quit=true;
            }
        }

        input_update();


        if(input_down(INPUT_MENU))
        {
            quit=true;
        }


        static uint32_t last_time = 0;
        static uint32_t music_timer = 0;


        uint32_t now = SDL_GetTicks();


        if(last_time == 0)
        {
            last_time = now;
        }


        music_timer += now - last_time;

        last_time = now;



        while(music_timer >= 20)
        {
            music_timer -= 20;

            music_update();
        }


     

        video_display();
    }



    video_shutdown();


    return 0;
}
*/

