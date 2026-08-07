#include "ula.h"


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stddef.h>
#include "sd_reader.h"

#define SD_CS 17
#define LED_PIN 25

// Variable globale
uint8_t indexFichier = 0;




int sd_read_binary(const char *filename)
{
    FILE *f = fopen(filename, "rb");

    if (!f)
        return -1;

    uint8_t buffer[512];

    while (fread(buffer, 1, sizeof(buffer), f) > 0)
        ;

    fclose(f);

    return 0;
}




uint16_t lireTap(const char *filename, uint8_t *buffer)
{
    FILE *f = fopen(filename, "rb");

    if (f == NULL)
    {
        perror(filename);
        return 0;
    }

    /* Taille du fichier */
    if (fseek(f, 0, SEEK_END) != 0)
    {
        fclose(f);
        return 0;
    }

    long fileSize = ftell(f);

    if (fileSize < 0)
    {
        fclose(f);
        return 0;
    }

    rewind(f);

    if (fileSize > (48 * 1024))
    {
        printf("file too big\n");
        fclose(f);
        return 0;
    }

    size_t br = fread(buffer, 1, fileSize, f);

    if (br != (size_t)fileSize)
    {
        printf("read error\n");
        fclose(f);
        return 0;
    }

    printf("read ok : %zu bytes\n", br);

    fclose(f);

    return (uint16_t)fileSize;
}


bool parse_tap_header(const uint8_t *buffer,
                       uint32_t fileSize,
                       TAPHeader *header)
{
    memset(header, 0, sizeof(TAPHeader));

    uint32_t pos = 0;

    // sync
    while (buffer[pos] == 0x16) pos++;

    if (buffer[pos] != 0x24) return false;
    pos++;

    pos += 2; // reserved

    header->language = buffer[pos++];
    header->autostart = buffer[pos++];

    header->endAddress =
        (buffer[pos] << 8) | buffer[pos + 1];
    pos += 2;

    header->startAddress =
        (buffer[pos] << 8) | buffer[pos + 1];
    
    // ✔️ ASTUCE ULTRA FIABLE
    uint32_t codeSize =
        (header->endAddress - header->startAddress) + 1;

    header->offsetCode = fileSize - codeSize;

    header->sizeCode = codeSize;

    return true;
}

/*
 * Recherche la séquence :
 *   0x16 0x16 0x16 0x24
 *
 * Paramètres :
 *   buffer : buffer à analyser
 *   size   : taille du buffer
 *   start  : position de départ de la recherche
 *
 * Retour :
 *   position du premier octet après 0x24
 *   ou 0xFFFF si non trouvé
 */
uint16_t tap_seek(const uint8_t *buffer, size_t size, uint16_t start)
{
    /* Vérification minimale */
    if (buffer == NULL || start >= size)
    {
        return TAP_NOT_FOUND;
    }

    /* Il faut au moins 4 octets à comparer */
    for (size_t i = start; i + 3 < size; i++)
    {
        if (buffer[i]     == 0x16 &&
            buffer[i + 1] == 0x16 &&
            buffer[i + 2] == 0x16 &&
            buffer[i + 3] == 0x24)
        {
            /* Retourne l'octet suivant 0x24 */
            if (i + 4 < size)
            {
                return (uint16_t)(i + 4);
            }
            else
            {
                return TAP_NOT_FOUND;
            }
        }
    }
    return TAP_NOT_FOUND;
}


/*
 * Analyse des headers TAP
 */
