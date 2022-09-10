#pragma once 

#include <functional>

#include <SDL2/SDL.h>

class WindowEventHelper {
public:
    using OnCloseFunction = std::function<void ()>;

    using EventMapper = std::unordered_map<SDL_EventType, std::function<void (const SDL_Event& e)>>;

    WindowEventHelper(EventMapper& event_mapper, OnCloseFunction on_close);

    void WindowHandler(const SDL_Event& e);

    ~WindowEventHelper() = default;
private:
    OnCloseFunction _on_close;
};