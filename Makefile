# Compiler gcc/g++
CC = g++

# Compiler Flags
CXXFLAGS = -g -c

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf

game : game.o labyrinthe.o player.o virus.o potion.o
	$(CC) -g -o $@ $^ $(LINKER_FLAGS)

game.o : game.cpp SDL_FUNCs.h labyrinthe.h virus.h potion.h player.h
	$(CC) $(CXXFLAGS) $<

labyrinthe.o : labyrinthe.cpp SDL_FUNCs.h labyrinthe.h
	$(CC) $(CXXFLAGS) $<

player.o : player.cpp labyrinthe.h player.h
	$(CC) $(CXXFLAGS) $<

virus.o : virus.cpp labyrinthe.h virus.h
	$(CC) $(CXXFLAGS) $<

potion.o : potion.cpp labyrinthe.h potion.h
	$(CC) $(CXXFLAGS) $<

.PHONY = clean

clean : 
	rm -f game *.o