void analyseHeader(const uint8_t *buffer,
                   size_t size,
                   tap_headers_t *tapHeaders)
{
    uint16_t pos = 0;
    uint8_t index = 0;

    if (tapHeaders == NULL || buffer == NULL)
    {
        return;
    }

    /* Reset structure */
    memset(tapHeaders, 0, sizeof(tap_headers_t));

    while (index < MAX_HEADERS)
    {
        pos = tap_seek(buffer, size, pos);

        if (pos == TAP_NOT_FOUND)
        {
            break;
        }

        /* sécurité taille header */
        if (pos + sizeof(tap_header_t) > size)
        {
            break;
        }

        /* copie header */
        memcpy(&tapHeaders->header[index],
               &buffer[pos],
               sizeof(tap_header_t));

        /* afterSync */
        tapHeaders->bloc[index].afterSync = pos;
        tapHeaders->bloc[index].atSync = pos-1;

        /* calcul start/end */
        uint16_t start_addr =
            ((uint16_t)tapHeaders->header[index].start_addr_hi << 8) |
             (uint16_t)tapHeaders->header[index].start_addr_lo;

        uint16_t end_addr =
            ((uint16_t)tapHeaders->header[index].end_addr_hi << 8) |
             (uint16_t)tapHeaders->header[index].end_addr_lo;

        tapHeaders->bloc[index].dataSize =
            (uint16_t)(end_addr - start_addr + 1);

       
        
        index++;

        pos += sizeof(tap_header_t);
    }

    tapHeaders->nbHeader = index;

    /*
     * Calcul firstByte
     */
    for (uint8_t i = 0; i < tapHeaders->nbHeader; i++)
    {
        if (i < (tapHeaders->nbHeader - 1))
        {
            tapHeaders->bloc[i].firstByte =
                (uint16_t)(
                    (tapHeaders->bloc[i + 1].afterSync - 4) -
                    tapHeaders->bloc[i].dataSize
                );
            
            tapHeaders->bloc[i].dataFullSize =
            (uint16_t)(
                    (tapHeaders->bloc[i + 1].atSync - 3) -
                    tapHeaders->bloc[i].atSync
                );
        }
        else
        {
            tapHeaders->bloc[i].firstByte =
                (uint16_t)(size - tapHeaders->bloc[i].dataSize);
             tapHeaders->bloc[i].dataFullSize =
                (uint16_t)(size - tapHeaders->bloc[i].atSync);
        }
    }

    /*
     * Sécurise filename (garantit string propre)
     */
    for (uint8_t i = 0; i < tapHeaders->nbHeader; i++)
    {
        tapHeaders->header[i].filename[15] = '\0';
    }
}

#include <stdio.h>
#include <stdint.h>

int read_binary_file(const char *filename,
                     uint8_t *buffer,
                     size_t max_size,
                     size_t *bytes_read)
{
    if (bytes_read != NULL)
        *bytes_read = 0;

    FILE *f = fopen(filename, "rb");

    if (f == NULL)
        return -1;

    if (fseek(f, 0, SEEK_END) != 0)
    {
        fclose(f);
        return -1;
    }

    long file_size = ftell(f);

    if (file_size < 0)
    {
        fclose(f);
        return -1;
    }

    rewind(f);

    if ((size_t)file_size > max_size)
    {
        fclose(f);
        return -2;
    }

    size_t br = fread(buffer, 1, (size_t)file_size, f);

    fclose(f);

    if (br != (size_t)file_size)
        return -3;

    if (bytes_read != NULL)
        *bytes_read = br;

    return 0;
}

#include <stdio.h>
#include <stdint.h>

int sd_write_binary(uint8_t *ram,
                    tap_headers_t *tapHeaders)
{
    tap_header_t *h = &tapHeaders->header[0];

    char fullFilename[32];

    snprintf(fullFilename,
             sizeof(fullFilename),
             "%s.tap",
             (char *)h->filename);

    FILE *f = fopen(fullFilename, "wb");

    if (f == NULL)
    {
        perror(fullFilename);
        return -1;
    }

    size_t written = 0;

    /* En-tête TAP */
    static const uint8_t header[4] = { 0x16, 0x16, 0x16, 0x24 };

    written += fwrite(header, 1, sizeof(header), f);

    /* Les 9 octets précédant le nom */
    written += fwrite(&h->flag_int, 1, 9, f);

    /* Nom terminé par '\0' */
    for (int i = 0; i < sizeof(h->filename); i++)
    {
        written += fwrite(&h->filename[i], 1, 1, f);

        if (h->filename[i] == 0)
            break;
    }

    /* Données */
    uint16_t startAddr =
        ((uint16_t)h->start_addr_hi << 8) | h->start_addr_lo;

    uint16_t endAddr =
        ((uint16_t)h->end_addr_hi << 8) | h->end_addr_lo;

    if (endAddr >= startAddr)
    {
        size_t dataSize = (size_t)(endAddr - startAddr + 1);

        written += fwrite(&ram[startAddr], 1, dataSize, f);
    }

    if (fclose(f) != 0)
    {
        perror("fclose");
        return -1;
    }

    return (int)written;
}


#define CSV_BUFFER_SIZE 128

/*
 * Lit un fichier CSV depuis SD
 *
 * Exemple fichier :
 * 1,7,4
 * 2,5,4
 *
 * Le paramètre filename doit être sans extension :
 * "test" -> ouvre "test.csv"
 *
 * Retour :
 * >=0  : nombre de lignes chargées
 * -1   : erreur ouverture
 * -2   : fichier trop gros
 */
