#include "EmuWindows/PaletteWindow.hpp"
#include "WindowUtilities/SdlColorHelper.hpp"

PaletteWindow::PaletteWindow(std::span<const uint32_t> paletteView) : 
    _pixelTextureHelper(PALETTE_ROW_SIZE, PALETTE_COL_SIZE, PIXEL_SIZE),
    BaseWindow("PaletteWindow", {0,0, PALETTE_ROW_SIZE * PIXEL_SIZE, PALETTE_COL_SIZE * PIXEL_SIZE}, 0, COLOR_WHITE),
    _paletteView(std::move(paletteView))
{
    _pixelTextureHelper.fillTexture(paletteView);
}

void PaletteWindow::renderWindow()
{
    SDL_RenderClear(_renderer.get());
    SDL_Rect rect = _pixelTextureHelper.getTextureRect();
    SDL_RenderCopy(_renderer.get(), _pixelTextureHelper.getTexture(_renderer).get(), NULL, &rect);
    SDL_RenderPresent(_renderer.get());
}