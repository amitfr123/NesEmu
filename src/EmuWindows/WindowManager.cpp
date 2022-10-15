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
        _windowMapper.insert(std::make_pair(window->getWindowId(), window));
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
            EventMapperHelper(e.key.windowID, e);
            break;
        case SDL_TEXTINPUT:
            EventMapperHelper(e.text.windowID, e);
            break;
        case SDL_MOUSEWHEEL:
            EventMapperHelper(e.wheel.windowID, e);
            break;
        case SDL_WINDOWEVENT:
            EventMapperHelper(e.window.windowID, e);
            if (e.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                _windowMapper.clear();
                flag = false;
            }
            break;
        default:
            break;
        }
        if (e.type != SDL_QUIT) // SDL_QUIT should kill the program or something like that
        {
            for(auto iter = _windowMapper.begin(); iter != _windowMapper.end(); ++iter)
            {
                if (!iter->second->isWindowClosed())
                {
                    iter->second->renderWindow();
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

void WindowManager::EventMapperHelper(uint32_t win_id, const SDL_Event& e)
{
    auto iter = _windowMapper.find(win_id);
    if (iter == _windowMapper.end())
    {
        return;
    }
    iter->second->eventHandler(e);
}