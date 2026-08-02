# MP3 Audio Playback Example

The following chapter explains the source code of the example program located in:

```text
anbernic/src/audio/mp3
```

This example demonstrates how to play an **MP3** file using **SDL2_mixer**.

Unlike WAV playback, SDL2 alone cannot decode compressed audio formats such as MP3. The **SDL2_mixer** extension library provides support for music formats including MP3, OGG, FLAC, MOD, MIDI and WAV. `Mix_LoadMUS()` loads a music file that is decoded on demand while it is playing.

---

# Why use SDL2_mixer?

SDL2 provides low-level audio playback but does not include MP3 decoding.

SDL2_mixer adds:

- MP3 playback
- OGG playback
- FLAC playback
- MOD playback
- MIDI playback
- Music looping
- Volume control
- Multiple simultaneous sound effects

For background music in games, SDL2_mixer is the recommended solution.

---

# Initializing SDL

Only the audio subsystem is required.

```c
SDL_Init(SDL_INIT_AUDIO);
```

---

# Opening the audio mixer

Before loading any music, the mixer must be initialized.

```c
Mix_OpenAudio(
    44100,
    MIX_DEFAULT_FORMAT,
    2,
    2048);
```

The parameters are:

| Parameter | Description |
|-----------|-------------|
| 44100 | Sample rate (44.1 kHz) |
| MIX_DEFAULT_FORMAT | Default sample format |
| 2 | Stereo output |
| 2048 | Audio buffer size |

If the initialization fails, the mixer cannot play any music.

---

# Loading an MP3 file

Music is loaded using:

```c
Mix_Music *music =
    Mix_LoadMUS("test.mp3");
```

Unlike `SDL_LoadWAV()`, the music is **not fully loaded into memory**.

SDL2_mixer streams and decodes the MP3 while it is playing, making it suitable for long music tracks.

If loading fails, the function returns `NULL`.

---

# Starting playback

Playback starts with:

```c
Mix_PlayMusic(music, 1);
```

The second parameter specifies the number of loops.

| Value | Meaning |
|-------:|---------|
| 1 | Play once |
| 0 | Play once (no extra loop) |
| -1 | Infinite loop |

---

# Waiting for playback to finish

The program periodically checks whether music is still playing.

```c
while (Mix_PlayingMusic())
{
    SDL_Delay(100);
}
```

The delay avoids unnecessary CPU usage.

---

# Releasing resources

Once playback is finished, all allocated resources are released.

```c
Mix_FreeMusic(music);

Mix_CloseAudio();

SDL_Quit();
```

---

# The Makefile

Unlike the WAV example, this program requires the **SDL2_mixer** library.

For the ARM64 build:

```make
LIBS_ARM64 = \
    -L$(SDL2_ARM64)/lib \
    -lSDL2_mixer \
    -lSDL2
```

Two libraries are linked:

| Library | Purpose |
|---------|---------|
| SDL2 | SDL core library |
| SDL2_mixer | Decoding and playback of MP3, OGG, FLAC, MOD, MIDI and other music formats |

`SDL2_mixer` automatically uses the appropriate decoder depending on the file extension and its contents.

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
## Press M (menu) key to exit

---

# Summary

This example introduces **SDL2_mixer**, the SDL extension library dedicated to music playback.

The program:

1. initializes the SDL audio subsystem;
2. opens the audio mixer;
3. loads an MP3 file using `Mix_LoadMUS()`;
4. starts playback with `Mix_PlayMusic()`;
5. waits until the music has finished playing;
6. releases all allocated resources.

Compared with the WAV example, the main difference is that the MP3 file is **decoded and streamed during playback**, allowing large compressed music files to be played efficiently without loading the entire file into memory.
