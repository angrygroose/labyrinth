# Compiler gcc/g++
CC = g++

# Compiler Flags
CXXFLAGS = -g -I./lib

# LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf

# Source files directory
SRC_DIR = src/

# Build directory where object (.o) will be put
BUILD_DIR := ./build

# Program library where .h modular library files exits
LIB_DIR := lib/

game : $(BUILD_DIR)/game.o $(BUILD_DIR)/labyrinthe.o $(BUILD_DIR)/player.o $(BUILD_DIR)/virus.o $(BUILD_DIR)/potion.o
	$(CC) -g -o $@ $^ $(LINKER_FLAGS)

$(BUILD_DIR)/game.o : $(SRC_DIR)game.cpp $(LIB_DIR)SDL_FUNCs.h $(SRC_DIR)labyrinthe.h $(SRC_DIR)virus.h $(SRC_DIR)potion.h $(SRC_DIR)player.h
	$(CC) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/labyrinthe.o : $(SRC_DIR)labyrinthe.cpp $(LIB_DIR)SDL_FUNCs.h $(SRC_DIR)labyrinthe.h
	$(CC) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/player.o : $(SRC_DIR)player.cpp $(SRC_DIR)labyrinthe.h $(SRC_DIR)player.h
	$(CC) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/virus.o : $(SRC_DIR)virus.cpp $(SRC_DIR)labyrinthe.h $(SRC_DIR)virus.h
	$(CC) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/potion.o : $(SRC_DIR)potion.cpp $(SRC_DIR)labyrinthe.h $(SRC_DIR)potion.h
	$(CC) -c $(CXXFLAGS) $< -o $@

.PHONY = clean

clean :
	rm -f game *.o
	rm -f build/*.o
