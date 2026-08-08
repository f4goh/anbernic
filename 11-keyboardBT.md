# Anbernic RG40XX H Bluetooth Keyboard Input Example

This chapter explains how to install Bluetooth support on an **Anbernic RG40XX H running muOS Jacaranda**, connect a Bluetooth keyboard, verify the Linux input device, and read keyboard events from a C program.

The example uses the Linux input subsystem:

```text
/dev/input/eventX
```

to receive keyboard events directly from a Bluetooth HID device.

---

# Hardware and software configuration

Target device:

```text
Anbernic RG40XX H
```

Operating system:

```text
muOS 2601.0 JACARANDA
```

Bluetooth device:

```text
Bluetooth Keyboard
MAC:
54:46:6E:3A:0A:D2
```

The keyboard uses the standard Bluetooth HID profile.

---

# Installing Bluetooth support

The Bluetooth package must match the installed muOS version.

https://github.com/nvcuong1312/bltMuos/releases

For RG40XX H:

```text
Bluetooth-PIXIE-GOOSE-JACARANDA-ANBERNIC.muxupd
```

Do not unzip the file.

---

# Copying the Bluetooth package

Insert the muOS SD card into a computer.

Copy the file into:

```text
/ARCHIVE
```

Example:

```text
/ARCHIVE/Bluetooth-PIXIE-GOOSE-JACARANDA-ANBERNIC.muxupd
```

The file must remain unchanged with the `.muxupd` extension.

---

# Installing from muOS

Start the Anbernic.

Open:

```text
Application
    |
    +-- Archive Manager
            |
            +-- Bluetooth-PIXIE-GOOSE-JACARANDA-ANBERNIC
                    |
                    +-- Install
```

The console reboots automatically.

After reboot:

```text
Application
    |
    +-- Bluetooth
```

is available.


![bluetooth1](images/oricEmu/bluetooth1.png)

![bluetooth2](images/oricEmu/bluetooth2.png)

---

# SSH connection

Connect to the console:

```bash
ssh root@192.168.1.203
```

Example welcome message:

```text
Welcome to muOS 2601.0 JACARANDA
```

---

# Bluetooth initialization

Start Bluetooth management:

```bash
bluetoothctl
```

The controller should show:

```text
hci0 new_settings:
powered bondable ssp br/edr le secure-conn
```

---

# Searching for Bluetooth devices

Enable scanning:

```bash
scan on
```

The keyboard appears:

```text
Device 54:46:6E:3A:0A:D2 Bluetooth Keyboard
```

---

# Pairing the keyboard

Pair the device:

```bash
pair 54:46:6E:3A:0A:D2
```

Expected result:

```text
Pairing successful
```

---

# Connecting the keyboard

Connect:

```bash
connect 54:46:6E:3A:0A:D2
```

Expected result:

```text
Connection successful
```
---

# Info
```text
[/run/muos/storage/application/keyboard]# bluetoothctl
hci0 new_settings: powered bondable ssp br/edr le secure-conn 
Agent registeredard]# 
[CHG] Controller 68:8F:C9:E0:05:CF Pairable: yes
[Bluetooth Keyboard]# info 54:46:6E:3A:0A:D2
Device 54:46:6E:3A:0A:D2 (public)
	Name: Bluetooth Keyboard
	Alias: Bluetooth Keyboard
	Appearance: 0x03c1 (961)
	Icon: input-keyboard
	Paired: yes
	Bonded: yes
	Trusted: no
	Blocked: no
	Connected: yes
	WakeAllowed: no
	LegacyPairing: no
	UUID: Generic Access Profile    (00001800-0000-1000-8000-00805f9b34fb)
	UUID: Generic Attribute Profile (00001801-0000-1000-8000-00805f9b34fb)
	UUID: Device Information        (0000180a-0000-1000-8000-00805f9b34fb)
	UUID: Battery Service           (0000180f-0000-1000-8000-00805f9b34fb)
	UUID: Human Interface Device    (00001812-0000-1000-8000-00805f9b34fb)
	Modalias: usb:v04E8p7021d0001
[Bluetooth Keyboard]# 
```

