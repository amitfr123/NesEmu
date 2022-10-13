#include "WindowUtilities/PixelTextureHelper.hpp"

PixelTextureHelper::PixelTextureHelper(uint32_t width, uint32_t height, uint32_t pixelSize) : 
    _pixelSize(pixelSize),
    _surface(SDL_CreateRGBSurface(0, width * pixelSize, height * pixelSize, 32, 0, 0, 0, 0), SDL_FreeSurface)
{
    if (_surface.get() == nullptr)
    {
        throw std::runtime_error("failed to create pixel surface");
    }
}

SDL_Rect PixelTextureHelper::getTextureRect()
{
    return {0,0, static_cast<int>(_surface->w), static_cast<int>(_surface->h)};
}

std::shared_ptr<SDL_Texture> PixelTextureHelper::getTexture(std::shared_ptr<SDL_Renderer> renderer)
{
    std::shared_ptr<SDL_Texture> texture(SDL_CreateTextureFromSurface(renderer.get(), _surface.get()), SDL_DestroyTexture);
    return texture;
}

void PixelTextureHelper::clearTexture()
{
    SDL_LockSurface(_surface.get());
    SDL_memset(_surface->pixels, 0, _surface->h * _surface->pitch);
    SDL_UnlockSurface(_surface.get());
}

void PixelTextureHelper::fillTexture(std::span<const uint32_t> pixelSource)
{
    uint32_t num = 0;
    SDL_LockSurface(_surface.get());
    for (auto iter : pixelSource)
    {
        uint8_t r = ((iter & 0xff0000) >> 16);
        uint8_t g = ((iter & 0xff00) >> 8);
        uint8_t b = (iter & 0xff);
        drawPixel(r, g, b, num % (_surface->w / _pixelSize), num / (_surface->w / _pixelSize));
        num++;
    }
    SDL_UnlockSurface(_surface.get());
}

void PixelTextureHelper::drawPixel(uint8_t r, uint8_t g, uint8_t b, uint32_t x, uint32_t y)
{
    uint32_t pixelVal = SDL_MapRGBA(_surface->format, r, g, b, 255);
    SDL_Rect pixel = {static_cast<int>(x * _pixelSize), static_cast<int>(y * _pixelSize), static_cast<int>(_pixelSize), static_cast<int>(_pixelSize)};
    SDL_FillRect(_surface.get(), &pixel, pixelVal);
}