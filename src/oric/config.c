#include "config.h"

#include <stdio.h>
#include <string.h>

EmulatorConfig config =
{
    "/dev/input/event3"      // valeur par défaut
};

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int config_load(const char *filename)
{
    FILE *f = fopen(filename, "r");

    if (!f)
        return 0;

    char line[128];

    while (fgets(line, sizeof(line), f))
    {
        /* enlève CR/LF */
        line[strcspn(line, "\r\n")] = 0;

        /* saute les espaces en début de ligne */
        char *p = line;
        while (isspace((unsigned char)*p))
            p++;

        /* ligne vide ou commentaire */
        if (*p == '\0' || *p == '#' || *p == ';')
            continue;

        char *eq = strchr(p, '=');

        if (!eq)
            continue;

        *eq++ = '\0';

        /* supprime les espaces avant '=' */
        char *end = p + strlen(p) - 1;
        while (end > p && isspace((unsigned char)*end))
            *end-- = '\0';

        /* saute les espaces après '=' */
        while (isspace((unsigned char)*eq))
            eq++;

        if (!strcmp(p, "keyboard"))
        {
            strncpy(config.keyboard,
                    eq,
                    sizeof(config.keyboard) - 1);

            config.keyboard[sizeof(config.keyboard) - 1] = '\0';
        }
    }

    fclose(f);

    return 1;
}
