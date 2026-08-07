# ORIC Emulator Project

## Overview

This project is a complete ORIC computer emulator written in C.

The objective was to recreate the original ORIC hardware environment and make it run on modern platforms:

- Desktop Linux PC
- ARM64 handheld devices (Anbernic / MuOS)
- Raspberry Pi Pico prototype platform (initial development)

The emulator reproduces the main ORIC hardware components while providing modern input, audio and storage features.

---

# Implemented Features

## 6502 CPU Emulation

- Fully functional MOS 6502 CPU emulation.
- Instruction execution.
- IRQ and NMI interrupt handling.
- WAI instruction support.
- CPU timing synchronized with peripherals.

---

## VIA 6522 Emulation

Implemented VIA peripheral support:

- VIA clock synchronized with CPU cycles.
- IRQ generation.
- Hardware timing management.
- Keyboard interface support.

---

## ULA Video Emulation

Implemented ORIC video hardware:

- Text mode.
- HIRES graphics mode.
- Character rendering.
- Alternate character sets.
- Video attributes:
  - Foreground color
  - Background color
  - Inverse video
  - Blink mode
  - Double height characters

Supported ORIC 8-color palette.

The video output is converted to SDL frames for modern displays.

---

## Keyboard Support

Implemented PS/2 keyboard emulation.

Features:

- Linux input event support.
- Scan code processing.
- Keyboard buffering.
- ORIC compatible key handling.

---

## NES Controller Emulation

A virtual NES controller interface has been implemented.

Controller mapping:

```
Bit     Function

D7      A
D6      B
D5      SELECT
D4      START
D3      UP
D2      DOWN
D1      LEFT
D0      RIGHT
```

The controller uses active-low logic like the original NES protocol.

---

## Anbernic Joystick Support

Implemented support for Anbernic handheld controls.

Supported inputs:

- D-Pad
- Left analog stick
- A button
- B button
- SELECT button
- START button

The analog stick and D-Pad can both control ORIC directions.

Analog sensitivity is configurable using a detection threshold.

---

## AY-3-8912 Audio Emulation

Implemented ORIC sound generation using the AY PSG chip.

Features:

- SDL audio output.
- Mono audio stream.
- Configurable sample rate.
- Audio filtering.
- Adjustable audio gain.

---

## TAP File Support

Implemented ORIC tape loading.

Features:

- TAP file browsing.
- TAP header analysis.
- Binary loading.
- CLOAD support.
- Automatic tape loading.

---

## Snapshot System

Implemented ORIC snapshots.

Snapshot format:

```
.sna
```

Contains:

- CPU state
- VIA state
- PSG state
- Complete RAM image

Supported operations:

- Save emulator state.
- Restore emulator state.

---

## Binary Block Save

Implemented raw memory block saving.

Features:

- Save selected RAM regions.
- Export binary files.
- Compatible with ORIC software development workflows.

---

## Configuration System

Implemented external configuration files.

Example:

```ini
# ORIC Emulator configuration

keyboard=/dev/input/event3
audio_gain=2
```

Advantages:

- No recompilation required.
- Same binary can run on different machines.
- Hardware settings can be customized.

---

## Video Timing

Implemented real-time emulation timing:

- 60 Hz video refresh.
- CPU timing control.
- Frame synchronization.
- Stable emulation speed.

---

# Platform Support

## Desktop Linux

Supported:

- SDL2 video output.
- SDL audio.
- Linux input subsystem.
- OpenGL renderer.

---

## ARM64 Handheld Devices

Tested on:

- Anbernic devices.
- MuOS environment.

Using:

- SDL2.
- OpenGLES2 renderer.
- Linux input events.

---

# Emulator Architecture

Main hardware components:

```
                 ORIC Emulator

                      CPU
                       |
          +------------+------------+
          |                         |
        VIA                        ULA
          |                         |
      Keyboard                  Video
          
          |
        Input
          |
  +-------+--------+
  |                |
NES Controller   Anbernic Joystick


          |
        AY PSG
          |
       SDL Audio


          |
       Storage
          |
 TAP / SNAP / Binary files
```

The emulator is designed using independent modules to simplify maintenance and future improvements.

---

# Current Status

The emulator is feature complete for normal ORIC usage.

Implemented:

- CPU 6502 emulation
- VIA synchronization
- ULA text and HIRES graphics
- PS/2 keyboard
- NES controller support
- Anbernic joystick support
- AY audio
- TAP loading
- Snapshot save/load
- Binary block saving
- Configuration files
- 60 Hz timing
- PC and ARM64 portability

---

# Future Improvements

Possible future enhancements:

- Integrated graphical menu.
- Automatic input device detection.
- Video scanlines.
- CRT effects.
- Improved audio mixer.
- More accurate analog joystick handling.
- Additional ORIC hardware expansions.

---

# Conclusion

This project successfully recreates a complete ORIC computer environment on modern hardware.

It combines accurate hardware emulation with modern interfaces, allowing classic ORIC software to run on desktop computers and portable ARM64 gaming devices.

The project started as a Raspberry Pi Pico hardware experiment and evolved into a complete cross-platform ORIC emulator.
