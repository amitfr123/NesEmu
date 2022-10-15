#include "EmuWindows/PaletteWindow.hpp"
#include "WindowUtilities/SdlColorHelper.hpp"

PaletteWindow::PaletteWindow(std::span<const uint32_t> paletteView, GetWorkPaletteFunction getWorkPalette) : 
    _pixelTextureHelper(PALETTE_ROW_SIZE, PALETTE_COL_SIZE, PIXEL_SIZE),
    _workPixelTextureHelper(WORK_PALETTE_ROW_SIZE, WORK_PALETTE_COL_SIZE, WORK_PIXEL_SIZE),
    BaseWindow("PaletteWindow", {0,0, PALETTE_ROW_SIZE * PIXEL_SIZE, PALETTE_COL_SIZE * PIXEL_SIZE + 10 + WORK_PIXEL_SIZE}, 0, COLOR_WHITE),
    _paletteView(std::move(paletteView)),
    _getWorkPalette(std::move(getWorkPalette))
{
    _pixelTextureHelper.fillTexture(paletteView);
}

void PaletteWindow::renderWindow()
{
    SDL_RenderClear(_renderer.get());
    SDL_Rect rect = _pixelTextureHelper.getTextureRect();
    SDL_RenderCopy(_renderer.get(), _pixelTextureHelper.getTexture(_renderer).get(), NULL, &rect);
    for (uint8_t i = 0; i < 8; ++i)
    {
        auto pal = _getWorkPalette(i);
        _workPixelTextureHelper.fillTexture(std::span<uint32_t>(pal.begin(), pal.size()));
        SDL_Rect work_rect = _workPixelTextureHelper.getTextureRect();
        work_rect.y = rect.h + 10;
        work_rect.x = work_rect.w * i + 20 * i;
        SDL_RenderCopy(_renderer.get(), _workPixelTextureHelper.getTexture(_renderer).get(), NULL, &work_rect);
    }
    SDL_RenderPresent(_renderer.get());
}