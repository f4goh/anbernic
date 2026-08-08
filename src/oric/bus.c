/*
   scp -r oric-arm64  root@192.168.1.43:/run/muos/storage/application/oric/

 * Adaptation MOS6522 VIA
 * Version simplifiée IRQ unique
// 0300-03FF I/O Area
// 0300-030F Internal VIA 6522
// 0310-0310 DK'tronics Joystick Interface (left port)
// 0310-0313 Microdisc FDC WD1793
// 0310-031F Pravetz FDC
// 0314-031B Microdisc additionnal I/O registers
// 031C-031F Internal ACIA 6551 (Telestrat)
// 0320-032F RS232 extension (Atmos)
// 0320-0320 DK'tronics Joystick Interface (right port)
// 0320-032F Second VIA 6522 (Telestrat)
// 0320-03FF Pravetz ROM
// 0330-035F Spare Memory
// 0360-0371 RTC ICM7170 (Telestrat and Atmos)
// 0380-03DF Spare Memory
// 03E0-03E1 Oric Lightpen
// 03E2-03F3 Spare Memory
// 03F4-03FF Jasmin FDC WD1773
 pc info demarrage:
INFO: Nombre de drivers audio : 7
INFO: Driver audio courant : pulseaudio
SDL: create window
window=0x6456f1540600 error=
Renderer : opengl
Flags    : 10
renderer=0x6456f172cb40 error=
Input OK
INFO: AY audio : 22050 Hz
anbernic info demarrage
NFO: Nombre de drivers audio : 1
INFO: Driver audio courant : alsa
SDL: create window
window=0x5597bfd6f0 error=
Renderer : opengles2
Flags    : 10
renderer=0x5597c1ac10 error=
Input OK
INFO: AY audio : 22050 Hz


 */

#include "vrEmu6502.h"

#define CHIPS_IMPL
#include "via.h"
#include "ula.h"

#include "audio.h"
#include "video.h"

#include "config.h"
#include "bus.h"
#include "oric_keyboard.h"  

#include "timing.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "input.h"
#include "linux_ps2.h"

//#include "DesassembleurWrapper.h"
#include "sd_reader.h"
#include "autocmd.h"
//#include "6551.h"


// --------------------------------------------------
// RAM / ROM
// --------------------------------------------------


uint8_t  ram[ORIC_RAM_SIZE];
uint8_t  ramMoni[ORIC_MONITOR_SIZE];

extern uint8_t oricRom[];

uint8_t* romPtr()
{
    return oricRom;
}

size_t romSize()
{
    return ORIC_ROM_SIZE;
}

uint8_t buffer[64*1024];
TAPHeader tap;
tap_headers_t tapHeaders;
uint8_t indexTap=0;
size_t bytes_read = 0;
size_t nb_files = 0;
char tap_filenames[MAX_TAP_FILES][MAX_NAME_LEN];
uint8_t stateCload=0;
uint16_t cptByteTap=0;
uint16_t startByteTap=0;
uint16_t sizeByteTap=0;

uint8_t ps2_release = 0;
uint16_t dump_addr_state = 0;
uint16_t disasm_addr_state = 0x0600;
CommandType witchDump=CMD_NONE;
uint32_t infoCpt=0;
uint8_t infoFlag=0;
const char *srcFilename;
 
absolute_time_t currentTime;

absolute_time_t nextVideo;

// --------------------------------------------------
// CPU / VIA
// --------------------------------------------------

static VrEmu6502* cpu = NULL;
struct via vi;
//struct acia acia = {0};
 
// IRQ unique
static bool cpuIrq = false;
static bool nmi_request = false;

Ula ula;
bool quit=false;

// --------------------------------------------------

#define ORIC_CLOCK_FREQ_MHZ 1.0000

#define MICROSECONDS_PER_BURST 50

#define TICKS_PER_BURST \
    (int)(MICROSECONDS_PER_BURST * ORIC_CLOCK_FREQ_MHZ)

#define CPU_6502_WAI 0xCB


#define  CPU_SNAP_SIZE 15
#define  VIA_SNAP_SIZE 37
#define  PSG_SNAP_SIZE 112

// --------------------------------------------------

void busWrite(uint16_t addr, uint8_t val);

uint8_t busRead(uint16_t addr, bool isDbg);

void cload();
void csave();

void updateVideo();

void snapSave();
void snapLoad();

