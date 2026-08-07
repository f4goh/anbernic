#include "ula.h"
#include <string.h>
#include "keyboard.h"
#include "config.h"
#include <stdio.h>
#include "linux_ps2.h"


// palette 8 couleurs Oric
/*
 * Palette convertie en format 12-bit BGR
 * suivant la fonction colorFromRgb()
 */

/*
static const uint16_t pal[8] =
{
    0x000, // black
    0x001, // red
    0x002, // green
    0x003, // yellow
    0x004, // blue
    0x005, // magenta
    0x006, // cyan
    0x007  // white
};
*/

static const uint32_t rgbPalette[8]=
{
    0xFF000000,
    0xFFFF0000,
    0xFF00FF00,
    0xFFFFFF00,
    0xFF0000FF,
    0xFFFF00FF,
    0xFF00FFFF,
    0xFFFFFFFF
};

extern uint8_t oricRom[];
extern uint8_t charset[];
extern uint8_t ramMoni[];

volatile uint8_t ula_kbdScancode = 0;
volatile uint8_t ula_nesValue = 0xff;
volatile uint8_t ula_nesSt = 0x0;

// -------------------------

void ula_init(Ula* u) {
    //memset(u->ram, 0x00, RAM_SIZE);
    keyboard_init(config.keyboard);
    //keyboard_init("/dev/input/event3");  //3 pour ambernic
    //keyboard_init("/dev/input/event4");  // portable
    //keyboard_init("/dev/input/event15");  //3 pour usb ext
    keyboard_flush();
    linux_ps2_init();
}

// -------------------------
void ula_reset(Ula* u)
{
   
    u->pattr=0;  //LORES
    u->pattrSave = 0;
    u->blink_counter=0;
    u->enMonitor=0;

}


void ula_monitor(Ula* u, bool en)
{
    if (en && !u->enMonitor)
    {
        u->enMonitor = true;
        /* Sauvegarde de l'état vidéo courant */
        u->pattrSave = u->pattr;
        /* Forcer le mode texte */
        u->pattr &= ~PATTR_HIRES;
    }
    else if (!en && u->enMonitor)
    {
        u->enMonitor = false;
        /* Restauration de l'état vidéo */
        u->pattr = u->pattrSave;
    }
}





