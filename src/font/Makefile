# Nom du programme
TARGET = testfont

SRC = main.c


# ==========================
# PC
# ==========================

CC_PC = gcc

CFLAGS_PC = $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf)
LIBS_PC   = $(shell pkg-config --libs sdl2 SDL2_image SDL2_ttf)

pc:
	$(CC_PC) $(SRC) $(CFLAGS_PC) $(LIBS_PC) -o $(TARGET)-pc


# ==========================
# ARM64
# ==========================

CC_ARM64 = aarch64-linux-gnu-gcc

SDL2_ARM64 = ../../SDL2-SDK-arm64

CFLAGS_ARM64 = \
	-I$(SDL2_ARM64)/include

LDFLAGS_ARM64 = \
	-L$(SDL2_ARM64)/lib \
	-Wl,-rpath,'$$ORIGIN'

LIBS_ARM64 = \
	-lSDL2_ttf \
	-lSDL2_image \
	-lSDL2 \
	-lharfbuzz \
	-lfreetype \
	-lpng16 \
	-lbz2 \
	-lbrotlidec \
	-lbrotlicommon \
	-lglib-2.0 \
	-lpcre2-8 \
	-lz

arm64:
	$(CC_ARM64) $(SRC) $(CFLAGS_ARM64) $(LDFLAGS_ARM64) $(LIBS_ARM64) -o $(TARGET)-arm64


# ==========================
# Nettoyage
# ==========================

clean:
	rm -f $(TARGET)-pc $(TARGET)-arm64