/*
void moniDump();
bool ps2_is_press(uint8_t sc);
char sc_to_ascii(uint8_t sc);
void cmd_draw(const CmdLine *c);
bool cmd_feed(CmdLine *c, uint8_t sc);
bool cmd_parse(const char *txt, Command *cmd);
const char* skip_spaces(const char *s);
void cmd_error_draw(void);
void cmd_error_clear(void);
void cmd_compact(char *dst, const char *src);
char hexdigit(uint8_t v);
void dumpRam();
void dumpDisasm(void);
void execute_command(const Command *cmd);
void bmpSave(const char* filename);
*/




// --------------------------------------------------
// INIT
// --------------------------------------------------


void busInit()
{

    memset(ram,0,sizeof(ram));

    config_load("oric.cfg");

    ula_init(&ula);
    ula_reset(&ula,0);


    if(!video_init(&ula))
        return ;

    if(!input_init())
    {
        printf("Input non disponible\n");
    }
    else
    {
        printf("Input OK\n");
    }

    
    // CPU     
    cpu = vrEmu6502New(
        CPU_6502,
        busRead,
        busWrite
    );

    // VIA
    via_init(&vi);
    //acia_init(&acia);

    // Audio / Video

    audioInit(
        AY38910_CLOCK,
        SAMPLE_FREQUENCY
    );

    //videoInit();
    //uart_init(uart0, 115200);
    //gpio_set_function(16, GPIO_FUNC_UART);
    oric_keyboard_init();
   
     oricRom[0xE4AC & (ORIC_ROM_SIZE - 1)]=0xEA; //nop bande amorce
     oricRom[0xE4AD & (ORIC_ROM_SIZE - 1)]=0xEA;
     oricRom[0xE4AE & (ORIC_ROM_SIZE - 1)]=0xEA;
     
     oricRom[0xE6C9 & (ORIC_ROM_SIZE - 1)]=0xA9; //lda xx
     oricRom[0xE6CA & (ORIC_ROM_SIZE - 1)]=0x00; //octet a cet endroit
     oricRom[0xE6CB & (ORIC_ROM_SIZE - 1)]=0x60;

     oricRom[0xE92C & (ORIC_ROM_SIZE - 1)]=0x4C; //jmp $E93B
     oricRom[0xE92D & (ORIC_ROM_SIZE - 1)]=0x3B; //pour csave
     oricRom[0xE92E & (ORIC_ROM_SIZE - 1)]=0xE9;    
    
     
}

void clavier()
{
    uint8_t sc = ula_kbdScancode;

    if(!sc)
        return;

    switch(sc)
    {
        case 0x05:
            nmi_request=true;
            break;
        case 0x06:
            vrEmu6502Reset(cpu);
            via_init(&vi);
            break;
        default:
            //printf(" %x\n",sc);
            oric_keyboard_process(sc);
            break;
    }
    ula_kbdScancode=0;
}


void manetteNes() {

    uint8_t nesSt = ula_nesSt;
    uint8_t nesValue = ula_nesValue;

    if (nesSt==0) {
        //gpio_put(17, 1);
        return;
    }
    oric_keyboard_nes(nesValue);
 
    ula_nesSt = 0; // consommé

}

// --------------------------------------------------
// MAIN LOOP
// --------------------------------------------------

