#pragma once

#include <string>
#include <span>

#include "BaseWindow.hpp"

#include "WindowUtilities/MouseWheelScrollHelper.hpp"
#include "WindowUtilities/OutputTextHelper.hpp"

class MemoryWindow : public BaseWindow {
public:
    MemoryWindow(std::span<const uint8_t> ramView);
    void renderWindow() override;
private:
    static constexpr uint32_t SCROLL_SENSITIVITY = 10;
    static constexpr uint32_t FONT_SIZE = 10;
    static constexpr SDL_Rect WINDOW_RECT = {0, 0, 625, 500};

    std::vector<std::string> RamViewBuilder();
    void MoveYIndex(int amount);

    std::span<const uint8_t> _ramView;
    OutputTextHelper _outputTextHelper;
    MouseWheelScrollHelper _scrollHelper;
    int _yIndex;
};