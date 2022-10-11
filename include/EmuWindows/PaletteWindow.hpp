#pragma once 

#include <span>

#include "BaseWindow.hpp"
#include "WindowUtilities/PixelTextureHelper.hpp"

class PaletteWindow : public BaseWindow {
public:
    PaletteWindow(std::span<const std::tuple<uint8_t,uint8_t,uint8_t>> paletteView);

    void renderWindow() override;
private:
    static constexpr uint32_t PALETTE_ROW_SIZE = 16;
    static constexpr uint32_t PALETTE_COL_SIZE = 4;
    static constexpr uint32_t PIXEL_SIZE = 20;

    std::span<const std::tuple<uint8_t,uint8_t,uint8_t>> _paletteView;
    PixelTextureHelper _pixelTextureHelper;
};