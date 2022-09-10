#include <stdexcept>

#include "EmuWindows/BaseWindow.hpp"

BaseWindow::BaseWindow(const std::string& window_title, SDL_Rect window_pos, uint32_t window_flags, SDL_Color window_background)
    :
    _window_event_helper(_event_mapper, std::bind(&BaseWindow::OnWindowClose, this))
{
    _window.reset(SDL_CreateWindow(
        window_title.c_str(),
        window_pos.x,
        window_pos.y,
        window_pos.w,
        window_pos.h,
        window_flags), &SDL_DestroyWindow);
    if (_window.get() == nullptr)
    {
        throw std::runtime_error("failed to create a window");
    }
    _renderer.reset(SDL_CreateRenderer(_window.get(), -1, SDL_RENDERER_ACCELERATED), &SDL_DestroyRenderer);
    if (_renderer.get() == nullptr)
    {
        throw std::runtime_error("failed to create a renderer for the window");
    }
    SDL_SetRenderDrawColor(_renderer.get(), window_background.r, window_background.g, window_background.b, window_background.a);
    _window_closed = false;
}

void BaseWindow::HideWindow()
{
    if (_window.get() == nullptr)
    {
        return;
    }
    SDL_HideWindow(_window.get());
}

void BaseWindow::ShowWindow()
{
    if (_window.get() == nullptr)
    {
        return;
    }
    SDL_ShowWindow(_window.get());
}

bool BaseWindow::IsWinodwHidden()
{
    uint32_t flags = SDL_GetWindowFlags(_window.get());
    return (flags & SDL_WINDOW_MINIMIZED || flags & SDL_WINDOW_HIDDEN);
}

void BaseWindow::EventHandler(const SDL_Event& e)
{
    auto handler = _event_mapper.find(static_cast<SDL_EventType>(e.type));
    if (handler == _event_mapper.end())
    {
        return;
    }
    handler->second(e);
}

uint32_t BaseWindow::GetWindowId()
{
    if (_window.get() == nullptr)
    {
        return 0;
    }
    return SDL_GetWindowID(_window.get());
}

bool BaseWindow::IsWindowClosed()
{
    return _window_closed;
}

void BaseWindow::OnWindowClose()
{
    _window_closed = true;
    _renderer.reset();
    _window.reset();
}