void busMainLoop()
{
    vrEmu6502Reset(cpu);

     currentTime = get_absolute_time();

     nextVideo = currentTime;

    int i = 0;


    while(!quit)
    {
    // =====================================================
    // PHASE INPUT / PERIPHERIQUES
    // =====================================================
    // Lecture clavier à chaque frame
    clavier();
    manetteNes();
    SDL_Event e;


        while(SDL_PollEvent(&e))
        {
            if(e.type==SDL_QUIT)
                quit=true;


            if(e.type==SDL_KEYDOWN)
            {
                if(e.key.keysym.sym==SDLK_F12)
                    quit=true;
                if(e.key.keysym.sym==SDLK_F5)
                  autoCmd_start();
                if(e.key.keysym.sym==SDLK_F6)
                  snapSave();
                if(e.key.keysym.sym==SDLK_F7)
                  snapLoad();
            }

        }

        input_update();

        if(input_down(INPUT_L1))
        {
            snapSave();
        }
        if(input_down(INPUT_R1))
        {
            snapLoad();
        }

        if(input_down(INPUT_START))
        {
            autoCmd_start();
        }

        if(input_down(INPUT_SELECT))
        {
            vrEmu6502Reset(cpu);
            via_init(&vi);
        }

        if(input_down(INPUT_MENU))
        {
            quit=true;
        }

        if (infoFlag == 0) {
            if (infoCpt == 50000) {
                memcpy(ram + 0xbb80 + 40 * 3 + 2, "\x03\x14ORIC-EMU F4GOH 2026\x17\x00", 23);
                infoFlag = 1;
                ram[0x2f5] = 0x00;
                ram[0x2f6] = 0x80;
                /*
                ram[0x8000] = 0x20;
                ram[0x8001] = 0xe2;
                ram[0x8002] = 0x00;

                ram[0x8003] = 0x20;
                ram[0x8004] = 0xe2;
                ram[0x8005] = 0x00;

                ram[0x8006] = 0x20;
                ram[0x8007] = 0xe2;
                ram[0x8008] = 0x00;
                */
                
                ram[0x8000] = 0x60;
                
                //gpio_put(25, 1);
            }
            infoCpt += 1;
        }
    
        // ------------------------------------------
        // CPU
        // ------------------------------------------

      // ------------------------------------------
// CPU + VIA synchronisés
// ------------------------------------------

        while (i < TICKS_PER_BURST)
        {
            int cycleTicks =
                vrEmu6502InstCycle(cpu);


            /*
               La VIA avance exactement avec
               les cycles consommés par le CPU
            */
            via_clock(&vi, cycleTicks);


            i += cycleTicks;


            if (vrEmu6502GetCurrentOpcode(cpu)
                == CPU_6502_WAI)
            {
                /*
                   CPU arrêté mais périphériques actifs
                */
                int remaining =
                    TICKS_PER_BURST - i;

                if(remaining > 0)
                    via_clock(&vi, remaining);

                i = TICKS_PER_BURST;
                break;
            }


            /*
               IRQ VIA
            */
            cpuIrq = (vi.ifr & 0x80) != 0;

            *(vrEmu6502Int(cpu)) =
                cpuIrq
                ? IntRequested
                : IntCleared;
        }


        i -= TICKS_PER_BURST;
       
                
        //acia_clock(&acia, TICKS_PER_BURST);

    // =====================================================
    // NMI (GPIO18 externe)
    // =====================================================
    // Ligne indépendante de l’IRQ (non masquable)
    *(vrEmu6502Nmi(cpu)) =
        nmi_request ? IntRequested : IntCleared;

    // Reset du latch NMI après prise en compte CPU
    nmi_request = false;

    
        updateVideo();
/*
         
        static int frame = 0;

        frame++;

        if (frame == 60)
        {
            frame = 0;

            printf("current=%lld next=%lld\n",
                to_us_since_boot(currentTime),
                to_us_since_boot(get_absolute_time()));
        }
*/

       
    }
    video_shutdown();
}

void updateVideo(void)
{
    absolute_time_t now = get_absolute_time();

    // ============================
    // VIDEO
    // ============================

    if (time_reached(nextVideo))
    {
        video_display();
        autoCmd();
        // repartir de l'heure actuelle
        nextVideo = delayed_by_us(now, 16667);
    }

    // ============================
    // CPU timing
    // ============================

    currentTime = delayed_by_us(currentTime,
                                MICROSECONDS_PER_BURST);

    now = get_absolute_time();

    if (!time_reached(currentTime))
    {
        busy_wait_until(currentTime);
    }
    else
    {
        // on est en retard, on repart de maintenant
        currentTime = now;
    }
}


// --------------------------------------------------
// BUS WRITE
// --------------------------------------------------

void busWrite(
    uint16_t addr,
    uint8_t val)
{
    if (addr < 0xC000) {
        // ------------------------------------------
        // VIA
        // ------------------------------------------

        if (addr >= 0x0300 &&
                addr < 0x0310) {

            via_write(
                    &vi,
                    addr & 0x0F,
                    val
                    );
            cpuIrq = (vi.ifr & 0x80) != 0;
        }
        //if (addr >= 0x031C &&     //031C-031F
        //        addr < 0x0320) {
        //    acia_write(&acia,addr & 0x03,val);
        //} 
       else {
            // --------------------------------------
            // RAM
            // --------------------------------------

            ram[addr] = val;
        }
    }
    
}

// --------------------------------------------------
// BUS READ
// --------------------------------------------------

