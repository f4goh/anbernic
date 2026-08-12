# ==========================================================
# Sokoban SDL2 / C++
# ==========================================================

TARGET = sokoban

SRC = main.cpp \
      Game.cpp \
      Level.cpp \
      Player.cpp \
      Crate.cpp \
      Renderer.cpp \
      Input.cpp \
      Audio.cpp \
      Levels.cpp \
      Intro.cpp \
      SelLevel.cpp

OBJ = $(SRC:.cpp=.o)


# ==========================================================
# Compilation PC
# ==========================================================

CXX_PC = g++

CXXFLAGS_PC = -std=c++17 -Wall -Wextra -O2 \
              $(shell pkg-config --cflags sdl2 SDL2_image SDL2_mixer SDL2_ttf)

LIBS_PC = $(shell pkg-config --libs sdl2 SDL2_image SDL2_mixer SDL2_ttf)


pc:
	$(CXX_PC) $(CXXFLAGS_PC) $(SRC) $(LIBS_PC) -o $(TARGET)-pc


# ==========================================================
# Compilation ARM64
# ==========================================================

CXX_ARM64 = aarch64-linux-gnu-g++

SDL2_ARM64 = ../../SDL2-SDK-arm64

CXXFLAGS_ARM64 = -std=c++17 -Wall -Wextra -O2 \
                 -I$(SDL2_ARM64)/include

LDFLAGS_ARM64 = \
                -L$(SDL2_ARM64)/lib \
                -Wl,-rpath,'$$ORIGIN'


LIBS_ARM64 = \
             -lSDL2_ttf \
             -lSDL2_mixer \
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
	$(CXX_ARM64) $(CXXFLAGS_ARM64) $(SRC) \
	$(LDFLAGS_ARM64) $(LIBS_ARM64) \
	-o $(TARGET)-arm64


# ==========================================================
# Compilation des deux
# ==========================================================

all: pc arm64


# ==========================================================
# Nettoyage
# ==========================================================

clean:
	rm -f $(TARGET)-pc $(TARGET)-arm64
	rm -f *.o


# ==========================================================
# Exécution PC
# ==========================================================

run:
	./$(TARGET)-pc


