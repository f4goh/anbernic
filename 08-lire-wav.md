# WAV Audio Playback Example

The following chapter explains the source code of the example program located in:

```text
anbernic/src/audio/wav
```

This example demonstrates how to play an uncompressed **WAV** audio file using the SDL2 audio subsystem.

Unlike the previous examples, no graphics are used. The program only initializes the SDL audio subsystem, loads a WAV file into memory, sends it to the audio device, waits until playback finishes, and then exits. SDL provides the `SDL_LoadWAV()` helper function to load WAV files directly into memory.

---

# Initializing the audio subsystem

Only the audio subsystem is required.

```c
SDL_Init(SDL_INIT_AUDIO);
```

Unlike previous examples, there is no need to initialize the video subsystem.

---

# Loading the WAV file

The audio file is loaded using:

```c
SDL_AudioSpec wavSpec;
Uint8 *wavBuffer = NULL;
Uint32 wavLength = 0;

SDL_LoadWAV(
    "test.wav",
    &wavSpec,
    &wavBuffer,
    &wavLength);
```

`SDL_LoadWAV()` performs several operations automatically:

- opens the WAV file;
- reads its header;
- determines the audio format;
- allocates a memory buffer;
- loads the PCM audio samples into memory.

The function fills three variables:

| Variable | Description |
|----------|-------------|
| `wavSpec` | Audio format (sample rate, channels, sample format...) |
| `wavBuffer` | Pointer to the audio samples |
| `wavLength` | Size of the audio buffer in bytes |

If the file cannot be opened or is not a valid WAV file, the function returns `NULL`.

---

# Opening the audio device

Once the file has been loaded, the program opens the default audio device.

```c
SDL_OpenAudioDevice(
    NULL,
    0,
    &wavSpec,
    NULL,
    0);
```

Passing `wavSpec` tells SDL to configure the audio device using exactly the same format as the WAV file.

The returned value is an audio device identifier.

```c
SDL_AudioDeviceID dev;
```

If the value is zero, opening the audio device has failed.

---

# Queueing the audio

Instead of using an audio callback, this example uses SDL's audio queue.

```c
SDL_QueueAudio(
    dev,
    wavBuffer,
    wavLength);
```

This copies the entire WAV file into the playback queue.

SDL will then stream the samples automatically to the sound hardware.

---

# Starting playback

Audio devices are initially paused.

Playback begins with:

```c
SDL_PauseAudioDevice(dev, 0);
```

Passing `0` means:

> Start audio playback.

---

# Waiting for the end of playback

The program periodically checks how many bytes remain in the queue.

```c
while (SDL_GetQueuedAudioSize(dev) > 0)
{
    SDL_Delay(100);
}
```

As long as the queue is not empty, the sound is still playing.

The `SDL_Delay(100)` call avoids consuming CPU unnecessarily.

---

# Releasing resources

Once playback has finished, every allocated resource is released.

```c
SDL_CloseAudioDevice(dev);

SDL_FreeWAV(wavBuffer);

SDL_Quit();
```

`SDL_FreeWAV()` releases the memory previously allocated by `SDL_LoadWAV()`.

---

# Supported WAV format

This example plays standard uncompressed PCM WAV files.

Typical formats include:

- 16-bit PCM
- Mono or Stereo
- 22050 Hz
- 44100 Hz
- 48000 Hz

Compressed formats such as MP3 or OGG are **not** supported by `SDL_LoadWAV()`.

---

# The Makefile

Unlike the PNG and text examples, no additional SDL extension library is required.

The program only uses the SDL2 audio subsystem.

### PC

```make
pkg-config --cflags sdl2

pkg-config --libs sdl2
```

### ARM64

```make
LIBS_ARM64 = \
    -lSDL2
```

No `SDL2_image`, `SDL2_ttf` or `SDL2_mixer` libraries are required.

Everything needed to play a WAV file is already included in the standard SDL2 library.

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

This example demonstrates the simplest way to play a sound with SDL2.

The program:

1. initializes the SDL audio subsystem;
2. loads a WAV file into memory using `SDL_LoadWAV()`;
3. opens the default audio device;
4. queues the audio samples for playback;
5. starts playback;
6. waits until all samples have been played;
7. releases every allocated resource before exiting.

It is an excellent starting point for adding sound effects to SDL2 applications running on the RG40XX H.
