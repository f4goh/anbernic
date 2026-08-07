#ifndef VIDEO_H
#define VIDEO_H

#include <stdbool.h>

#include "ula.h"


bool video_init(Ula *u);

void video_shutdown(void);


/*
 * Appelé par le thread principal SDL
 */
void video_display(void);


#endif
