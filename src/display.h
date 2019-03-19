#ifndef DISPLAY_H
#define DISPLAY_H
#include <SDL2/SDL.h>

bool setup_graphics();
void draw(uint8_t gfx[]);
void close_window();

#endif // DISPLAY_H