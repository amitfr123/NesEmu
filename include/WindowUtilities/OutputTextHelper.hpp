#pragma once 

#include <string>
#include <memory>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class OutputTextHelper {
public:
    OutputTextHelper(const std::string& font_path, const uint32_t font_size, SDL_Color text_color);

    std::shared_ptr<SDL_Texture> CreateTextTexture(const std::string& text, std::shared_ptr<SDL_Renderer> renderer);

    SDL_Rect GetTrueTextRectangleDim(const std::string& text);

    ~OutputTextHelper() = default;
private:
    std::shared_ptr<TTF_Font> _font;
    SDL_Color _color;
};