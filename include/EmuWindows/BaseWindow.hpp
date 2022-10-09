#pragma once

#include <SDL2/SDL.h>

#include <unordered_map>
#include <memory>
#include <string>
#include <functional>

#include "WindowUtilities/WindowEventHelper.hpp"

class BaseWindow {
public:
    bool initWindow();
    void hideWindow();
    void showWindow();
    bool isWindowHidden();
    void eventHandler(const SDL_Event& e);
    uint32_t getWindowId();
    bool isWindowClosed();
    virtual void renderWindow(){}
    virtual ~BaseWindow() = default;

protected:
    using EventHandlerFunction = std::function<void (const SDL_Event& e)>;

    BaseWindow(const std::string& window_title, SDL_Rect window_pos, uint32_t window_flags, SDL_Color window_background);
    
    std::unordered_map<SDL_EventType, EventHandlerFunction> _eventMapper;
    std::shared_ptr<SDL_Window> _window;
    std::shared_ptr<SDL_Renderer> _renderer;
    
    bool _window_closed;
private:
    void OnWindowClose();
    WindowEventHelper _windowEventHelper;
};