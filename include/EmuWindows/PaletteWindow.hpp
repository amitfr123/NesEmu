#pragma once 

#include <span>

#include "BaseWindow.hpp"
#include "WindowUtilities/PixelTextureHelper.hpp"

class PaletteWindow : public BaseWindow {
public:
    PaletteWindow(std::span<const uint32_t> paletteView);

    void renderWindow() override;
private:
    static constexpr uint32_t PALETTE_ROW_SIZE = 16;
    static constexpr uint32_t PALETTE_COL_SIZE = 4;
    static constexpr uint32_t PIXEL_SIZE = 20;

    std::span<const uint32_t> _paletteView;
    PixelTextureHelper _pixelTextureHelper;
};