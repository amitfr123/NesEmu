#pragma once 

#include <functional>

#include "BaseWindow.hpp"
#include "WindowUtilities/PixelTextureHelper.hpp"

class PatternWindow : public BaseWindow {
public:
    using GetPatternFunction = std::function<std::array<std::array<uint32_t, 0x4000>, 2> (uint8_t)>;
    PatternWindow(GetPatternFunction getPattern);

    void renderWindow() override;
private:
    static constexpr uint32_t PATTERN_ROW_SIZE = 128;
    static constexpr uint32_t PATTERN_COL_SIZE = PATTERN_ROW_SIZE;
    static constexpr uint32_t PIXEL_SIZE = 2;

    void handleKeyDown(const SDL_Event& e);

    GetPatternFunction _getPattern;
    PixelTextureHelper _pixelTextureHelper;
    uint8_t _paletteId;
};