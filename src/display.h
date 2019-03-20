#ifndef DISPLAY_H
#define DISPLAY_H
#include <array>
#include <SDL2/SDL.h>

bool setup_graphics();
void draw(std::array<uint8_t, 64 * 32> gfx);
void draw(std::array<uint8_t, 128 * 64> gfx); // Overload for extended resolution
void close_window();

#endif // DISPLAY_H