// -------------------------
// hires/text Oric timing
// -------------------------
void ula_scanline(Ula* u, uint16_t y, uint32_t* out)
//void ula_scanline(Ula* u, uint16_t y, uint8_t* out) 
{
    //bool blink_state = u->blink_counter & 0x20;  //valeur pour 50hz
    bool blink_state = u->blink_counter & 0x10;  //adaptation pour 60hz


    //uint8_t pattr = u->pattr;
    uint8_t pattr = u->enMonitor ? 0 : u->pattr;
        

    uint8_t lattr = 0;
    uint8_t fgcol = 7;
    uint8_t bgcol = 0;

    uint8_t* p= u->fb;

    if (y>=ORIC_SCREEN_HEIGHT){
         for (int i=0;i<320;i++) out[i] = 0;
         return;
    }
    
    for (int x = 0; x < 40; x++) {
        // Lookup the byte and, if needed, the pattern data
        uint8_t ch, pat;
        if ((pattr & PATTR_HIRES) && y < 200)
            //ch = pat = u->ram[0xA000 + y * 40 + x];
            //ch = pat = busRead(0xA000 + y * 40 + x,0);    
            ch = pat = ram[0xA000 + y * 40 + x];

        else {
            //ch = u->ram[0xBB80 + (y >> 3) * 40 + x];
            if (!u->enMonitor){
              ch = ram[0xBB80 + (y >> 3) * 40 + x];  //oui
            }
            else{
              ch = ramMoni[(y >> 3) * 40 + x];  
            }
            int off = (lattr & LATTR_DSIZE ? y >> 1 : y) & 7;
            const uint8_t* base;
            if (pattr & PATTR_HIRES)
                if (lattr & LATTR_ALT)
                    //base = u->ram + 0x9C00;
                    base = ram + 0x9C00;  //altcharset hires
                else
                    //base = u->ram + 0x9800;
                    base = ram + 0x9800;  //charset hires
            else if (lattr & LATTR_ALT)
                //base = u->ram + 0xB800;
                base = ram + 0xB800;  //altcharset text
            else
                 if (!u->enMonitor)
                    base = ram + 0xB400;  //charset text
                 else 
                    base = charset;
                
                // if (!u->enMonitor)
                //    base = ram + 0xB400;  //charset text
                // else if (u->pattrSave & PATTR_HIRES)
                //     base = ram + 0x9800;  //charset hires
                //      else base = ram + 0xB400;
                 
            pat = base[((ch & 0x7F) << 3) | off];
        }

        // Handle state-chaging attributes
        if (!(ch & 0x60)) {
            pat = 0x00;
            switch (ch & 0x18) {
                case 0x00:
                    fgcol = ch & 7;
                    break;
                case 0x08:
                    lattr = ch & 7;
                    break;
                case 0x10:
                    bgcol = ch & 7;
                    break;
                case 0x18:
                    pattr = ch & 7;
                    break;
            }
        }

        // Pick up the colors for the pattern
        uint8_t c_fgcol = fgcol;
        uint8_t c_bgcol = bgcol;

        // inverse video
        if (ch & 0x80) {
            c_bgcol = c_bgcol ^ 0x07;
            c_fgcol = c_fgcol ^ 0x07;
        }
        // blink
        if ((lattr & LATTR_BLINK) && blink_state) c_fgcol = c_bgcol;

        // Draw the pattern
        uint8_t c;
        c = pat & 0x20 ? c_fgcol : c_bgcol;
        *p = c << 4;
        c = pat & 0x10 ? c_fgcol : c_bgcol;
        *p++ |= c;
        c = pat & 0x08 ? c_fgcol : c_bgcol;
        *p = c << 4;
        c = pat & 0x04 ? c_fgcol : c_bgcol;
        *p++ |= c;
        c = pat & 0x02 ? c_fgcol : c_bgcol;
        *p = c << 4;
        c = pat & 0x01 ? c_fgcol : c_bgcol;
        *p++ |= c;
    }

        int x = 0;

        /* marge gauche 40 pixels */
        for(int i=0;i<40;i++)
            out[x++] = 0xFF000000;


        /* pixels Oric */
        for(int i=0;i<120;i++)
        {
            uint8_t v = u->fb[i];


            uint8_t color = (v >> 4) & 0x07;
            out[x++] = rgbPalette[color];


            color = v & 0x07;
            out[x++] = rgbPalette[color];
        }


        /* marge droite 40 pixels */
        for(int i=0;i<40;i++)
            out[x++] = 0xFF000000;


/*
    int x = 0;
    int i;
    //il faut deux marges de 80 pixels de chaque coté donc 40
    for (i=0;i<40;i++) out[x++] = 0;
    
    for (int i = 0; i < 120; i++) {
        uint8_t v = u->fb[i];

        // pixel gauche (nibble haut)
        uint8_t color = (v >> 4) & 0x07;
        out[x++] = pal[color];

        // pixel droit (nibble bas)
        color = v & 0x07;
        out[x++] = pal[color];
    }
    for (i=0;i<40;i++) out[x++] = 0;
*/
    
    u->pattr = pattr;    
}



void ula_endframe(Ula* u)
{

    KeyboardEvent ev;


    if(keyboard_get_event(&ev))
    {
        linux_ps2_feed(ev.scan, ev.released);
    }


    ula_kbdScancode = linux_ps2_read();

    ula_nesValue = nes_get_state();
    ula_nesSt = 1;

    //clignotement ula a faire
    //u->blink_counter = (u->blink_counter + 1) & 0x3F; //valeur pour 50Hz
    u->blink_counter = (u->blink_counter + 1) & 0x1F; //adaptation pour 60hz

}

/*
D7      D6      D5              D4              D3          D2          D1          D0
A       B       SELECT          START           HAUT        BAS         GAUCHE      DROITE
INPUT_A INPUT_B INPUT_SELECT    INPUT_START     D-Pad Haut  D-Pad bas   D-Padgauche D-Pad droite

*/


