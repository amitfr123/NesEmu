#pragma once 

#include <unordered_map>
#include <string>
#include <functional>

#include <SDL2/SDL.h>

class InputTextLineHelper {
public:
    enum class LineState
    {
        READY,
        POST_ENTER
    };

    using EventMapper = std::unordered_map<SDL_EventType, std::function<void (const SDL_Event& e)>>;
    using LineCompletionFunction = std::function<void (const std::string& str)>;

    InputTextLineHelper(EventMapper& event_mapper, LineCompletionFunction line_func);

    void SetAfterEnterMessage(std::string && str);
    void TextHandler(const SDL_Event& e);
    std::string GetLine() const; 
    ~InputTextLineHelper() = default;
private:
    LineState _line_state;
    std::string _line;
    LineCompletionFunction _line_func; 
};