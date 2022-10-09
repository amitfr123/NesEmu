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

    void renderWindow() override;
private:
    void LineChecker(const std::string&);

    static constexpr uint32_t FONT_SIZE = 20;
    static constexpr SDL_Rect WINDOW_RECT = {0, 0, 500, 24};

    InputTextLineHelper _inputTextLineHelper;
    OutputTextHelper _outputTextHelper;
    TransferFile _transferFunction;
};