---

# Trusting the keyboard

Save the connection:

```bash
trust 54:46:6E:3A:0A:D2
```

Exit:

```bash
exit
```

---

# Checking Linux input devices

Linux exposes input devices through:

```text
/proc/bus/input/devices
```

Run:

```bash
cat /proc/bus/input/devices
```

The Bluetooth keyboard appears:

```text
N: Name="Bluetooth Keyboard"

H: Handlers=sysrq kbd leds event3
```

The important information is:

```text
event3
```

The keyboard device is:

```text
/dev/input/event3
```

---

# Testing raw keyboard events

You can verify the keyboard directly:

```bash
hexdump -C /dev/input/event3
```

Press keys.

Data should appear in the terminal.

Exit:

```text
CTRL+C
```

---

# Linux input events

Linux does not directly send ASCII characters.

A keyboard sends:

```text
Event type
Key code
Value
```

Example:

```text
KEY_A
value = 1
```

means:

```text
The A key was pressed
```

The application converts Linux key codes into characters.

---

# C Bluetooth Keyboard Reader Example

This program:

- opens the Bluetooth keyboard;
- reads Linux input events;
- converts key codes into characters;
- exits when the Q key is pressed.

File:

```text
btkeyboard.c
```

---

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>


char *keymap[256] =
{
    /* Lettres */
    [KEY_A]="a",
    [KEY_B]="b",
    [KEY_C]="c",
    [KEY_D]="d",
    [KEY_E]="e",
    [KEY_F]="f",
    [KEY_G]="g",
    [KEY_H]="h",
    [KEY_I]="i",
    [KEY_J]="j",
    [KEY_K]="k",
    [KEY_L]="l",
    [KEY_M]="m",
    [KEY_N]="n",
    [KEY_O]="o",
    [KEY_P]="p",
    [KEY_Q]="q",
    [KEY_R]="r",
    [KEY_S]="s",
    [KEY_T]="t",
    [KEY_U]="u",
    [KEY_V]="v",
    [KEY_W]="w",
    [KEY_X]="x",
    [KEY_Y]="y",
    [KEY_Z]="z",

    /* Chiffres */
    [KEY_0]="0",
    [KEY_1]="1",
    [KEY_2]="2",
    [KEY_3]="3",
    [KEY_4]="4",
    [KEY_5]="5",
    [KEY_6]="6",
    [KEY_7]="7",
    [KEY_8]="8",
    [KEY_9]="9",

    /* Espace et contrôle */
    [KEY_SPACE]=" ",
    [KEY_ENTER]="\n",
    [KEY_BACKSPACE]="\b",
    [KEY_TAB]="\t",

    /* Symboles */
    [KEY_MINUS]="-",
    [KEY_EQUAL]="=",
    [KEY_LEFTBRACE]="[",
    [KEY_RIGHTBRACE]="]",
    [KEY_SEMICOLON]=";",
    [KEY_APOSTROPHE]="'",
    [KEY_GRAVE]="`",
    [KEY_BACKSLASH]="\\",
    [KEY_COMMA]=",",
    [KEY_DOT]=".",
    [KEY_SLASH]="/",

    /* Flèches */
    [KEY_UP]="<UP>",
    [KEY_DOWN]="<DOWN>",
    [KEY_LEFT]="<LEFT>",
    [KEY_RIGHT]="<RIGHT>"
};


