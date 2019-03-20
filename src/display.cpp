#include <iostream>
#include <cstdint>
#include "display.h"

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

bool setup_graphics()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cout << "SDL initialization failure. SDL_Error: "
                  << SDL_GetError() << std::endl;
        return false;
    }

    //Create window
    window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                              SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        std::cout << "Failed to create window. SDL_Error: " 
                  << SDL_GetError() << std::endl;
        return false;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (renderer == nullptr)
    {
        std::cout << "Failed to create renderer. SDL_Error: " 
                  << SDL_GetError() << std::endl;
        return false;
    }

    // Create texture that stores frame buffer
    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                64, 32);
    if (texture == nullptr)
    {
        std::cout << "Failed to create texture. SDL_Error: " 
                  << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void draw(uint8_t gfx[])
{
    uint32_t pixels[64 * 32];

    // Store pixels in temporary buffer
    for (int i = 0; i < 64 * 32; ++i)
    {
        uint8_t pixel = gfx[i];
        pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
    }

    // Update texture
    SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(uint32_t));

    // Clear screen and render
    SDL_RenderClear(renderer);  
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void close_window()
{
    SDL_DestroyWindow(window);
    window = nullptr;
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyTexture(texture);
    texture = nullptr;
    SDL_Quit();
}