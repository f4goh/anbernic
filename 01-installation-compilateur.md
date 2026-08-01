# Installing the Development Environment

To compile and test applications for **muOS**, you need a few development tools on your Linux PC.

## Install the basic development tools

```bash
sudo apt update
sudo apt install build-essential cmake git libsdl2-dev
```

### Why install these packages?

- **build-essential**: Installs the GNU compiler (`gcc`), `make`, and other essential build tools.
- **cmake**: Generates portable build files and simplifies project compilation.
- **git**: Used to clone and update source code repositories.
- **libsdl2-dev**: Installs the SDL2 development library. SDL2 provides a simple API for graphics, audio, input devices (buttons, gamepad), and window management. It is commonly used to develop applications and games compatible with muOS.

## Install the ARM64 cross compiler

```bash
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
```

### Why install the ARM64 cross compiler?

The development PC usually runs on an **x86_64** processor, while the handheld running **muOS** uses an **ARM64 (AArch64)** processor.

The cross compiler allows you to build executables on your PC that will run directly on the ARM64 device.

- **gcc-aarch64-linux-gnu**: C compiler for the ARM64 architecture.
- **g++-aarch64-linux-gnu**: C++ compiler for the ARM64 architecture.

## Verify the installation

Run the following commands:

```bash
gcc --version
g++ --version
cmake --version
git --version
sdl2-config --version
aarch64-linux-gnu-gcc --version

aarch64-linux-gnu-gcc (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
Copyright (C) 2023 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

aarch64-linux-gnu-g++ --version

aarch64-linux-gnu-g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
Copyright (C) 2023 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

If each command prints a version number without any error message, the development environment has been installed correctly.
