#include <cstring>
#include "EmuWindows/ScreenWindow.hpp"
#include "WindowUtilities/SdlColorHelper.hpp"

ScreenWindow::ScreenWindow(std::span<const uint32_t> screenView) : 
    _pixelTextureHelper(SCREEN_ROW_SIZE, SCREEN_COL_SIZE, PIXEL_SIZE),
    BaseWindow("ScreenWindow", {0,0, SCREEN_ROW_SIZE * PIXEL_SIZE, SCREEN_COL_SIZE * PIXEL_SIZE}, 0, COLOR_WHITE),
    _screenView(std::move(screenView))
{
}

void ScreenWindow::renderWindow()
{
    SDL_RenderClear(_renderer.get());
    _pixelTextureHelper.fillTexture(_screenViewOut);
    SDL_Rect rect = _pixelTextureHelper.getTextureRect();
    SDL_RenderCopy(_renderer.get(), _pixelTextureHelper.getTexture(_renderer).get(), NULL, &rect);
    SDL_RenderPresent(_renderer.get());
}

void ScreenWindow::readNewFrame()
{
    std::memcpy(_screenViewOut.data(), _screenView.data(), sizeof(_screenViewOut));
}