#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

#define SERIAL_DEVICE "/dev/ttyS5"
#define LED_COUNT 8

/* ============================================================
 * RGB LED - RG40XX H / MuOS
 *
 * Joystick DROIT : 8 RGB
 * Joystick GAUCHE: 8 RGB
 *
 * Fonction publique :
 *
 *   rgb_joysticks(
 *       right_r, right_g, right_b,
 *       left_r,  left_g,  left_b,
 *       brightness
 *   );
 *
 * Toutes les valeurs RGB et brightness : 0..255
 * ============================================================ */


/* ------------------------------------------------------------
 * Checksum protocole MCU
 * ------------------------------------------------------------ */
static uint8_t rgb_checksum(const uint8_t *data, size_t len)
{
    unsigned int sum = 0;

    for (size_t i = 0; i < len; i++)
        sum += data[i];

    return (uint8_t)(sum & 0xff);
}


/* ------------------------------------------------------------
 * Configuration UART
 * ------------------------------------------------------------ */
static int rgb_serial_open(void)
{
    int fd = open(SERIAL_DEVICE, O_WRONLY | O_NOCTTY);

    if (fd < 0) {
        perror("open " SERIAL_DEVICE);
        return -1;
    }

    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        perror("tcgetattr");
        close(fd);
        return -1;
    }

    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    tty.c_cflag |= CLOCAL | CREAD;

    tty.c_cflag &= ~(PARENB | CSTOPB | CRTSCTS);

    tty.c_iflag = 0;
    tty.c_oflag = 0;
    tty.c_lflag = 0;

    if (tcsetattr(fd, TCSANOW, &tty) < 0) {
        perror("tcsetattr");
        close(fd);
        return -1;
    }

    return fd;
}


/* ------------------------------------------------------------
 * Envoi du paquet
 * ------------------------------------------------------------ */
static int rgb_send_packet(const uint8_t *packet, size_t len)
{
    int fd = rgb_serial_open();

    if (fd < 0)
        return -1;

    ssize_t written = write(fd, packet, len);

    if (written != (ssize_t)len) {
        perror("write RGB packet");
        close(fd);
        return -1;
    }

    tcdrain(fd);
    close(fd);

    return 0;
}


/* ------------------------------------------------------------
 * Fonction principale réutilisable
 *
 * right_* : couleur joystick DROIT
 * left_*  : couleur joystick GAUCHE
 * brightness : luminosité générale
 *
 * Retour :
 *    0  = OK
 *   -1  = erreur
 * ------------------------------------------------------------ */
int rgb_joysticks(
    uint8_t right_r,
    uint8_t right_g,
    uint8_t right_b,

    uint8_t left_r,
    uint8_t left_g,
    uint8_t left_b,

    uint8_t brightness)
{
    uint8_t packet[51];
    size_t p = 0;

    /* Mode 1 = solid */
    packet[p++] = 1;

    /* Luminosité */
    packet[p++] = brightness;


    /* --------------------------------------------------------
     * 8 LEDs du joystick DROIT
     * -------------------------------------------------------- */
    for (int i = 0; i < LED_COUNT; i++) {
        packet[p++] = right_r;
        packet[p++] = right_g;
        packet[p++] = right_b;
    }


    /* --------------------------------------------------------
     * 8 LEDs du joystick GAUCHE
     * -------------------------------------------------------- */
    for (int i = 0; i < LED_COUNT; i++) {
        packet[p++] = left_r;
        packet[p++] = left_g;
        packet[p++] = left_b;
    }


    /* --------------------------------------------------------
     * Checksum
     *
     * IMPORTANT :
     * calculer AVANT p++
     * -------------------------------------------------------- */
    uint8_t checksum = rgb_checksum(packet, p);

    packet[p++] = checksum;


    return rgb_send_packet(packet, p);
}


/* ============================================================
 * Fonctions pratiques
 * ============================================================ */

