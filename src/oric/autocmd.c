#include <stdint.h>
#include <stdbool.h>


extern volatile uint8_t ula_kbdScancode;


static const uint8_t cload_seq[] =
{
    0x21,0xf0,0x21,
    0x4b,0xf0,0x4b,
    0x44,0xf0,0x44,
    0x1c,0xf0,0x1c,
    0x23,0xf0,0x23,
    0x59,0x52,0xf0,0x52,0xf0,0x59,
    0x5a
};


static bool auto_active = false;

static int auto_index = 0;
static int auto_delay = 0;



void autoCmd_start(void)
{
    auto_active = true;

    auto_index = 0;

    auto_delay = 0;
}



void autoCmd(void)
{
    if(!auto_active)
        return;


    /*
       délai entre deux scans clavier
       1 frame = 16ms
    */

    if(auto_delay > 0)
    {
        auto_delay--;
        return;
    }



    /*
       envoyer un scan
    */

    ula_kbdScancode = cload_seq[auto_index];



    auto_index++;


    /*
       temps de maintien entre codes
       à ajuster si besoin
    */

    auto_delay = 2;



    if(auto_index >= sizeof(cload_seq))
    {
        auto_active = false;
    }
}
