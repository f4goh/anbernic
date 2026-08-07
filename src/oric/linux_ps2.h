#ifndef LINUX_PS2_H
#define LINUX_PS2_H

#include <stdint.h>
#include <stdbool.h>


void linux_ps2_feed(uint16_t code, bool released);


/*
    récupère un octet PS2
*/
uint8_t linux_ps2_read(void);
void linux_ps2_init(void);


#endif