int rgb_off(void)
{
    return rgb_joysticks(
        0, 0, 0,       /* droite */
        0, 0, 0,       /* gauche */
        0               /* luminosité */
    );
}


int rgb_all(
    uint8_t r,
    uint8_t g,
    uint8_t b,
    uint8_t brightness)
{
    return rgb_joysticks(
        r, g, b,
        r, g, b,
        brightness
    );
}


/* ============================================================
 * Affichage aide
 * ============================================================ */

static void usage(const char *program)
{
    printf(
        "\n"
        "RGB RG40XX H\n"
        "\n"
        "Usage:\n"
        "  %s R G B BRIGHTNESS\n"
        "      Les deux joysticks même couleur\n"
        "\n"
        "  %s RR RG RB LR LG LB BRIGHTNESS\n"
        "      Couleurs séparées\n"
        "\n"
        "  %s off\n"
        "      Éteindre\n"
        "\n"
        "Exemples:\n"
        "  %s 255 0 0 255\n"
        "      Rouge partout, luminosité 255\n"
        "\n"
        "  %s 255 0 0 128\n"
        "      Rouge partout, luminosité 128\n"
        "\n"
        "  %s 255 0 0 0 0 255 255\n"
        "      Droite rouge / gauche bleu\n"
        "\n",
        program,
        program,
        program,
        program,
        program,
        program
    );
}


/* ============================================================
 * MAIN
 * ============================================================ */

int main(int argc, char **argv)
{
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }


    /* --------------------------------------------------------
     * OFF
     * -------------------------------------------------------- */
    if (strcmp(argv[1], "off") == 0) {

        if (argc != 2) {
            usage(argv[0]);
            return 1;
        }

        return rgb_off() == 0 ? 0 : 1;
    }


    /* --------------------------------------------------------
     * 4 arguments :
     *
     * R G B BRIGHTNESS
     *
     * Même couleur aux deux joysticks
     * -------------------------------------------------------- */
    if (argc == 5) {

        int r = atoi(argv[1]);
        int g = atoi(argv[2]);
        int b = atoi(argv[3]);
        int brightness = atoi(argv[4]);

        if (r < 0 || r > 255 ||
            g < 0 || g > 255 ||
            b < 0 || b > 255 ||
            brightness < 0 || brightness > 255) {

            fprintf(stderr,
                    "Erreur: toutes les valeurs doivent être entre 0 et 255\n");

            return 1;
        }

        return rgb_all(
            (uint8_t)r,
            (uint8_t)g,
            (uint8_t)b,
            (uint8_t)brightness
        ) == 0 ? 0 : 1;
    }


    /* --------------------------------------------------------
     * 7 arguments :
     *
     * RR RG RB LR LG LB BRIGHTNESS
     *
     * Couleurs indépendantes
     * -------------------------------------------------------- */
    if (argc == 8) {

        int rr = atoi(argv[1]);
        int rg = atoi(argv[2]);
        int rb = atoi(argv[3]);

        int lr = atoi(argv[4]);
        int lg = atoi(argv[5]);
        int lb = atoi(argv[6]);

        int brightness = atoi(argv[7]);

        if (rr < 0 || rr > 255 ||
            rg < 0 || rg > 255 ||
            rb < 0 || rb > 255 ||
            lr < 0 || lr > 255 ||
            lg < 0 || lg > 255 ||
            lb < 0 || lb > 255 ||
            brightness < 0 || brightness > 255) {

            fprintf(stderr,
                    "Erreur: toutes les valeurs doivent être entre 0 et 255\n");

            return 1;
        }

        return rgb_joysticks(
            (uint8_t)rr,
            (uint8_t)rg,
            (uint8_t)rb,

            (uint8_t)lr,
            (uint8_t)lg,
            (uint8_t)lb,

            (uint8_t)brightness
        ) == 0 ? 0 : 1;
    }


    usage(argv[0]);
    return 1;
}