uint8_t busRead(
        uint16_t addr,
        bool isDbg) {
    // ------------------------------------------
    // ROM
    // ------------------------------------------

    if (addr >= 0xC000) {
        switch (stateCload) {
            case 0:
                if (addr == 0xE4AC) { //1re bande amorce au lieu de cload
                    cload();                    
                }
                break;
            case 1:
                if (addr == 0xE6C9) { //lecture de l'octet
                     oricRom[0xE6CA & (ORIC_ROM_SIZE - 1)]=buffer[startByteTap+cptByteTap];
                     cptByteTap++;
                     if (cptByteTap==sizeByteTap){
                         indexTap++;
                         //
                         if (indexTap==tapHeaders.nbHeader){
                           stateCload = 0;//retour au cload menu
                           //gpio_put(25, 1);
                           indexTap=0;
                           ram[0x2b1]=0;
                         }
                         else{ //il y a une autre partie
                           stateCload = 2;
                           ram[0x2b1]=0; //supprime les erreurs ??
                         }
                     }
                }
                break;
             case 2:
                if (addr == 0xE4AC) { //amorce
                    cptByteTap = 0; //reinit cpt
                    startByteTap = tapHeaders.bloc[indexTap].atSync; //nouvelle partie
                    sizeByteTap = tapHeaders.bloc[indexTap].dataFullSize;
                    stateCload = 1;                    
                }
                break;
        }
        if (addr == 0xE92C){
           // csave();
        }

        return oricRom[
                addr &
                (ORIC_ROM_SIZE - 1)
                ];
    }

    // ------------------------------------------
    // VIA
    // ------------------------------------------

    if (addr >= 0x0300 &&
            addr < 0x0310) {
        uint8_t value =
                via_read(
                &vi,
                addr & 0x0F
                );
        cpuIrq = (vi.ifr & 0x80) != 0;
        return value;
    }
/*
    if (addr == 0x02f5){
           
           gpio_put(25, gpio_get(25) ^ 1);
    }
  */  
    //if (addr >= 0x031C && //031C-031F
    //        addr < 0x0320) {
    //    uint8_t value = acia_read(&acia, addr & 0x03);
    //    return value;
    //} 

    // ------------------------------------------
    // RAM
    // ------------------------------------------

    return ram[addr];
}

void cload() {
    int ret;
    if (indexTap == 0) {

        ret = list_tap_files(tap_filenames, MAX_TAP_FILES, &nb_files);
        menuColor(ram);

        print_files_ram(tap_filenames, nb_files, ram);
        keyboard_flush();
        deplace_curseur(0, nb_files, ram);
        uint8_t indexFile=0;
        uint8_t nesOldValue = 0xff;
        uint8_t kbdOldValue = ula_kbdScancode;

        /* historique clavier */
        uint8_t kbdHistory[3] = {0, 0, 0};

        /* état touches (anti-retrigger) */
        uint8_t rightPressed = 0;
        uint8_t leftPressed = 0;

        ula_kbdScancode=0;
        uint8_t nesValue;
        /* boucle principale */
        while (ula_nesValue != 0x7f && ula_kbdScancode != 0x29) {
            //ula_endframe(&ula);
            //printf("boucle");
            input_update();
            updateVideo();
            /* ---------------- NES ---------------- */
            if (ula_nesSt != 0) {
                nesValue = ula_nesValue;
                if (nesValue != nesOldValue) {

                    if (nesValue == 0xfe) {
                        indexFile = deplace_curseur(1, nb_files, ram);
                    } else if (nesValue == 0xfd) {
                        indexFile = deplace_curseur(-1, nb_files, ram);
                    }

                    nesOldValue = nesValue;
                }
                ula_nesSt = 0;
            }

            /* ---------------- CLAVIER ---------------- */
            if (ula_kbdScancode != kbdOldValue) {

                /* mise à jour historique uniquement si changement réel */
                kbdHistory[0] = kbdHistory[1];
                kbdHistory[1] = kbdHistory[2];
                kbdHistory[2] = ula_kbdScancode;

                /* -------- droite (0x74) -------- */

                if (kbdHistory[2] == 0x74 && kbdHistory[1] != 0xF0) {

                    if (!rightPressed) {
                        rightPressed = 1;
                        indexFile = deplace_curseur(1, nb_files, ram);
                         //updateVideo();
                    }
                }

                if (kbdHistory[2] == 0x74 && kbdHistory[1] == 0xF0) {
                    rightPressed = 0;
                }

                /* -------- gauche (0x6B) -------- */

                if (kbdHistory[2] == 0x6B && kbdHistory[1] != 0xF0) {

                    if (!leftPressed) {
                        leftPressed = 1;
                        indexFile = deplace_curseur(-1, nb_files, ram);
                         //updateVideo();
                    }
                }

                if (kbdHistory[2] == 0x6B && kbdHistory[1] == 0xF0) {
                    leftPressed = 0;
                }

                kbdOldValue = ula_kbdScancode;
            }
        }
        ula_nesSt = 0;
        ula_kbdScancode = 0;
        oric_keyboard_process(0x66);
        //const char *filename = "mushmania";
        srcFilename = tap_filenames[indexFile];

        char fullpath[32];

        /* construit "mushmania.tap" */
        snprintf(fullpath,
                sizeof (fullpath),
                "%s.tap",
                srcFilename);

        ret = read_binary_file(fullpath, buffer, sizeof (buffer), &bytes_read);
        if (!ret) {
            load_csv_u8(srcFilename, tableNes);
            analyseHeader(buffer, bytes_read, &tapHeaders);
        }
        stateCload = 1;
        cptByteTap = 0;
        startByteTap = tapHeaders.bloc[indexTap].atSync;
        sizeByteTap = tapHeaders.bloc[indexTap].dataFullSize;
        clearKeys();
    }
}

