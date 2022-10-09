#include <vector>

#include "WindowUtilities/SdlColorHelper.hpp"
#include "NumToHexStringConvertor.hpp"


#include "EmuWindows/MemoryWindow.hpp"


MemoryWindow::MemoryWindow(std::span<const uint8_t> ramView) :
    BaseWindow("Memory_View_Window", WINDOW_RECT, 0, COLOR_RED),
    _outputTextHelper(std::string(RESOURCE_PATH) + "/fonts/PressStart2P.ttf", FONT_SIZE, COLOR_WHITE),
    _scrollHelper(_eventMapper, std::bind(&MemoryWindow::MoveYIndex, this, std::placeholders::_1)),
    _ramView(std::move(ramView))
{
    _yIndex = 0;
}

std::vector<std::string> MemoryWindow::RamViewBuilder()
{
    uint32_t line_addr = 0;
    std::vector<std::string> string_vector;
    std::string view;
    for(const uint8_t &mem : _ramView)
    {
        if (line_addr % 16 == 0)
        {
            string_vector.push_back(std::string(view.c_str()));
            view.clear();
            view = "Address " + NumToHexStringConvertor::Convert(line_addr, 4) + ":  ";
        }
        view += NumToHexStringConvertor::Convert(mem, 2) + ",";
        line_addr++;
        if (line_addr % 4 == 0)
        {
            view.pop_back();
            view += " ";
        }
    }
    return std::move(string_vector);
}

void MemoryWindow::renderWindow()
{
    SDL_RenderClear(_renderer.get());
    auto vec = RamViewBuilder();
    SDL_Rect line_rect;
    uint32_t y_index = _yIndex;
    for (const auto& line : vec)
    {  
        std::shared_ptr<SDL_Texture> texture(_outputTextHelper.CreateTextTexture(line, _renderer));
        SDL_Rect line_rect = _outputTextHelper.GetTrueTextRectangleDim(line);
        line_rect.x = 0;
        line_rect.y = y_index;
        SDL_RenderCopy(_renderer.get(), texture.get(), NULL, &line_rect);
        y_index += FONT_SIZE + 4;
    }
    SDL_RenderPresent(_renderer.get());
}

void MemoryWindow::MoveYIndex(int amount)
{
    _yIndex += amount * SCROLL_SENSITIVITY;
    if (_yIndex > 0)
    {
        _yIndex = 0;
    }
}