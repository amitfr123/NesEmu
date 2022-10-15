#pragma once 

#include <span>
#include <functional>
#include "BaseWindow.hpp"
#include "WindowUtilities/PixelTextureHelper.hpp"

class ScreenWindow : public BaseWindow {
public:
    ScreenWindow(std::span<const uint32_t> screenView);

    void readNewFrame();

    void renderWindow() override;
private:
    static constexpr uint32_t SCREEN_ROW_SIZE = 240;
    static constexpr uint32_t SCREEN_COL_SIZE = 256;
    static constexpr uint32_t PIXEL_SIZE = 3;

    std::span<const uint32_t> _screenView;
    std::array<uint32_t, 0xf000> _screenViewOut;
    PixelTextureHelper _pixelTextureHelper;
};