void csave() {

    tap_header_t *h = &tapHeaders.header[0];

    // 0x2A8 -> 0x2B0
    h->reserved = ram[0x2A8];
    h->start_addr_lo = ram[0x2A9];
    h->start_addr_hi = ram[0x2AA];
    h->end_addr_lo = ram[0x2AB];
    h->end_addr_hi = ram[0x2AC];
    h->autorun = ram[0x2AD];
    h->type = ram[0x2AE];
    h->flag_str = ram[0x2AF];
    h->flag_int = ram[0x2B0];

    memset(h->filename, 0, sizeof (h->filename));

    if (ram[0x27F] == 0) {
        strcpy((char*) h->filename, "save");
    } else {
        int i;

        for (i = 0; i < sizeof (h->filename) - 1; i++) {
            h->filename[i] = ram[0x27F + i];

            if (h->filename[i] == 0)
                break;
        }

        h->filename[sizeof (h->filename) - 1] = 0;
    }
    sd_write_binary(ram,&tapHeaders);
    clearKeys();
}

void snapSave()
{
  uint8_t cpus=cpuSize();
  uint8_t vias=viaSize();
  uint8_t audios=audioSize();


    uint8_t snapBuffer[cpus+vias+audios+1+15];

    uint8_t* p = snapBuffer;
    memcpy(p,"CPU", 3);
    p+=3;
    memcpy(p,(uint8_t *)cpu,cpus);
    p+=cpus;
    memcpy(p,"VIA", 3);
    p+=3;
    memcpy(p, (uint8_t *)&vi, vias);
    p += vias;
    memcpy(p,"PSG", 3);
    p+=3;
    memcpy(p, audioSave(),  audios);
    p +=  audios;
    memcpy(p,"ULA", 3);
    p+=3;
    *p=ula.pattr;
    p++;
    memcpy(p,"RAM", 3);
    p+=3;


    
    char fullpath[32];

    if (srcFilename == NULL || srcFilename[0] == '\0')
    {
        srcFilename = "save";
    }
    //snprintf(fullpath,sizeof (fullpath),"%s.sna",srcFilename);


   sd_save_snap(srcFilename,snapBuffer,cpus+vias+audios+16,(uint8_t *)ram,ORIC_RAM_SIZE);
    
    
}

void snapLoad()
{
    uint8_t cpus = cpuSize();
    uint8_t vias = viaSize();
    uint8_t audios = audioSize();

    uint8_t snapBuffer[cpus + vias + audios + 1+15];

    char fullpath[32];

    if (srcFilename == NULL || srcFilename[0] == '\0')
    {
        srcFilename = "save";
    }
    //snprintf(fullpath,sizeof (fullpath),"%s.sna",srcFilename);


    sd_load_snap(srcFilename,
                 snapBuffer,
                 cpus + vias + audios + 1+15,
                 ram,
                 ORIC_RAM_SIZE);

    uint8_t* p = snapBuffer;

    // ---- CPU ----
    p += 3; // skip "CPU"
    memcpy(cpu, p, cpus);
    p += cpus;

    // ---- VIA ----
    p += 3; // skip "VIA"
    memcpy(&vi, p, vias);
    p += vias;

    // ---- PSG ----
    p += 3; // skip "PSG"
    memcpy(audioSave(), p, audios);
    p += audios;
    p += 3; // skip "ULA"
    ula_reset(&ula,*p);
}

