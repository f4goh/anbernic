# Amiga MOD Music Playback Example

The following chapter explains the source code of the example program located in:

```text
anbernic/src/audio/mod
```

This example demonstrates how to play an **Amiga ProTracker MOD** music file while implementing the structure of a future game engine.

Unlike the previous MP3 example, this program introduces a real **game loop** running at 60 FPS, allowing music playback and controller input to run simultaneously.

---

# What is the MOD format?

The **MOD** format was created on the **Commodore Amiga** in the late 1980s.

It became famous with music trackers such as:

- Ultimate SoundTracker
- NoiseTracker
- **ProTracker**

A MOD file does not contain recorded music like an MP3.

Instead, it contains:

- digital instrument samples;
- musical patterns;
- effects (vibrato, arpeggio, portamento, volume, etc.);
- song structure.

During playback, the tracker replays the samples according to the pattern data in real time.

This approach produces very small music files while maintaining excellent audio quality.

ProTracker became the de facto standard for Amiga games and the demoscene during the early 1990s, and the MOD format remains one of the best-known tracker formats.

If you are interested in Amiga programming, demos and game development, you can also visit my Amiga projects:

https://github.com/f4goh/amiga

---

# Initializing SDL audio

Only the audio subsystem is required.

```c
SDL_Init(SDL_INIT_AUDIO);
```

---

# Initializing MOD support

Before loading a MOD file, SDL2_mixer must initialize support for this music format.

```c
Mix_Init(MIX_INIT_MOD);
```

`MIX_INIT_MOD` tells SDL2_mixer that the application requires playback of Amiga MOD files.

If MOD support is unavailable, the program terminates immediately.

---

# Opening the audio device

The mixer is configured for:

```c
Mix_OpenAudio(
    44100,
    MIX_DEFAULT_FORMAT,
    2,
    2048);
```

This creates a standard stereo output running at 44.1 kHz.

---

# Loading the MOD file

Loading the music is identical to an MP3.

```c
Mix_Music *music =
    Mix_LoadMUS("test.mod");
```

SDL2_mixer automatically detects the file format and uses the appropriate decoder.

---

# Playing the music

Playback starts immediately.

```c
Mix_PlayMusic(music, -1);
```

The value:

```text
-1
```

means:

> Loop forever.

This is typically how background music is handled in a game.

---

# Opening the controller

Unlike the MP3 example, this program also opens the controller.

```c
int input_fd =
    open("/dev/input/event1",
         O_RDONLY | O_NONBLOCK);
```

The important difference is the use of:

```text
O_NONBLOCK
```

Without this option, `read()` would block until a button is pressed.

With `O_NONBLOCK`, the game loop continues running even when no input is available.

This is essential for every real-time game.

---

# The game loop

The example introduces the classic structure used by most video games.

```text
while(running)
{
    Read Inputs

    Update Game

    Render Graphics

    Limit FPS
}
```

Although this example only plays music, the comments already show where future game logic will be inserted.

Typical organization:

```text
Game Loop
│
├── INPUT
│
├── UPDATE
│
│   ├── Player movement
│   ├── Enemy AI
│   ├── Physics
│   └── Collisions
│
├── RENDER
│
│   ├── Draw map
│   ├── Draw objects
│   ├── Draw player
│   └── Present frame
│
└── Wait next frame
```

This architecture is used by almost every 2D game engine.

---

# Reading controller events

Every frame, the controller is polled.

```c
while (read(input_fd,
            &ev,
            sizeof(ev)) == sizeof(ev))
{
    ...
}
```

If the MENU button is pressed:

```c
ev.code == 354
```

the variable

```c
running
```

becomes

```text
false
```

which exits the main loop.

---

# Frame rate limiting

The example targets:

```text
60 FPS
```

Each frame duration is measured.

```c
Uint32 elapsed =
    SDL_GetTicks() - start;
```

If the frame completed too quickly, the program waits.

```c
SDL_Delay(
    FRAME_TIME - elapsed);
```

This keeps the game running at a constant speed regardless of CPU performance.

---

# Future game engine

Several commented sections already describe the planned architecture.

Future additions include:

- Player structure
- Tile map
- Sprite rendering
- Collision detection
- Enemy AI
- Physics
- Animation system
- Object management

This transforms the example from a simple music player into the foundation of a complete 2D game engine.

---

# Cleaning up

When the user presses the MENU button:

- controller is closed;
- music playback stops;
- music is released;
- audio device is closed;
- SDL2_mixer shuts down;
- SDL shuts down.

```c
Mix_HaltMusic();

Mix_FreeMusic(music);

Mix_CloseAudio();

Mix_Quit();

SDL_Quit();
```

---

# The Makefile

Like the MP3 example, this program uses SDL2_mixer.

The ARM64 linker options are:

```make
LIBS_ARM64 = \
    -L$(SDL2_ARM64)/lib \
    -lSDL2_mixer \
    -lSDL2
```

Only two SDL libraries are required:

| Library | Purpose |
|---------|---------|
| SDL2 | Core SDL library |
| SDL2_mixer | Playback of MOD, MP3, OGG, FLAC, MIDI and other music formats |

No additional code changes are required to play a MOD file.

The only difference compared with the MP3 example is the initialization of MOD support:

```c
Mix_Init(MIX_INIT_MOD);
```

and the music file:

```text
test.mod
```

---

# Compilation

Compile for the PC:

```bash
make pc
```

Compile for the RG40XX H:

```bash
make arm64
```

---

# Summary

This example demonstrates how to play an **Amiga ProTracker MOD** music file while implementing the structure of a future game engine.

The application:

1. initializes SDL audio;
2. enables MOD support with `Mix_Init(MIX_INIT_MOD)`;
3. loads `test.mod`;
4. starts music playback in an infinite loop;
5. opens the controller in non-blocking mode;
6. executes a classic **Input → Update → Render** game loop at **60 FPS**;
7. exits when the **MENU** button is pressed.

Beyond music playback, this example serves as a template for future SDL2 games running on the RG40XX H under muOS.
