#include "WindowUtilities/WindowEventHelper.hpp"

WindowEventHelper::WindowEventHelper(EventMapper& event_mapper, OnCloseFunction on_close)
    :
    _on_close(std::move(on_close))
{
    event_mapper.insert(std::make_pair(SDL_EventType::SDL_WINDOWEVENT, std::bind(&WindowEventHelper::WindowHandler, this, std::placeholders::_1)));
}

void WindowEventHelper::WindowHandler(const SDL_Event& e)
{
    switch (e.window.event)
    {
    case SDL_WINDOWEVENT_CLOSE:
        _on_close();
        break;
    
    default:
        break;
    }
}