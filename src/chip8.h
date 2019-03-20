#ifndef CHIP8_H
#define CHIP8_H
#include <array>
#include <cstdint>

class Chip8
{
    private:
        uint16_t opcode;
        uint16_t I;
        uint16_t pc;

        std::array<uint8_t, 4096> memory;
        std::array<uint8_t, 16> V;

        std::array<uint16_t, 16> stack;
        uint16_t sp;

        uint8_t delay_timer;
        uint8_t sound_timer;

    public:
        std::array<uint8_t, 16> keys;
        std::array<uint8_t, 64 * 32> gfx;
        std::array<uint8_t, 128 * 64> gfx_extended;
        bool draw_flag;
        bool is_running;
        bool extended_resolution;

        Chip8();
        bool load(const char filepath[]);
        void cycle();
        void process_input();
};

#endif // CHIP8_H