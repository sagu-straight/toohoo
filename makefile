CC = gcc
CFLAGS = -lSDL2 -lSDL2_image -lSDL2_mixer
OBJECTS = ll.o entity.o main.o

toohoo: $(OBJECTS) 
	$(CC) $(CFLAGS) $(OBJECTS) -o toohoo

debug: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -g -o toohoo

main.o: main.c
	$(CC) $(CFLAGS) main.c -c -o main.o

ll.o: ll.c
	$(CC) $(CFLAGS) ll.c -c -o ll.o

entity.o: entity.c
	$(CC) $(CFLAGS) entity.c -c entity.o

clean:
	rm -f *.o

purge:
	rm -f *.o
	rm -f toohoo
	
