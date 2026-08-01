#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdbool.h>


#define INPUT_DEVICE "/dev/input/event1"

#define FPS 60
#define FRAME_TIME (1000 / FPS)


/*
    FUTUR MOTEUR DE JEU

    Structure prévue :

        boucle principale

              |
              |
        +-----+------+
        |            |
     UPDATE       RENDER
        |            |
        |            |
    collisions    dessiner
    déplacement   décor
    IA            sprites
    physique      interface


*/


/*
    FUTUR JOUEUR

typedef struct
{
    int x;
    int y;

    int speed;

    SDL_Texture *texture;

} Player;


*/


/*
    FUTURE MAP TILE

#define TILE_SIZE 32

int map[100][100];

*/


int main(void)
{
    /*
        Initialisation SDL audio
    */

    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        SDL_Log("SDL_Init : %s", SDL_GetError());
        return 1;
    }


    /*
        Initialisation MOD
    */

    if (!(Mix_Init(MIX_INIT_MOD) & MIX_INIT_MOD))
    {
        SDL_Log("Mix_Init MOD : %s", Mix_GetError());

        SDL_Quit();
        return 1;
    }


    if (Mix_OpenAudio(44100,
                      MIX_DEFAULT_FORMAT,
                      2,
                      2048) < 0)
    {
        SDL_Log("Mix_OpenAudio : %s", Mix_GetError());

        Mix_Quit();
        SDL_Quit();

        return 1;
    }



    /*
        Chargement musique Amiga MOD
    */

    Mix_Music *music = Mix_LoadMUS("test.mod");


    if (!music)
    {
        SDL_Log("Mix_LoadMUS : %s",
                Mix_GetError());

        Mix_CloseAudio();
        Mix_Quit();
        SDL_Quit();

        return 1;
    }


    Mix_PlayMusic(music, -1);



    /*
        Ouverture des boutons Anbernic

        O_NONBLOCK permet de ne pas bloquer
        la boucle du jeu.
    */

    int input_fd = open(INPUT_DEVICE,
                        O_RDONLY | O_NONBLOCK);


    if (input_fd < 0)
    {
        perror("input");

        Mix_FreeMusic(music);
        Mix_CloseAudio();
        Mix_Quit();
        SDL_Quit();

        return 1;
    }



    printf("Lecture test.mod\n");
    printf("MENU pour quitter\n");



    bool running = true;

    struct input_event ev;



    /*
        BOUCLE PRINCIPALE DU JEU

        Ici on aura :

            while(running)
            {

                lire_clavier();

                update();

                collision();

                render();

            }

    */


    while (running)
    {

        Uint32 start = SDL_GetTicks();



        /*
            INPUT

            Lecture manette
        */

        while (read(input_fd,
                    &ev,
                    sizeof(ev)) == sizeof(ev))
        {

            if (ev.type == EV_KEY &&
                ev.code == 354 &&
                ev.value == 1)
            {
                running = false;
            }


            /*
                FUTURS CONTROLES


                if(ev.code == 304)
                    joueur_attaque();


                if(ev.code == 305)
                    joueur_action();


            */

        }




        /*
            UPDATE

            Ici :

            - déplacement joueur
            - animation
            - IA ennemis
            - physique
            - collisions


            Exemple futur :

            player.x += dx;

            if(collision(player, map))
                player.x -= dx;

        */



        /*
            RENDER

            Ordre classique :

            1) vider écran

            SDL_RenderClear();


            2) dessiner la carte

            drawMap();


            3) dessiner objets

            drawObjects();


            4) dessiner joueur

            drawPlayer();


            5) afficher

            SDL_RenderPresent();

        */



        /*
            Limitation FPS
        */

        Uint32 elapsed =
            SDL_GetTicks() - start;


        if (elapsed < FRAME_TIME)
        {
            SDL_Delay(FRAME_TIME - elapsed);
        }

    }



    /*
        Fermeture propre
    */


    close(input_fd);


    Mix_HaltMusic();

    Mix_FreeMusic(music);

    Mix_CloseAudio();

    Mix_Quit();

    SDL_Quit();



    printf("Fin jeu\n");


    return 0;
}
