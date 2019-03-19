SOURCES = src/chip8.cpp src/display.cpp src/main.cpp

OBJS = chip8.o display.o main.o

CC = g++

CFLAGS = -Wall

# Compile for Windows (MinGW) and Linux
ifeq ($(OS), Windows_NT)
	LFLAGS = -lmingw32 -lSDL2main -lSDL2
else
	LFLAGS = -lSDL2
endif

output: $(OBJS)
	$(CC) $(SOURCES) $(CFLAGS) $(LFLAGS) -o chip8

main.o: src/main.cpp
	$(CC) -c src/main.cpp

chip8.o: src/chip8.cpp
	$(CC) -c src/chip8.cpp

display.o: src/display.cpp
	$(CC) -c src/display.cpp

clean:
	rm *.o chip8