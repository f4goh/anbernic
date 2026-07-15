# Programming the Anbernic RG40XX H with SDL2

## Introduction

This project is a practical introduction to native application development for the **Anbernic RG40XX H** handheld console using **SDL2**.

Although primarily designed for retro gaming, the [RG40XX H](https://fr.aliexpress.com/item/1005012030722515.html) is a capable Linux-based ARM64 computer featuring a quad-core Cortex-A53 processor, a 640×480 IPS display, Wi-Fi, Bluetooth, HDMI output, and hardware-accelerated graphics. These features make it an excellent platform for developing emulators, games, multimedia applications, and embedded graphical software.

This tutorial explains how to set up a development environment on a Linux PC, write a first SDL2 application, cross-compile it for the RG40XX H, and run it directly on the console.

![view](images/RG40XXview.png)

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

# Installing the Development Environment

Install the required tools:

```bash
sudo apt update
sudo apt install build-essential cmake git libsdl2-dev
```

Install the ARM64 cross compiler:

```bash
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
```

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
aarch64-linux-gnu-g++ \
main.cpp \
-o carrerouge \
-lSDL2
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


