#include <SDL2/SDL_ttf.h>

#include <filesystem>

#include "EmuWindows/FileLoadingWindow.hpp"
#include "WindowUtilities/SdlColorHelper.hpp"

FileLoadingWindow::FileLoadingWindow(TransferFile transfer_function) :  
    BaseWindow("File_Load_Window", WINDOW_RECT, 0, COLOR_RED),
    _transferFunction(std::move(transfer_function)),
    _inputTextLineHelper(_eventMapper, std::bind(&FileLoadingWindow::LineChecker, this, std::placeholders::_1)),
    _outputTextHelper(std::string(RESOURCE_PATH) + "/fonts/PressStart2P.ttf", FONT_SIZE, COLOR_WHITE)
{
}

void FileLoadingWindow::renderWindow()
{
    SDL_RenderClear(_renderer.get());
    std::string line = _inputTextLineHelper.GetLine();
    std::shared_ptr<SDL_Texture> texture(_outputTextHelper.CreateTextTexture(line, _renderer));
    SDL_Rect window_rect;
    SDL_GetWindowSize(_window.get(), &window_rect.w, &window_rect.h);
    SDL_Rect line_rect = _outputTextHelper.GetTrueTextRectangleDim(line);
    line_rect.x = (window_rect.w < line_rect.w)? window_rect.w - line_rect.w : 0;
    line_rect.y = ((window_rect.h > FONT_SIZE)? (window_rect.h - FONT_SIZE) / 2 + 1 : 0);

    SDL_RenderCopy(_renderer.get(), texture.get(), NULL, &line_rect);
    SDL_RenderPresent(_renderer.get());
}

// LineChecker This function checks the line so we will be able to load a file from the fs
void FileLoadingWindow::LineChecker(const std::string& str)
{
    bool flag = false;
    if (std::filesystem::exists(str) && str.ends_with(".nes"))
    {
        _transferFunction(str);
        flag = true;
    }
    _inputTextLineHelper.SetAfterEnterMessage(((flag)? "SUCCESS" : "FAIL"));
}