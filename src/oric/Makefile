# Nom du programme
TARGET = oric

# Sources
SRC = \
	main.c \
	ula.c \
	videoMemLores.c \
	videoMemHires.c \
	video.c \
    audio.c \
    emu2149.c \
    input.c \
    bus.c \
    oric_keyboard.c \
    rom.c \
    timing.c \
    via.c \
    vrEmu6502.c \
    charset.c \
    keyboard.c \
    linux_ps2.c \
    sd_reader.c \
    autocmd.c \
    config.c


#==========================
# Compilation PC (SDL2)
#==========================

CC_PC = gcc

CFLAGS_PC = -Wall -O2 \
	$(shell pkg-config --cflags sdl2)

LIBS_PC = $(shell pkg-config --libs sdl2)

pc:
	$(CC_PC) $(SRC) $(CFLAGS_PC) $(LIBS_PC) -o $(TARGET)-pc


#==========================
# Compilation ARM64
#==========================

CC_ARM64 = aarch64-linux-gnu-gcc

SDL2_ARM64 = ../../SDL2-SDK-arm64

CFLAGS_ARM64 = -Wall -O2 \
	-I$(SDL2_ARM64)/include

LIBS_ARM64 = \
	-L$(SDL2_ARM64)/lib \
	-lSDL2

arm64:
	$(CC_ARM64) $(SRC) $(CFLAGS_ARM64) $(LIBS_ARM64) -o $(TARGET)-arm64


#==========================
# Nettoyage
#==========================

clean:
	rm -f $(TARGET)-pc $(TARGET)-arm64
