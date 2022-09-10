#include "WindowUtilities/MouseWheelScrollHelper.hpp"

MouseWheelScrollHelper::MouseWheelScrollHelper(EventMapper& event_mapper, ScrollFunction on_scroll)
    :
    _on_scroll(std::move(on_scroll))
{
    event_mapper.insert(std::make_pair(SDL_EventType::SDL_MOUSEWHEEL, std::bind(&MouseWheelScrollHelper::ScrollHandler, this, std::placeholders::_1)));
}

void MouseWheelScrollHelper::ScrollHandler(const SDL_Event& e)
{
    if (e.wheel.y != 0)
    {
        _on_scroll(e.wheel.y);
    }
}