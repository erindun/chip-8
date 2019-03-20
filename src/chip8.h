#ifndef CHIP8_H
#define CHIP8_H
#include <cstdint>

class Chip8
{
    private:
        uint16_t opcode;
        uint16_t I;
        uint16_t pc;

        uint8_t memory[4096];
        uint8_t V[16];

        uint16_t stack[16];
        uint16_t sp;

        uint8_t delay_timer;
        uint8_t sound_timer;

    public:
        uint8_t keys[16];
        uint8_t gfx[64 * 32];
        bool draw_flag;
        bool is_running;


        Chip8();
        bool load(const char filepath[]);
        void cycle();
        void process_input();
};

#endif // CHIP8_H