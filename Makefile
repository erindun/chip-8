OBJS = chip8.o display.o main.o

CXX = g++

CXXFLAGS = -Wall -Werror

# Compile for Windows (MinGW) and Linux
ifeq ($(OS), Windows_NT)
	LFLAGS = -lmingw32 -lSDL2main -lSDL2
else
	LFLAGS = -lSDL2
endif

output: $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) $(LFLAGS) -o chip8

main.o: src/main.cpp
	$(CXX) -c src/main.cpp

chip8.o: src/chip8.cpp
	$(CXX) -c src/chip8.cpp

display.o: src/display.cpp
	$(CXX) -c src/display.cpp

clean:
	rm *.o chip8
