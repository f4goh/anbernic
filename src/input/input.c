#include "input.h"

#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <string.h>

static int fd = -1;

static bool buttons[INPUT_MAX];

static int dpad_x;
static int dpad_y;

static int left_x;
static int left_y;

static int right_x;
static int right_y;

bool input_init(void)
{
    memset(buttons, 0, sizeof(buttons));

    fd = open("/dev/input/event1", O_RDONLY | O_NONBLOCK);

    return fd >= 0;
}

void input_close(void)
{
    if (fd >= 0)
        close(fd);
}

void input_update(void)
{
    if (fd < 0)
        return;

    struct input_event ev;

    while (read(fd, &ev, sizeof(ev)) == sizeof(ev))
    {
        if (ev.type == EV_KEY)
        {
            bool pressed = (ev.value != 0);

            switch (ev.code)
            {
                case 304: buttons[INPUT_A] = pressed; break;
                case 305: buttons[INPUT_B] = pressed; break;
                case 306: buttons[INPUT_Y] = pressed; break;
                case 307: buttons[INPUT_X] = pressed; break;

                case 308: buttons[INPUT_L1] = pressed; break;
                case 309: buttons[INPUT_R1] = pressed; break;

                case 314: buttons[INPUT_L2] = pressed; break;
                case 315: buttons[INPUT_R2] = pressed; break;

                case 310: buttons[INPUT_SELECT] = pressed; break;
                case 311: buttons[INPUT_START] = pressed; break;
                case 354: buttons[INPUT_MENU] = pressed; break;
            }
        }

        else if (ev.type == EV_ABS)
        {
            switch (ev.code)
            {
                case 16:
                    dpad_x = ev.value;
                    break;

                case 17:
                    dpad_y = ev.value;
                    break;

                case 2:
                    left_x = ev.value;
                    break;

                case 3:
                    left_y = ev.value;
                    break;

                case 4:
                    right_x = ev.value;
                    break;

                case 5:
                    right_y = ev.value;
                    break;
            }
        }
    }
}

bool input_down(InputButton b)
{
    return buttons[b];
}

int input_dpad_x(void)
{
    return dpad_x;
}

int input_dpad_y(void)
{
    return dpad_y;
}

int input_left_x(void)
{
    return left_x;
}

int input_left_y(void)
{
    return left_y;
}

int input_right_x(void)
{
    return right_x;
}

int input_right_y(void)
{
    return right_y;
}