int load_csv_u8(const char *filename,
                uint8_t data[MAX_ROWS][MAX_COLS])
{
    char fullpath[32];

    /* construit "test.csv" */
    snprintf(fullpath,
             sizeof(fullpath),
             "%s.csv",
             filename);

    uint8_t buffer[CSV_BUFFER_SIZE];
    size_t bytes_read = 0;

    /*
     * Réutilise ta fonction existante
     */
    int ret = read_binary_file(fullpath,
                               buffer,
                               sizeof(buffer) - 1,
                               &bytes_read);

    if (ret != 0)
    {
        return ret;
    }

    /*
     * Important :
     * transforme buffer en string C
     */
    buffer[bytes_read] = '\0';

    int row = 0;

    char *line = strtok((char *)buffer, "\r\n");

    while (line != NULL && row < MAX_ROWS)
    {
        unsigned int v0, v1, v2;

        int n = sscanf(line,
                       "%u,%u,%u",
                       &v0,
                       &v1,
                       &v2);

        if (n == 3)
        {
            data[row][0] = (uint8_t)v0;
            data[row][1] = (uint8_t)v1;
            data[row][2] = (uint8_t)v2;

            row++;
        }

        line = strtok(NULL, "\r\n");
    }

    return row;
}


int list_tap_files(char filenames[][MAX_NAME_LEN],
                   size_t max_files,
                   size_t *files_found)
{
    if (files_found != NULL)
        *files_found = 0;

    DIR *dir = opendir(".");

    if (dir == NULL)
        return -1;

    size_t count = 0;

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        /* Ignore . et .. */
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;


        /* Vérifie si c'est un fichier */
        struct stat st;

        if (stat(entry->d_name, &st) != 0)
            continue;

        if (S_ISDIR(st.st_mode))
            continue;


        const char *name = entry->d_name;

        size_t len = strlen(name);


        /* Vérifie extension .tap */
        if (len >= 4)
        {
            const char *ext = &name[len - 4];

            if (strcasecmp(ext, ".tap") == 0)
            {
                if (count >= max_files)
                {
                    closedir(dir);
                    return -2;
                }


                /* Copie sans extension */
                size_t copy_len = len - 4;

                if (copy_len >= MAX_NAME_LEN)
                    copy_len = MAX_NAME_LEN - 1;


                strncpy(filenames[count],
                        name,
                        copy_len);

                filenames[count][copy_len] = '\0';

                count++;
            }
        }
    }

    closedir(dir);


    if (files_found != NULL)
        *files_found = count;


    return 0;
}

void print_files_ram(char filenames[][MAX_NAME_LEN],
                     size_t files_found,
                     uint8_t *ram)
{
    //1120 bytes for display (28 lines of 40 columns) (from #BB80 to #BFDF)
    // Pointeur vers mémoire écran ORIC
    uint8_t  *screen = &ram[ORIC_SCREEN_ADDR];

    // Efface uniquement les zones texte
    // en conservant les 2 premiers octets de chaque ligne
    for (size_t y = FILES_FIRST_LINE; y < SCREEN_LINES; y++) {

        uint8_t *line_ptr = screen + (y * SCREEN_COLUMNS);

        for (size_t x = 2; x < SCREEN_COLUMNS; x++) {
            line_ptr[x] = 0x20;
        }
    }

    // Affichage fichiers
    for (size_t i = 0; i < files_found; i++) {

        size_t row = FILES_FIRST_LINE + (i / 2);

        if (row >= SCREEN_LINES) {
            break;
        }

        size_t col = ((i & 1) == 0) ? COL1_X : COL2_X;

        uint8_t *line_ptr = screen + (row * SCREEN_COLUMNS);

        char *name = filenames[i];

        size_t len = strlen(name);

        if (len > MAX_NAME_LEN) {
            len = MAX_NAME_LEN;
        }

        // Alignement à gauche
        for (size_t j = 0; j < len; j++) {
            if ((col + j) < SCREEN_COLUMNS) {
                line_ptr[col + j] = name[j];
            }
        }
    }
}


