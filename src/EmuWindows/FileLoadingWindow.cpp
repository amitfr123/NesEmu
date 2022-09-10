#include <SDL2/SDL_ttf.h>

#include <filesystem>

#include "EmuWindows/FileLoadingWindow.hpp"
#include "WindowUtilities/SdlColorHelper.hpp"

FileLoadingWindow::FileLoadingWindow(TransferFile transfer_function) :  
    BaseWindow("File_Load_Window", WINDOW_RECT, 0, COLOR_RED),
    _transfer_function(std::move(transfer_function)),
    _input_text_line_helper(_event_mapper, std::bind(&FileLoadingWindow::LineChecker, this, std::placeholders::_1)),
    _output_text_helper(std::string(FONT_PATH) + "PressStart2P.ttf", FONT_SIZE, COLOR_WHITE)
{
}

void FileLoadingWindow::RenderWindow()
{
    SDL_RenderClear(_renderer.get());
    const std::string line = _input_text_line_helper.GetLine();
    std::shared_ptr<SDL_Texture> texture(_output_text_helper.CreateTextTexture(line, _renderer));
    SDL_Rect window_rect;
    SDL_GetWindowSize(_window.get(), &window_rect.w, &window_rect.h);
    SDL_Rect line_rect = _output_text_helper.GetTrueTextRectangleDim(line);
    line_rect.x = (window_rect.w < line_rect.w)? window_rect.w - line_rect.w : 0;
    line_rect.y = ((window_rect.h > FONT_SIZE)? (window_rect.h - FONT_SIZE) / 2 + 1 : 0);

    SDL_RenderCopy(_renderer.get(), texture.get(), NULL, &line_rect);
    SDL_RenderPresent(_renderer.get());
}

// LineChecker This function checks the line so we will be able to load a file from the fs
void FileLoadingWindow::LineChecker(const std::string& str)
{
    bool flag = false;
    if (std::filesystem::exists(str))
    {
        _transfer_function(str);
        flag = true;
    }
    _input_text_line_helper.SetAfterEnterMessage(((flag)? "SUCCESS" : "FAIL"));
}