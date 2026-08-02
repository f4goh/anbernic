# PNG Image Viewer Example

The following chapter explains the source code of the example program located in:

```text
anbernic/src/image
```

This example demonstrates how to display a **PNG image** using **SDL2** and the **SDL2_image** extension library.

Unlike the previous examples that only use SDL2, this program introduces **SDL2_image**, a library dedicated to loading image formats such as PNG, JPEG, GIF, TIFF and WebP into SDL surfaces.

The application:

- initializes SDL2;
- initializes the SDL2_image library;
- loads a PNG image from disk;
- converts it into an SDL texture;
- displays the image in a window;
- waits until the **MENU** button is pressed;
- releases all allocated resources before exiting.

![image](images/image.png)

---

# Initializing SDL

Before using any SDL function, the video subsystem must be initialized.

```c
SDL_Init(SDL_INIT_VIDEO);
```

This initializes the SDL video driver and prepares the creation of windows, renderers and textures.

---

# Initializing SDL2_image

Unlike SDL2, image loading support is provided by a separate library.

```c
IMG_Init(IMG_INIT_PNG);
```

This initializes PNG decoding support.

If the initialization fails, no PNG image can be loaded.

---

# Creating the window

```c
SDL_CreateWindow(...)
```

creates a 640×480 window.

```c
WINDOW_WIDTH  = 640
WINDOW_HEIGHT = 480
```

These dimensions correspond to the native screen resolution of the RG40XX H.

---

# Creating the renderer

```c
SDL_CreateRenderer(...)
```

The renderer is responsible for drawing textures on the screen.

Most SDL graphics operations are performed through the renderer.

---

# Loading the PNG image

```c
SDL_Surface *surface = IMG_Load("image.png");
```

`IMG_Load()` reads the PNG file from disk and creates an SDL surface stored in RAM.

If the file cannot be found or is invalid, the function returns `NULL`.

---

# Creating the texture

The surface is converted into a GPU texture.

```c
SDL_Texture *texture =
    SDL_CreateTextureFromSurface(renderer, surface);
```

Textures are optimized for rendering.

Once the texture has been created, the temporary surface is no longer needed.

```c
SDL_FreeSurface(surface);
```

---

# Displaying the image

The rendering sequence is very simple.

```c
SDL_RenderClear(renderer);

SDL_RenderCopy(renderer,
               texture,
               NULL,
               NULL);

SDL_RenderPresent(renderer);
```

The image occupies the entire window.

---

# Waiting for the MENU button

The program then opens the controller device.

```c
int fd = open("/dev/input/event1", O_RDONLY);
```

It continuously reads controller events.

When the MENU button (code **354**) is pressed, the program exits.

```c
if (ev.type == EV_KEY &&
    ev.code == 354 &&
    ev.value == 1)
{
    break;
}
```

This allows the user to close the image viewer without using a keyboard.

---

# Cleaning up

Before terminating, every SDL resource is released.

```c
SDL_DestroyTexture(texture);
SDL_DestroyRenderer(renderer);
SDL_DestroyWindow(window);

IMG_Quit();
SDL_Quit();
```

Proper cleanup avoids memory leaks and ensures that SDL shuts down correctly.

---

# The Makefile

This example introduces a new dependency: **SDL2_image**.

The Makefile therefore links both SDL2 and SDL2_image.

## PC compilation

```make
CFLAGS_PC = $(shell pkg-config --cflags sdl2 SDL2_image)
LIBS_PC   = $(shell pkg-config --libs sdl2 SDL2_image)
```

`pkg-config` automatically retrieves:

- the include directories;
- the compiler options;
- the libraries to link.

The important difference from the previous example is the addition of **SDL2_image** alongside **SDL2**.

---

## ARM64 compilation

For the RG40XX H, the ARM64 SDK is used.

```make
LIBS_ARM64 = \
    -L$(SDL2-SDK-arm64)/lib \
    -lSDL2_image \
    -lSDL2
```

The linker must now include **two libraries**:

- `-lSDL2`
- `-lSDL2_image`

The `libSDL2_image.so` library provides all image loading functions, including `IMG_Init()` and `IMG_Load()`, while `libSDL2.so` provides the SDL graphics and rendering API.

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

This example introduces the **SDL2_image** library, which extends SDL2 with support for loading image files such as PNG.

The program initializes SDL2 and SDL2_image, loads `image.png`, converts it into a texture, displays it on the screen, and waits for the **MENU** button before exiting.

Compared with a basic SDL2 application, the main difference is the additional dependency on **SDL2_image**, which must be linked in both the PC and ARM64 versions of the application.
