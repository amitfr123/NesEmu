#include <vector>

#include "WindowUtilities/SdlColorHelper.hpp"
#include "NumToHexStringConvertor.hpp"


#include "EmuWindows/MemoryWindow.hpp"


MemoryWindow::MemoryWindow(GetRamFunction get_ram_function) :
    BaseWindow("Memory_View_Window", WINDOW_RECT, 0, COLOR_RED),
    _output_text_helper(std::string(FONT_PATH) + "PressStart2P.ttf", FONT_SIZE, COLOR_WHITE),
    _scroll_helper(_event_mapper, std::bind(&MemoryWindow::MoveYIndex, this, std::placeholders::_1)),
    _get_ram_function(std::move(get_ram_function))
{
    _y_index = 0;
}

std::vector<std::string> MemoryWindow::RamViewBuilder()
{
    uint32_t line_addr = 0;
    auto ram_arr = _get_ram_function();
    std::vector<std::string> string_vector;
    std::string view;
    for(const uint8_t &mem : ram_arr)
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

void MemoryWindow::RenderWindow()
{
    SDL_RenderClear(_renderer.get());
    auto vec = RamViewBuilder();
    SDL_Rect line_rect;
    uint32_t y_index = _y_index;
    for (const auto& line : vec)
    {  
        std::shared_ptr<SDL_Texture> texture(_output_text_helper.CreateTextTexture(line, _renderer));
        SDL_Rect line_rect = _output_text_helper.GetTrueTextRectangleDim(line);
        line_rect.x = 0;
        line_rect.y = y_index;
        SDL_RenderCopy(_renderer.get(), texture.get(), NULL, &line_rect);
        y_index += FONT_SIZE + 4;
    }
    SDL_RenderPresent(_renderer.get());
}

void MemoryWindow::MoveYIndex(int amount)
{
    _y_index += amount * SCROLL_SENSITIVITY;
    if (_y_index > 0)
    {
        _y_index = 0;
    }
}