uint8_t deplace_curseur(int8_t sens,
                        size_t max_files,
                        uint8_t *ram)
{
    static uint8_t old_index = 0;

    // Pointeur vers mémoire écran ORIC
    uint8_t *screen = &ram[ORIC_SCREEN_ADDR];

    // Aucun fichier
    if (max_files == 0) {
        return 0;
    }

    // Efface ancien curseur
    {
        size_t old_row = FILES_FIRST_LINE + (old_index / 2);

        size_t old_col;

        if ((old_index & 1) == 0) {
            old_col = CURSOR_COL1_X;
        } else {
            old_col = CURSOR_COL2_X;
        }

        screen[(old_row * SCREEN_COLUMNS) + old_col] = ' ';
    }

    // Déplacement
    if (sens > 0) {

        // Avance seulement si un fichier existe
        if ((indexFichier + 1) < max_files) {
            indexFichier++;
        }

    } else if (sens < 0) {

        // Recule sans passer sous 0
        if (indexFichier > 0) {
            indexFichier--;
        }
    }

    // Nouvelle position curseur
    {
        size_t row = FILES_FIRST_LINE + (indexFichier / 2);

        size_t col;

        if ((indexFichier & 1) == 0) {
            col = CURSOR_COL1_X;
        } else {
            col = CURSOR_COL2_X;
        }

        screen[(row * SCREEN_COLUMNS) + col] = CURSOR_CHAR;
    }

    old_index = indexFichier;

    return indexFichier;
}

void menuColor(uint8_t *ram)
{
    uint8_t *screen = &ram[ORIC_SCREEN_ADDR];

    // Colonne 0 : PAPER 0x10 sur toutes les lignes
    for (uint8_t y = 0; y < SCREEN_LINES; y++) {
        screen[y * SCREEN_COLUMNS + 0] = 0x10;
    }

    // Colonne 1 : INK 0x03 sur toutes les lignes
    for (uint8_t y = 0; y < SCREEN_LINES; y++) {
        screen[y * SCREEN_COLUMNS + 1] = 0x03;
    }

    // Exception ligne 0 colonne 1 : INK 0x14
    screen[0] = 0x14;
    screen[1] = 0x07;

    // Texte "F4GOH 2026" ligne 0 colonne 30
    const char *txt = "F4GOH 2026";

    uint8_t *dst = &screen[30];

    while (*txt) {
        *dst++ = *txt++;
    }
}
  

int sd_save_snap(const char *filename,
                 const uint8_t *snapBuffer,
                 uint32_t snapSize,
                 const uint8_t *ram,
                 uint32_t ramSize)
{
    char fullFilename[256];

    snprintf(fullFilename,
             sizeof(fullFilename),
             "%s.sna",
             filename);

    FILE *f = fopen(fullFilename, "wb");

    if (f == NULL)
    {
        perror("Impossible de créer le snapshot");
        return -1;
    }

    size_t written = 0;

    /* Snapshot (CPU + VIA + PSG...) */
    written += fwrite(snapBuffer,
                      1,
                      snapSize,
                      f);

    /* RAM */
    written += fwrite(ram,
                      1,
                      ramSize,
                      f);

    fflush(f);
    fclose(f);

    if (written != (snapSize + ramSize))
        return -1;

    return (int)written;
}


int sd_load_snap(const char *filename,
                 uint8_t *snapBuffer,
                 uint32_t snapSize,
                 uint8_t *ram,
                 uint32_t ramSize)
{
    char fullFilename[256];

    snprintf(fullFilename,
             sizeof(fullFilename),
             "%s.sna",
             filename);

    FILE *f = fopen(fullFilename, "rb");

    if (f == NULL)
    {
        perror("Impossible d'ouvrir le snapshot");
        return -1;
    }

    size_t read = 0;

    /* Lecture snapshot (CPU + VIA + PSG...) */
    read += fread(snapBuffer,
                  1,
                  snapSize,
                  f);

    /* Lecture RAM */
    read += fread(ram,
                  1,
                  ramSize,
                  f);

    fclose(f);

    /* Vérification de la taille */
    if (read != (snapSize + ramSize))
    {
        return -2;
    }

    return (int)read;
}

/*

int sd_save_bloc(const char *filename,
                 uint16_t addr,
                 uint16_t len,
                 const uint8_t *ram)
{
    char fullFilename[32];

    snprintf(fullFilename,
             sizeof(fullFilename),
             "%s.bin",
             filename);


    FILE *f = fopen(fullFilename, "wb");

    if (f == NULL)
    {
        perror(fullFilename);
        return -1;
    }


    size_t written = 0;

    written += fwrite(ram + addr,
                      1,
                      len,
                      f);


    if (fclose(f) != 0)
    {
        perror("fclose");
        return -1;
    }


    return (int)written;
}
*/



