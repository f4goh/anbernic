#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

char *keymap[256] =
{
    /* Lettres */
    [KEY_A]="a",
    [KEY_B]="b",
    [KEY_C]="c",
    [KEY_D]="d",
    [KEY_E]="e",
    [KEY_F]="f",
    [KEY_G]="g",
    [KEY_H]="h",
    [KEY_I]="i",
    [KEY_J]="j",
    [KEY_K]="k",
    [KEY_L]="l",
    [KEY_M]="m",
    [KEY_N]="n",
    [KEY_O]="o",
    [KEY_P]="p",
    [KEY_Q]="q",
    [KEY_R]="r",
    [KEY_S]="s",
    [KEY_T]="t",
    [KEY_U]="u",
    [KEY_V]="v",
    [KEY_W]="w",
    [KEY_X]="x",
    [KEY_Y]="y",
    [KEY_Z]="z",

    /* Chiffres */
    [KEY_0]="0",
    [KEY_1]="1",
    [KEY_2]="2",
    [KEY_3]="3",
    [KEY_4]="4",
    [KEY_5]="5",
    [KEY_6]="6",
    [KEY_7]="7",
    [KEY_8]="8",
    [KEY_9]="9",

    /* Espace et contrôle */
    [KEY_SPACE]=" ",
    [KEY_ENTER]="\n",
    [KEY_BACKSPACE]="\b",
    [KEY_TAB]="\t",

    /* Symboles */
    [KEY_MINUS]="-",
    [KEY_EQUAL]="=",
    [KEY_LEFTBRACE]="[",
    [KEY_RIGHTBRACE]="]",
    [KEY_SEMICOLON]=";",
    [KEY_APOSTROPHE]="'",
    [KEY_GRAVE]="`",
    [KEY_BACKSLASH]="\\",
    [KEY_COMMA]=",",
    [KEY_DOT]=".",
    [KEY_SLASH]="/",

    /* Flèches */
    [KEY_UP]="<UP>",
    [KEY_DOWN]="<DOWN>",
    [KEY_LEFT]="<LEFT>",
    [KEY_RIGHT]="<RIGHT>"
};

int main()
{
    int fd;
    struct input_event ev;

    fd = open("/dev/input/event3", O_RDONLY);

    if (fd < 0) {
        perror("Erreur ouverture clavier");
        return 1;
    }

    printf("Clavier Bluetooth actif...\n");
    printf("Tape 'q' pour quitter\n\n");

      while(1)
    {
        if(read(fd, &ev, sizeof(ev)) == sizeof(ev))
        {
            if(ev.type == EV_KEY && ev.value == 1)
            {
                if(ev.code < 256 && keymap[ev.code])
                {
                    printf("%s", keymap[ev.code]);
                    if (keymap[ev.code][0]=='q'){
                        printf("\nQuitter\n");
                        break;
                    }
                }
                else
                {
                    printf("[code=%d]", ev.code);
                }

                fflush(stdout);
            }
        }
    }

    close(fd);

    return 0;
}
