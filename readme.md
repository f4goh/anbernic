# Programming the Anbernic RG40XX H with SDL2

## Introduction

This project is a practical introduction to native application development for the **Anbernic RG40XX H** handheld console using **SDL2**.

Although primarily designed for retro gaming, the [RG40XX H](https://fr.aliexpress.com/item/1005012030722515.html) is a capable Linux-based ARM64 computer featuring a quad-core Cortex-A53 processor, a 640×480 IPS display, Wi-Fi, Bluetooth, HDMI output, and hardware-accelerated graphics. These features make it an excellent platform for developing emulators, games, multimedia applications, and embedded graphical software.

This tutorial explains how to set up a development environment on a Linux PC, write a first SDL2 application, cross-compile it for the RG40XX H, and run it directly on the console.

![view](images/RG40XXview.png)
---
# Menu

1. [Installation du compilateur sur le PC](01-installation-compilateur.md)
2. [Utilisation de muOS](02-utilisation-muos.md)
3. [Compilation](03-compilation.md)
4. [Exécution du programme dans muOS](04-execution-muos.md)
5. [Utilisation des boutons et du pavé directionnel](05-boutons-pad.md)
6. [Affichage d'une image](06-affichage-image.md)
7. [Affichage d'un texte](07-affichage-texte.md)
8. [Lire un fichier WAV](08-lire-wav.md)
9. [Lire un MP3](09-lire-mp3.md)
10. [Lire un fichier MOD](10-lire-mod.md)
---

# Hardware Specifications

| Feature | Specification |
|---------|---------------|
| Display | 4.0" IPS, OCA laminated |
| Resolution | 640 × 480 |
| CPU | Allwinner H700 Quad-Core ARM Cortex-A53 @ 1.5 GHz |
| GPU | Dual-Core Mali G31 MP2 |
| RAM | 1 GB DDR4 |
| Storage | 64 GB microSD (dual microSD slots) |
| Operating System | Linux 64-bit |
| Wireless | Wi-Fi 2.4/5 GHz (802.11 a/b/g/n/ac), Bluetooth 4.2 |
| Battery | 3200 mAh Li-Polymer |
| Battery Life | Up to 6 hours |
| Charging | USB-C (5V / 1.5A, C-to-C supported) |
| HDMI Output | Yes |
| USB Controllers | Wired and 2.4 GHz wireless supported |
| Vibration | Yes |
| RGB Lighting | RGB analog sticks with customizable effects |

![spec](images/RG40XX System.png)

---

# Software Environment

The console runs a 64-bit Linux system and is compatible with several custom firmwares such as **muOS**.

Development is performed on a Linux PC (Linux Mint is used throughout this tutorial) using the GNU cross compiler and SDL2.

Recommended Linux Mint editions:

| Edition | Typical RAM Usage | Recommendation |
|----------|------------------:|----------------|
| Cinnamon | 1.1–1.5 GB | No |
| MATE | 700–900 MB | Good |
| Xfce | 550–750 MB | Recommended |

---

# Creating the First SDL2 Project

Create a working directory:

```bash
mkdir Carrerouge
cd Carrerouge
```

The first example simply opens a 640×480 window and draws a red square.

Project structure:

```
Carrerouge/
│
├── main.cpp
├── CMakeLists.txt
└── build/
```

```c
#include <SDL2/SDL.h>
#include <stdbool.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Erreur SDL_Init : %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Carre Rouge",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        SDL_Log("Erreur creation fenetre : %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL)
    {
        SDL_Log("Erreur creation renderer : %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;

    SDL_Rect square = {
        WINDOW_WIDTH / 2 - 50,
        WINDOW_HEIGHT / 2 - 50,
        100,
        100};

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        running = false;
                    }
                    break;
            }
        }

        /* Fond noir */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        /* Carré rouge */
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &square);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
```

```console
cmake_minimum_required(VERSION 3.10)

project(carrerouge C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(carrerouge
    main.c
)

target_include_directories(carrerouge PRIVATE
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(carrerouge
    ${SDL2_LIBRARIES}
)
```


Compile on the PC:

```bash
mkdir build
cd build

cmake ..
make
```

Run the program:

```bash
./carrerouge
```

A window containing a red square should appear.

---

# Cross Compilation

Compile directly for the RG40XX H:

```bash
aarch64-linux-gnu-g++ main.cpp -o carrerouge -lSDL2
```

---

# Copying the Program to the Console

Transfer the executable:

```bash
scp carrerouge root@rg40xx:/mnt/
```

Connect through SSH:

```bash
ssh root@rg40xx
```

or

```bash
ssh root@192.168.1.50
```

Make it executable:

```bash
chmod +x carrerouge
```

Run it:

```bash
./carrerouge
```

---

# Useful References

## muOS

https://muos.dev/devices

## SDL2

https://wiki.libsdl.org/

## RG35XX SDL Test Application

https://github.com/CodeAsm/RG35XXH-Testapp

Although written for the RG35XX H, this project is an excellent reference for understanding SDL programming on Anbernic handhelds.

---

# Future Chapters

The next tutorials will cover:

- SDL2 rendering
- Image loading
- Audio playback
- Reading gamepad inputs
- Double buffering
- Sprites and animations
- Building a simple game engine
- Cross-compilation using CMake
- Performance optimization on the H700 processor


