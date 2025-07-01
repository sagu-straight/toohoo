# you can change this to your soy compiler if you want to
CC = gcc
GAME_NAME = toohoo

# magic makefile shit
SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,obj/%.o,$(SRC))
CFLAGS = -lm -lSDL2 -lSDL2_mixer -lSDL2_image -march=native

game: $(OBJ)
	$(CC) obj/* $(CFLAGS) -o $(GAME_NAME)

clean:
	rm -rf obj/*

purge:
	rm -rf obj/*
	rm -f $(GAME_NAME)

# crazy makefile shit 
obj/%.o: src/%.c 
	$(CC) $(CFLAGS) -c $< -o $@

