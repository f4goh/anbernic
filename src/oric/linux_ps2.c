#include "linux_ps2.h"
#include <linux/input-event-codes.h>


#define FIFO_SIZE 32


static uint8_t fifo[FIFO_SIZE];

static uint8_t head=0;
static uint8_t tail=0;



static void push(uint8_t c)
{
    fifo[head]=c;
    head=(head+1)%FIFO_SIZE;
}



uint8_t linux_ps2_read(void)
{
    uint8_t c;


    if(head==tail)
        return 0;


    c=fifo[tail];

    tail=(tail+1)%FIFO_SIZE;

    return c;
}



/*
 Linux Set1 -> PS2 Set2
*/

typedef struct
{
    uint16_t key;
    uint8_t set2;
    uint8_t ext;
} Conv;


static const Conv table[] =
{

/* Lettres */

{KEY_Q,0x15,0},
{KEY_W,0x1D,0},
{KEY_E,0x24,0},
{KEY_R,0x2D,0},
{KEY_T,0x2C,0},
{KEY_Y,0x35,0},
{KEY_U,0x3C,0},
{KEY_I,0x43,0},
{KEY_O,0x44,0},
{KEY_P,0x4D,0},

{KEY_A,0x1C,0},
{KEY_S,0x1B,0},
{KEY_D,0x23,0},
{KEY_F,0x2B,0},
{KEY_G,0x34,0},
{KEY_H,0x33,0},
{KEY_J,0x3B,0},
{KEY_K,0x42,0},
{KEY_L,0x4B,0},

{KEY_Z,0x1A,0},
{KEY_X,0x22,0},
{KEY_C,0x21,0},
{KEY_V,0x2A,0},
{KEY_B,0x32,0},
{KEY_N,0x31,0},
{KEY_M,0x3A,0},

/* Chiffres */

{KEY_1,0x16,0},
{KEY_2,0x1E,0},
{KEY_3,0x26,0},
{KEY_4,0x25,0},
{KEY_5,0x2E,0},
{KEY_6,0x36,0},
{KEY_7,0x3D,0},
{KEY_8,0x3E,0},
{KEY_9,0x46,0},
{KEY_0,0x45,0},

/* Symboles */

{KEY_MINUS,0x4E,0},
{KEY_EQUAL,0x55,0},

{KEY_LEFTBRACE,0x54,0},
{KEY_RIGHTBRACE,0x5B,0},

{KEY_SEMICOLON,0x4C,0},
{KEY_APOSTROPHE,0x52,0},

{KEY_COMMA,0x41,0},
{KEY_DOT,0x49,0},
{KEY_SLASH,0x4A,0},

/* Contrôles */

{KEY_ESC,0x76,0},
{KEY_BACKSPACE,0x66,0},
{KEY_TAB,0x0D,0},
{KEY_ENTER,0x5A,0},
{KEY_SPACE,0x29,0},

/* Modifieurs */

{KEY_LEFTSHIFT,0x12,0},
{KEY_RIGHTSHIFT,0x59,0},

{KEY_LEFTCTRL,0x14,0},
{KEY_RIGHTCTRL,0x14,1},

{KEY_LEFTALT,0x11,0},
{KEY_RIGHTALT,0x11,1},

/* Fonctions */

{KEY_F1,0x05,0},
{KEY_F2,0x06,0},
{KEY_F3,0x04,0},
{KEY_F4,0x0C,0},
{KEY_F5,0x03,0},
{KEY_F6,0x0B,0},
{KEY_F7,0x83,0},
{KEY_F8,0x0A,0},
{KEY_F9,0x01,0},
{KEY_F10,0x09,0},
{KEY_F11,0x78,0},
{KEY_F12,0x07,0},

/* Flèches */

{KEY_UP,0x75,1},
{KEY_DOWN,0x72,1},
{KEY_LEFT,0x6B,1},
{KEY_RIGHT,0x74,1},

};



#define TABLE_SIZE (sizeof(table)/sizeof(table[0]))

void linux_ps2_init(void)
{
    head = 0;
    tail = 0;
}



void linux_ps2_feed(uint16_t code, bool released)
{
    for(int i = 0; i < TABLE_SIZE; i++)
    {
        if(table[i].key != code)
            continue;

        if(table[i].ext)
            push(0xE0);

        if(released)
            push(0xF0);

        push(table[i].set2);

        return;
    }
}


