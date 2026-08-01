# Nom du programme
TARGET = testinput

# Sources
SRCS = main.c input.c

# ==========================
# Compilation PC
# ==========================

CC_PC = gcc

CFLAGS_PC = $(shell pkg-config --cflags sdl2)
LIBS_PC   = $(shell pkg-config --libs sdl2)

pc:
	$(CC_PC) $(SRCS) $(CFLAGS_PC) $(LIBS_PC) -o $(TARGET)-pc


# ==========================
# Compilation ARM64
# ==========================

CC_ARM64 = aarch64-linux-gnu-gcc

SDL2_ARM64 = ../../SDL2-SDK-arm64

CFLAGS_ARM64 = -I$(SDL2_ARM64)/include
LIBS_ARM64 = -L$(SDL2_ARM64)/lib -lSDL2

arm64:
	$(CC_ARM64) $(SRCS) $(CFLAGS_ARM64) $(LIBS_ARM64) -o $(TARGET)-arm64


# ==========================
# Nettoyage
# ==========================

clean:
	rm -f $(TARGET)-pc $(TARGET)-arm64
