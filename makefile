CC = gcc
CFLAGS = -lSDL2 -lSDL2_image -lSDL2_mixer
OBJECTS = ll.o entity.o main.o
HEADERS = ll.h entity.h rand.h

toohoo: $(OBJECTS) $(HEADERS) 
	$(CC) $(CFLAGS) $(OBJECTS) -o toohoo

debug: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -g -o toohoo

main.o: main.c ll.h entity.h rand.h
	$(CC) $(CFLAGS) main.c -c -o main.o

ll.o: ll.c ll.h
	$(CC) $(CFLAGS) ll.c -c -o ll.o

entity.o: entity.c entity.h rand.h
	$(CC) $(CFLAGS) entity.c -c -o entity.o

clean:
	rm -f *.o

purge:
	rm -f *.o
	rm -f toohoo
	
