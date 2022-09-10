#include "WindowUtilities/OutputTextHelper.hpp"

OutputTextHelper::OutputTextHelper(const std::string& font_path, const uint32_t font_size, SDL_Color text_color) 
    :
    _color(std::move(text_color))
{
    _font.reset(TTF_OpenFont(font_path.c_str(), font_size), &TTF_CloseFont);
    if (_font.get() == nullptr)
    {
        throw std::runtime_error("Invalid font path");
    }
}

std::shared_ptr<SDL_Texture> OutputTextHelper::CreateTextTexture(const std::string& text, std::shared_ptr<SDL_Renderer> renderer)
{
    if (_font.get() == nullptr)
    {
        return nullptr;
    }
    std::shared_ptr<SDL_Surface> surface(TTF_RenderText_Solid(_font.get(), text.c_str(), _color), SDL_FreeSurface);
    if (surface.get() == nullptr)
    {
        return nullptr;
    }
    std::shared_ptr<SDL_Texture> texture(SDL_CreateTextureFromSurface(renderer.get(), surface.get()), SDL_DestroyTexture);
    if (texture.get() == nullptr)
    {
        return nullptr;
    }
    return texture;
}

SDL_Rect OutputTextHelper::GetTrueTextRectangleDim(const std::string& text)
{
    SDL_Rect dim_rect;
    TTF_SizeText(_font.get(), text.c_str(), &dim_rect.w, &dim_rect.h);
    return dim_rect;
}