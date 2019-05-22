#include <iostream>
#include <cstdlib>
#include <ctime>
#include <SDL2/SDL.h>

#include "input.h"
#include "display.h"
#include "chip8.h"

int main(int argc, char **argv) {
    // Check command line arguments
    if (argc != 2) {
        std::cout << "Usage: chip8 [path to ROM file]" << std::endl;
        return -1;
    }

    // Initialize Chip-8
    Chip8 chip8 = Chip8();

    // Load ROM
    if (!chip8.load(argv[1])) {
        return -1;
    }

    // RNG seed
    srand(time(NULL));
    
    // Setup SDL
    if (!setup_graphics()) {
        return -1;
    }

    // Emulation cycled
    while (chip8.is_running) {
        // Emulate one cycle
        chip8.cycle();
        
        // Receive input from keyboard
        process_input(chip8);

        if (chip8.draw_flag) {
            chip8.draw_flag = false;
            if (chip8.extended_resolution) {
                draw(chip8.gfx_extended);
            } else {
                draw(chip8.gfx);
            }
        }

        SDL_Delay(2.5);
    }

    close_window();

    return 0;
}
