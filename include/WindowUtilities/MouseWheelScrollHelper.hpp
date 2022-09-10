#pragma once 

#include <functional>

#include <SDL2/SDL.h>

class MouseWheelScrollHelper {
public:
    using ScrollFunction = std::function<void (int)>;

    using EventMapper = std::unordered_map<SDL_EventType, std::function<void (const SDL_Event& e)>>;

    MouseWheelScrollHelper(EventMapper& event_mapper, ScrollFunction on_scroll);

    void ScrollHandler(const SDL_Event& e);

    ~MouseWheelScrollHelper() = default;
private:
    ScrollFunction _on_scroll;
};