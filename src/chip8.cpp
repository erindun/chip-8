#include <iostream>
#include <iomanip>
#include <fstream>
#include <SDL2/SDL.h>
#include <memory>

#include "chip8.h"

// CHIP-8 fontset
std::array<uint8_t, 80> fontset {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

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

// Constructor
Chip8::Chip8()
    : opcode {0}
    , I {0}
    , pc {0x200}
    , sp {0}
    , delay_timer {0}
    , sound_timer {0}
    , draw_flag {false}
    , is_running {true}
    , extended_resolution {false}
{
    stack.fill(0);
    memory.fill(0);
    V.fill(0);
    gfx.fill(0);
    keys.fill(0);

    // Load fontset into memory
    for (int i = 0; i < 80; i++)
    {
        memory[i] = fontset[i];
    }
}

// Open ROM and load into memory
bool Chip8::load(const char* filepath)
{
    std::cout << "Loading ROM: " << filepath << std::endl;
    std::ifstream rom(filepath, std::ifstream::binary);

    // Verify that ROM opens correctly
    if (!rom)
    {
        std::cerr << "Error: Failed to open ROM" << std::endl;
        return false;
    }

    // Find size of ROM
    rom.seekg(0, std::ifstream::end);
    int rom_size = rom.tellg();
    rom.clear();
    rom.seekg(0, std::ifstream::beg);

    
    // Allocate memory for ROM
    auto rom_buffer = std::make_unique<char[]>(rom_size);
    if (rom_buffer == nullptr)
    {
        std::cerr << "Error: Failed memory allocation for ROM" << std::endl;
        return false;
    }

    // Copy rom into buffer
    rom.read(rom_buffer.get(), rom_size);

    // Verify that ROM is of valid size
    // Available memory space begins at address 0x200
    if (rom_size > (4096 - 0x200))
    {
        std::cerr << "Error: ROM too large to load into memory" << std::endl;
        return false;
    }

    // Load ROM into memory
    for (int i = 0; i < rom_size; i++)
    {
        memory[i + 0x200] = rom_buffer[i];
    }

    // Clean up
    rom.close();

    return true;
}

// Complete one emulation cycle
void Chip8::cycle()
{
    // Fetch opcode 
    opcode = memory[pc] << 8 | memory[pc + 1];

    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint16_t nnn = opcode & 0x0FFF;
    uint16_t nn = opcode & 0x00FF;
    uint16_t n = opcode & 0x000F;

    bool increment_pc = true;

    // Decode and execute opcode
    switch (opcode & 0xF000)
    {
        case 0x0000:
            switch (opcode & 0x00FF)
            {
                // 00E0: Clears the screen
                case 0x00E0:
                    if (extended_resolution)
                    {
                        gfx.fill(0);
                    }
                    else
                    {
                        gfx_extended.fill(0);
                    }                    
                    draw_flag = true;
                    break;

                // 00EE: Returns from subroutine
                case 0x00EE:
                    sp--;
                    pc = stack[sp];
                    break;

                // 00FD (Super-CHIP): Exit CHIP interpreter
                case 0x00FD:
                    is_running = false;
                    break;

                // 00FE (Super-CHIP): Disable extended mode
                case 0x00FE:
                    extended_resolution = false;
                    break;

                // 00FF (Super-CHIP): Enable extended mode
                case 0x00FF:
                    extended_resolution = true;
                    break;

                default:
                    // Return erroneous opcode
                    std::cerr << "Error: unknown opcode 0x" << std::hex
                              << std::uppercase << std::setfill('0')
                              << std::setw(4) << opcode << std::endl;
            }
            break;

        // 1NNN: Jumps to address NNN
        case 0x1000:
            pc = nnn;
            increment_pc = false;
            break;

        // 2NNN: Calls subroutine at NNN
        case 0x2000:
            stack[sp] = pc;
            sp++;
            pc = nnn;
            increment_pc = false;
            break;

        // 3XNN: Skips next instruction if VX equals NN
        case 0x3000:
            if (V[x] == nn)
            {
                pc += 2;
            }
            break;
        
        // 4XNN: Skips next instruction if VX doesn't equal NN
        case 0x4000:
            if (V[x] != nn)
            {
                pc += 2;
            }
            break;

        // 5XY0: Skips next instruction if VX equals VY
        case 0x5000:
            if (V[x] == V[y])
            {
                pc += 2;
            }
            break;

        // 6XNN: Sets VX to NN
        case 0x6000:
            V[x] = nn;
            break;

        // 7XNN: Adds NN to VX
        case 0x7000:
            V[x] += nn;
            break;

        case 0x8000:
            switch (opcode & 0x000F)
            {
                // 8XY0: Sets VX to the value of VY
                case 0x0000:
                    V[x] = V[y];
                    break;

                // 8XY1: Sets VX to VX or VY
                case 0x0001:
                    V[x] |= V[y];
                    break; 

                // 8XY2: Sets VX to VX and VY
                case 0x0002:
                    V[x] &= V[y];
                    break;

                // 8XY3: Sets VX to VX xor VY
                case 0x0003:
                    V[x] ^= V[y];
                    break;

                // 8XY4: Adds VY to VX. VF is set to 0 when there's a borrow,
                // and 1 when there isn't
                case 0x0004:
                    V[x] += V[y];
                    if (V[y] > (0xFF - V[x]))
                    {
                        V[0xF] = 1; // Carry
                    }
                    else
                    {
                        V[0xF] = 0;
                    }               
                    break;

                // 8XY5: VY is subtracted from VX. VF is set to 0 when there's
                // a borrow, and 1 when there isn't
                case 0x0005:
                    if (V[y] > (V[x]))
                    {
                        V[0xF] = 0; // Borrow
                    }
                    else
                    {
                        V[0xF] = 1;
                    }
                    V[x] -= V[y];               
                    break;

                // 8XY6: Stores the least significant bit of VX in VF and then
                // shifts VX to the right by 1. 
                case 0x0006:
                    V[0xF] = V[x] & 0x1;
                    V[x] >>= 1;
                    break;
                
                // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's
                // a borrow, and 1 when there isn't
                case 0x0007:
                    if (V[x] > V[y])
                    {
                        V[0xF] = 0; // Borrow
                    }
                    else
                    {
                        V[0xF] = 1;
                    }
                    V[x] = V[y] - V[x];
                    break;
                
                // 8XYE: Stores the most significant bit of VX in VF and then
                // shifts VX to the left by one
                case 0x000E:
                    V[0xF] = V[x] >> 7;
                    V[x] <<= 1;
                    break;              

                default:
                    // Return erroneous opcode
                    std::cerr << "Error: unknown opcode 0x" << std::hex
                              << std::uppercase << std::setfill('0')
                              << std::setw(4) << opcode << std::endl;
            }
            break;

        // 9XY0: Skips the next instruction if VX doesn't equal VY
        case 0x9000:
            if(V[x] != V[y])
            {
                pc += 2;
            }
            break;

        // ANNN: Sets I to the address NNN
        case 0xA000:
            I = nnn;
            break;
        
        // BNNN: Jumps to the address NNN plus V0
        case 0xB000:
            pc = nnn + V[0];
            increment_pc = false;
            break;
        
        // CXNN: Sets VX to the result of a bitwise and operation on a random
        // number 0-255 and NN
        case 0xC000:
            V[x] = (rand() % 256) & nn;
            break;

        // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 
        // pixels and a height of N pixels. Each row of 8 pixels is read as
        // bit-coded starting from memory location I; I value doesn’t change
        // after the execution of this instruction. As described above, VF is
        // set to 1 if any screen pixels are flipped from set to unset when
        // the sprite is drawn, and to 0 if that doesn’t happen 
        case 0xD000:
        {
            uint16_t pixel;

            V[0xF] = 0;
            for (int yline = 0; yline < n; yline++)
            {
                pixel = memory[I + yline];
                for (int xline = 0; xline < 8; xline++)
                {
                    if ((pixel & (0x80 >> xline)) != 0)
                    {
                        if (gfx[V[x] + xline + ((V[y] + yline) * 64)] == 1)
                        {   
                            V[0xF] = 1;
                        }
                        gfx[V[x] + xline + ((V[y] + yline) * 64)] ^= 1;
                    }
                }
            }
            draw_flag = true;
            break;
        }

        case 0xE000:
            switch (opcode & 0x00FF)
            {
                // EX9E: Skips the next instruction if the key stored in VX
                // is pressed
                case 0x009E:
                    if (keys[V[x]] != 0)
                    {
                        pc += 2;
                    }
                    break;

                // EXA1: Skips the next instruction if the key stored in VX
                // isn't pressed
                case 0x00A1:
                    if (keys[V[x]] == 0)
                    {
                        pc += 2;
                    }
                    break;

                default:
                    std::cerr << "Error: unknown opcode 0x" << std::hex
                              << std::uppercase << std::setfill('0')
                              << std::setw(4) << opcode << std::endl;
            }
            break;


        case 0xF000:
            switch (opcode & 0x00FF)
            {
                // FX07: Sets VX to the value of the delay timer
                case 0x0007:
                    V[x] = delay_timer;
                    break;

                // FX0A: A key press is awaited, and then stored in VX
                case 0x000A:
                {
                    bool key_pressed;
                    
                    for (int i = 0; i < 16; i++)
                    {
                        if( keys[i] != 0)
                        {
                            V[x] = i;
                            key_pressed = true;
                        }
                    }

                    // No key is pressed
                    if (!key_pressed) return;
                    break;
                }

                // FX15: Sets the delay timer to VX
                case 0x0015:
                    delay_timer = V[x];
                    break;

                // FX18: Sets the delay timer to VX
                case 0x0018:
                    sound_timer = V[x];
                    break;

                // FX1E: Adds VX to I
                // (Undocumented feature: set VF to 1 if there is range 
                // overflow (when (I + VX) > 0xFFF), set VF to 0 if
                // there is not)
                case 0x001E:
                    if (I + V[x] > 0xFFF)
                    {
                        V[0xF] = 1;
                    }
                    else
                    {
                        V[0xF] = 0;
                    }
                    I += V[x];
                    break;

                // FX29: Sets I to the location of the sprite in character VX.
                // Characters 0-F (in hexadecimal) are represented by a 4x5
                // font 
                case 0x0029:
                    I = V[x] * 5;
                    break;

                // FX30 (Super-CHIP): Sets I to the location of the sprite in
                // digit VX
                case 0x0030:
                    I = V[x] * 10;
                    break;

                // Stores the binary-coded decimal representation of VX, with
                // the most significant of three digits at the address in I,
                // the middle digit at I plus 1, and the least significant
                // digit at I plus 2
                case 0x0033:
                    memory[I]     = V[x] / 100;
                    memory[I + 1] = (V[x] / 10) % 10;
                    memory[I + 2] = (V[x] % 100) % 10;
                    break;

                // FX55: Stores V0 to VX (including VX) in memory starting at
                // address I. The offset from I is increased by 1 for each
                // value written, but I itself is left unmodified
                case 0x0055:
                    for (int i = 0; i <= x; i++)
                    {
                        memory[I + i] = V[i];
                    }
                    break;

                // FX65: Fills V0 to VX (including VX) with values from memory
                // starting at address I. The offset from I is increased by 1
                // for each value written, but I itself is left unmodified
                case 0x0065:
                    for (int i = 0; i <= x; i++)
                    {
                        V[i] = memory[I + i];
                    }
                    break;

                default:
                    // Return erroneous opcode
                    std::cerr << "Error: unknown opcode 0x" << std::hex
                              << std::uppercase << std::setfill('0')
                              << std::setw(4) << opcode << std::endl;
            }
            break;

        default:
            // Return erroneous opcode
            std::cerr << "Error: unknown opcode 0x" << std::hex
                      << std::uppercase << std::setfill('0')
                      << std::setw(4) << opcode << std::endl;
    }

    if (increment_pc) pc += 2;

    // Update timers
    if (delay_timer > 0) delay_timer--;
    if (sound_timer > 0) sound_timer--;
    if (sound_timer == 0)
    {
        // TODO: Play sound
    }
}

// Receive input from keyboard
// Returns false if user requests quit
void Chip8::process_input()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT) is_running = false; // Quit

        // Keydown events
        if (e.type == SDL_KEYDOWN)
        {
            if (e.key.keysym.sym == SDLK_ESCAPE) is_running = false;; // Quit

            for (int i = 0; i < 16; i++)
            {
                if (e.key.keysym.sym == keyboard[i]) 
                {
                    keys[i] = 1;
                }
            }
        }

        // Keyup events
        if (e.type == SDL_KEYUP)
        {
            for (int i = 0; i < 16; i++)
            {
                if (e.key.keysym.sym == keyboard[i]) 
                {
                    keys[i] = 0;
                }
            }
        }
    }
}
