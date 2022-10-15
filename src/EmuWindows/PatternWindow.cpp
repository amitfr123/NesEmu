#include "EmuWindows/PatternWindow.hpp"
#include "WindowUtilities/SdlColorHelper.hpp"

PatternWindow::PatternWindow(GetPatternFunction getPattern) : 
    _pixelTextureHelper(PATTERN_ROW_SIZE, PATTERN_COL_SIZE, PIXEL_SIZE),
    BaseWindow("PatternWindow", {0,0, static_cast<int>(PATTERN_ROW_SIZE * PIXEL_SIZE * 2 + 5), PATTERN_COL_SIZE * PIXEL_SIZE}, 0, COLOR_WHITE),
    _getPattern(std::move(getPattern))
{
    _eventMapper.insert(std::make_pair(SDL_EventType::SDL_KEYDOWN, std::bind(&PatternWindow::handleKeyDown, this, std::placeholders::_1)));
}

void PatternWindow::renderWindow()
{
    auto pt = _getPattern(_paletteId);
    SDL_RenderClear(_renderer.get());
    _pixelTextureHelper.fillTexture(std::span<const uint32_t>(pt[0].begin(), pt[0].size()));
    SDL_Rect rect = _pixelTextureHelper.getTextureRect();
    SDL_RenderCopy(_renderer.get(), _pixelTextureHelper.getTexture(_renderer).get(), NULL, &rect);
    _pixelTextureHelper.fillTexture(std::span<const uint32_t>(pt[1].begin(), pt[1].size()));
    rect = _pixelTextureHelper.getTextureRect();
    rect.x = PATTERN_ROW_SIZE * PIXEL_SIZE + 5;
    SDL_RenderCopy(_renderer.get(), _pixelTextureHelper.getTexture(_renderer).get(), NULL, &rect);
    SDL_RenderPresent(_renderer.get());
}

void PatternWindow::handleKeyDown(const SDL_Event& e)
{
    switch (e.type)
    {
    case SDL_EventType::SDL_KEYDOWN:
        if (e.key.keysym.sym == SDL_KeyCode::SDLK_RIGHT)
        {
            _paletteId = (_paletteId + 1) % 0x7;
        }
        if (e.key.keysym.sym == SDL_KeyCode::SDLK_LEFT)
        {
            _paletteId = (_paletteId - 1) % 0x7;
        }
    break;
    default:
        break;
    }
}