int main()
{
    int fd;
    struct input_event ev;


    fd = open("/dev/input/event3", O_RDONLY);


    if(fd < 0)
    {
        perror("Keyboard open error");
        return 1;
    }


    printf("Bluetooth keyboard active\n");
    printf("Press Q to quit\n\n");


int main()
{
    int fd;
    struct input_event ev;

    fd = open("/dev/input/event3", O_RDONLY);

    if (fd < 0) {
        perror("Erreur ouverture clavier");
        return 1;
    }

    printf("Clavier Bluetooth actif...\n");
    printf("Tape 'q' pour quitter\n\n");

    while(1)
    {
        if(read(fd, &ev, sizeof(ev)) == sizeof(ev))
        {
            if(ev.type == EV_KEY && ev.value == 1)
            {
                if(ev.code < 256 && keymap[ev.code])
                {
                    printf("%s", keymap[ev.code]);
                    if (keymap[ev.code][0]=='q'){
                        printf("\nQuitter\n");
                        break;
                    }
                }
                else
                {
                    printf("[code=%d]", ev.code);
                }

                fflush(stdout);
            }
        }
    }

    close(fd);

    return 0;
}


    close(fd);

    return 0;
}
```

---

# Compilation

Compile directly on the RG40XX H:

```bash
gcc btkeyboard.c -o btkeyboard
```

Run:

```bash
./btkeyboard
```

Example:

```text
Bluetooth keyboard active
Press Q to quit

hello world
```

Press:

```text
q
```

The program exits:

```text
Exit
```

---

# SSH verification commands summary

## Open Bluetooth manager

```bash
bluetoothctl
```

## Scan devices

```bash
scan on
```

## Pair device

```bash
pair MAC_ADDRESS
```

## Connect device

```bash
connect MAC_ADDRESS
```

## Trust device

```bash
trust MAC_ADDRESS
```

## Check input devices

```bash
cat /proc/bus/input/devices
```

## List input files

```bash
ls /dev/input/
```

## Test keyboard events

```bash
hexdump -C /dev/input/event3
```

---
## USB keyboard


```text
I: Bus=0003 Vendor=046d Product=c31c Version=0110
N: Name="Logitech USB Keyboard"
P: Phys=usb-sunxi-ohci-1/input0
S: Sysfs=/devices/platform/soc/5101000.ohci0-controller/usb8/8-1/8-1:1.0/0003:046D:C31C.0008/input/input10
U: Uniq=
H: Handlers=sysrq kbd leds event4 
B: PROP=0
B: EV=120013
B: KEY=1000000000007 ff9f207ac14057ff febeffdfffefffff fffffffffffffffe
B: MSC=10
B: LED=1f

I: Bus=0003 Vendor=046d Product=c31c Version=0110
N: Name="Logitech USB Keyboard"
P: Phys=usb-sunxi-ohci-1/input1
S: Sysfs=/devices/platform/soc/5101000.ohci0-controller/usb8/8-1/8-1:1.1/0003:046D:C31C.0009/input/input11
U: Uniq=
H: Handlers=kbd event5 
B: PROP=0
B: EV=1b
B: KEY=2010000 397ad801d001 1e000000000000 0
B: ABS=100000000
B: MSC=10

[/run/muos/storage/application/keyboard]# hexdump -C /dev/input/event4
00000000  32 d1 70 6a 00 00 00 00  8f 94 08 00 00 00 00 00  |2.pj............|
00000010  04 00 04 00 0e 00 07 00  32 d1 70 6a 00 00 00 00  |........2.pj....|
00000020  8f 94 08 00 00 00 00 00  01 00 25 00 01 00 00 00  |..........%.....|
00000030  32 d1 70 6a 00 00 00 00  8f 94 08 00 00 00 00 00  |2.pj............|
00000040  00 00 00 00 00 00 00 00  32 d1 70 6a 00 00 00 00  |........2.pj....|
00000050  74 0b 0a 00 00 00 00 00  04 00 04 00 0e 00 07 00  |t...............|
00000060  32 d1 70 6a 00 00 00 00  74 0b 0a 00 00 00 00 00  |2.pj....t.......|

```

```c
fd = open("/dev/input/event4", O_RDONLY);
```
---
# Summary

This example demonstrates:

1. Installing Bluetooth support on the Anbernic RG40XX H;
2. Connecting a Bluetooth HID keyboard;
3. Checking Linux input registration;
4. Using `/dev/input/event3`;
5. Reading keyboard events in C;
6. Converting Linux key codes into characters;
7. Creating a simple text input system.

This method can later be extended for:

- SDL2 applications;
- game menus;
- text input systems;
- custom game engines;
- external HID controllers.
