#pragma once

#include <string>

#include "HardwareEmulation/Mmu.hpp"

#include "BaseWindow.hpp"

#include "WindowUtilities/MouseWheelScrollHelper.hpp"
#include "WindowUtilities/OutputTextHelper.hpp"

class MemoryWindow : public BaseWindow {
public:
    using GetRamFunction = std::function<const std::array<uint8_t, 0x800>& ()>;

    MemoryWindow(GetRamFunction get_ram_function);
    void RenderWindow() override;
    ~MemoryWindow() = default;
private:
    static constexpr uint32_t SCROLL_SENSITIVITY = 10;
    static constexpr uint32_t FONT_SIZE = 10;
    static constexpr SDL_Rect WINDOW_RECT = {0, 0, 625, 500};

    std::vector<std::string> RamViewBuilder();
    void MoveYIndex(int amount);

    GetRamFunction _get_ram_function;
    OutputTextHelper _output_text_helper;
    MouseWheelScrollHelper _scroll_helper;
    int _y_index;
};