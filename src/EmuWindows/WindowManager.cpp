#include <iostream>

#include <SDL2/SDL_ttf.h>

#include "EmuWindows/WindowManager.hpp"

WindowManager::WindowManager()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
    TTF_Init();
    // TODO: add event listening like TextInput
}

void WindowManager::AddNewWindow(std::shared_ptr<BaseWindow> window)
{
    if (window.get() != nullptr)
    {
        _window_mapper.insert(std::make_pair(window->GetWindowId(), window));
    }
}

void WindowManager::EmuWindowManagerEventLoop()
{
    SDL_Event e;
    bool flag = true;

    // TODO: add support for more event types
    // TODO: find a good way to allow you to close the program
    SDL_StartTextInput();
    while(flag)
    {
        SDL_PollEvent(&e);
        switch (e.type)
        {
        case SDL_KEYDOWN:
            EventMapperHelpr(e.key.windowID, e);
            break;
        case SDL_TEXTINPUT:
            EventMapperHelpr(e.text.windowID, e);
            break;
        case SDL_MOUSEWHEEL:
            EventMapperHelpr(e.wheel.windowID, e);
            break;
        case SDL_WINDOWEVENT:
            EventMapperHelpr(e.window.windowID, e);
            break;
        default:
            break;
        }
        if (e.type != SDL_QUIT) // SDL_QUIT should kill the program or something like that
        {
            for(auto iter = _window_mapper.begin(); iter != _window_mapper.end(); ++iter)
            {
                if (!iter->second->IsWindowClosed())
                {
                    iter->second->RenderWindow();
                }
            }
        }
        else
        {
            flag = false;
        }
    }
    SDL_StopTextInput();
}

void WindowManager::EventMapperHelpr(uint32_t win_id, const SDL_Event& e)
{
    auto iter = _window_mapper.find(win_id);
    if (iter == _window_mapper.end())
    {
        return;
    }
    iter->second->EventHandler(e);
}