#pragma once 

#include <string>
#include <fstream>

#include "BaseWindow.hpp"

#include "WindowUtilities/InputTextLineHelper.hpp"
#include "WindowUtilities/OutputTextHelper.hpp"

class FileLoadingWindow : public BaseWindow {
public:
    using TransferFile = std::function<void (std::string file_path)>;
    FileLoadingWindow(TransferFile transfer_function);

    void RenderWindow() override;
    ~FileLoadingWindow() = default;
private:
    void LineChecker(const std::string&);

    static constexpr uint32_t FONT_SIZE = 20;
    static constexpr SDL_Rect WINDOW_RECT = {0, 0, 500, 24};

    InputTextLineHelper _input_text_line_helper;
    OutputTextHelper _output_text_helper;
    TransferFile _transfer_function;
};