#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>


typedef struct
{
    uint8_t scan;
    bool released;

} KeyboardEvent;


int keyboard_init(const char *device);

int keyboard_get_event(KeyboardEvent *event);

void keyboard_flush(void);


#endif
