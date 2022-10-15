#pragma once 

#include <span>
#include <functional>
#include "BaseWindow.hpp"
#include "WindowUtilities/PixelTextureHelper.hpp"

class PaletteWindow : public BaseWindow {
public:
    using GetWorkPaletteFunction = std::function<std::array<uint32_t, 4> (uint8_t)>;
    PaletteWindow(std::span<const uint32_t> paletteView, GetWorkPaletteFunction getWorkPalette);

    void renderWindow() override;
private:
    static constexpr uint32_t PALETTE_ROW_SIZE = 16;
    static constexpr uint32_t PALETTE_COL_SIZE = 4;
    static constexpr uint32_t PIXEL_SIZE = 40;

    static constexpr uint32_t WORK_PALETTE_ROW_SIZE = 4;
    static constexpr uint32_t WORK_PALETTE_COL_SIZE = 1;
    static constexpr uint32_t WORK_PIXEL_SIZE = 15;

    std::span<const uint32_t> _paletteView;
    PixelTextureHelper _pixelTextureHelper;
    PixelTextureHelper _workPixelTextureHelper;
    GetWorkPaletteFunction _getWorkPalette;
};