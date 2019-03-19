#include <iostream>
#include <cstdlib>
#include <ctime>
#include <SDL2/SDL.h>
#include "display.h"
#include "chip8.h"

int main(int argc, char **argv)
{
    // Check command line arguments
    if (argc != 2)
    {
        std::cout << "Usage: chip8 [path to ROM file]" << std::endl;
        return -1;
    }

    // Initialize Chip-8
    Chip8 chip8 = Chip8();

    // Load ROM
    if (!chip8.load(argv[1]))
    {
        return -1;
    }

    // RNG seed
    srand(time(NULL));
    
    // Setup SDL
    if (!setup_graphics())
    {
        return -1;
    }

    // Emulation cycled
    bool quit = false;
    while (!quit)
    {
        // Emulate one cycle
        chip8.cycle();
        
        // Receive input from keyboard
        // Returns false if user requests quit
        if (!chip8.process_input())
        {
            return false;
        }

        if (chip8.draw_flag)
        {
            chip8.draw_flag = false;
            draw(chip8.gfx);
        }

        SDL_Delay(2.5);
    }

    close_window();

    return 0;
}