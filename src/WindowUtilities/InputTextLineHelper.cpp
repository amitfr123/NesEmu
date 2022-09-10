#include "WindowUtilities/InputTextLineHelper.hpp"

InputTextLineHelper::InputTextLineHelper(EventMapper& event_mapper, LineCompletionFunction line_func) :
        _line_func(std::move(line_func))
{
    event_mapper.insert(std::make_pair(SDL_EventType::SDL_TEXTINPUT, std::bind(&InputTextLineHelper::TextHandler, this, std::placeholders::_1)));
    event_mapper.insert(std::make_pair(SDL_EventType::SDL_KEYDOWN, std::bind(&InputTextLineHelper::TextHandler, this, std::placeholders::_1)));
    _line_state = LineState::READY;;
}

void InputTextLineHelper::SetAfterEnterMessage(std::string && str)
{
    _line.clear();
    _line.append(str);
    _line_state = LineState::POST_ENTER;
}

void InputTextLineHelper::TextHandler(const SDL_Event& e)
{
    if (_line_state == LineState::POST_ENTER)
    {
        _line.clear();
        _line_state = LineState::READY;
    }
    switch (e.type)
    {
    case SDL_EventType::SDL_KEYDOWN:
        if (e.key.keysym.sym == SDL_KeyCode::SDLK_BACKSPACE && _line.length() > 0)
        {
            _line.pop_back();
        }
        if (e.key.keysym.sym == SDL_KeyCode::SDLK_RETURN && _line.length() > 0)
        {
            _line_func(_line);
        }
    break;
    case SDL_EventType::SDL_TEXTINPUT:
        _line.append(e.text.text);
    break;
    default:
        break;
    }
}

std::string InputTextLineHelper::GetLine() const
{
    return _line;
}