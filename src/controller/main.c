#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdbool.h>

static const char *button_name(int code)
{
    switch (code)
    {
        case 304: return "A";
        case 305: return "B";
        case 306: return "Y";
        case 307: return "X";

        case 308: return "L1";
        case 309: return "R1";

        case 314: return "L2";
        case 315: return "R2";

        case 310: return "SELECT";
        case 311: return "START";
        case 354: return "MENU";

        default:
            return NULL;
    }
}

int main(void)
{
    int fd = open("/dev/input/event1", O_RDONLY);

    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    printf("=== RG40XX H INPUT TEST ===\n");
    printf("CTRL+C pour quitter\n\n");

    int last_abs[64] = {0};

    struct input_event ev;

    while (1)
    {
        if (read(fd, &ev, sizeof(ev)) != sizeof(ev))
            continue;

        switch (ev.type)
        {
            /*
             * Boutons
             */
            case EV_KEY:
            {
                const char *name = button_name(ev.code);

                if (name)
                {
                    if (ev.value)
                        printf("%s PRESSED\n", name);
                    else
                        printf("%s RELEASED\n", name);
                }
                else
                {
                    printf("KEY code=%d value=%d\n",
                           ev.code,
                           ev.value);
                }

                break;
            }

            /*
             * Axes analogiques + D-Pad
             */
            case EV_ABS:

                if (last_abs[ev.code] == ev.value)
                    break;

                last_abs[ev.code] = ev.value;

                switch (ev.code)
                {
                    /*
                     * D-Pad
                     */
                    case 16:

                        if (ev.value == -1)
                            printf("DPAD LEFT\n");
                        else if (ev.value == 1)
                            printf("DPAD RIGHT\n");
                        else
                            printf("DPAD X CENTER\n");

                        break;

                    case 17:

                        if (ev.value == -1)
                            printf("DPAD UP\n");
                        else if (ev.value == 1)
                            printf("DPAD DOWN\n");
                        else
                            printf("DPAD Y CENTER\n");

                        break;

                    /*
                     * Stick gauche
                     */
                    case 2:
                        printf("LEFT STICK X = %5d\n", ev.value);
                        break;

                    case 3:
                        printf("LEFT STICK Y = %5d\n", ev.value);
                        break;

                    /*
                     * Stick droit
                     */
                    case 4:
                        printf("RIGHT STICK X = %5d\n", ev.value);
                        break;

                    case 5:
                        printf("RIGHT STICK Y = %5d\n", ev.value);
                        break;

                    default:
                        printf("ABS code=%d value=%d\n",
                               ev.code,
                               ev.value);
                }

                break;
        }

        fflush(stdout);
    }

    close(fd);

    return 0;
}
