#include "keyboard.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>


static int fd = -1;

static int keyboard_startup = 1;

int keyboard_init(const char *device)
{
    fd = open(device, O_RDONLY | O_NONBLOCK);

    if(fd < 0)
    {
        perror("keyboard open");
        return -1;
    }

    return 0;
}

void keyboard_flush(void)
{
    struct input_event ev;

    while(read(fd, &ev, sizeof(ev)) == sizeof(ev))
    {
        printf("FLUSH type=%d code=%d value=%d\n",
               ev.type,
               ev.code,
               ev.value);
    }
}




int keyboard_get_event(KeyboardEvent *event)
{
    struct input_event ev;

    while (read(fd, &ev, sizeof(ev)) == sizeof(ev))
    {
        if (ev.type == EV_KEY)
        {
            //printf("EV_KEY code=%d value=%d\n",ev.code,ev.value);

            
             //Ignore les répétitions automatiques
             
            if (ev.value == 2)
                continue;


            
            // Seulement press et release
             
            if (ev.value == 0 || ev.value == 1)
            {
                
                 // Ignore le release du ENTER qui lance ./oric-pc
                 
                if (keyboard_startup && ev.value == 0)
                {
                    //printf("IGNORE startup release code=%d\n",ev.code);

                    keyboard_startup = 0;
                    return 0;
                }


                keyboard_startup = 0;


                
                // Ici on utilise le code Linux
                 
                event->scan = ev.code;

                event->released = (ev.value == 0);


                //printf("SEND code=%d %s\n",event->scan,event->released ? "RELEASE" : "PRESS");


                return 1;
            }
        }
    }

    return 0;
}


