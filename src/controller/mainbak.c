#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>

int main(void)
{
    int fd = open("/dev/input/js0", O_RDONLY);

    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    printf("Joystick ouvert\n");

    struct js_event e;

    while (read(fd, &e, sizeof(e)) == sizeof(e))
    {
        e.type &= ~JS_EVENT_INIT;

        switch (e.type)
        {
            case JS_EVENT_BUTTON:
                printf("BUTTON %u value=%d\n",
                       e.number,
                       e.value);
                break;

            case JS_EVENT_AXIS:
                printf("AXIS %u value=%d\n",
                       e.number,
                       e.value);
                break;
        }

        fflush(stdout);
    }

    close(fd);
    return 0;
}
