#include <array>
#include <SDL2/SDL.h>
#include "input.h"

// CHIP-8 hex keyboard
std::array<uint8_t, 16> keyboard {
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_4,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_r,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_f,
    SDLK_z,
    SDLK_x,
    SDLK_c,
    SDLK_v
};

// Receive input from keyboard
// Sets Chip8::is_running to false if user requests quit
void process_input(Chip8& chip8) {
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) chip8.is_running = false; // Quit

        // Keydown events
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE) chip8.is_running = false; // Quit

            for (int i = 0; i < 16; i++) {
                if (e.key.keysym.sym == keyboard[i]) {
                    chip8.keys[i] = 1;
                }
            }
        }

        // Keyup events
        if (e.type == SDL_KEYUP) {
            for (int i = 0; i < 16; i++) {
                if (e.key.keysym.sym == keyboard[i]) {
                    chip8.keys[i] = 0;
                }
            }
        }
    }
}
