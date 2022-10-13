#pragma once 

#include <memory>
#include <span>
#include <SDL2/SDL.h>

class PixelTextureHelper {
public:

    PixelTextureHelper(uint32_t width, uint32_t height, uint32_t pixelSize);

    SDL_Rect getTextureRect();

    std::shared_ptr<SDL_Texture> getTexture(std::shared_ptr<SDL_Renderer> renderer);

    void clearTexture();

    void fillTexture(std::span<const uint32_t> pixelSource);

private:
    void drawPixel(uint8_t r, uint8_t g, uint8_t b, uint32_t x, uint32_t y);

    std::shared_ptr<SDL_Surface> _surface;
    uint32_t _pixelSize;
};