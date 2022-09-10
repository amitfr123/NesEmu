#pragma once

#include "BaseWindow.hpp"

class WindowManager {
private:
    void EventMapperHelpr(uint32_t win_id, const SDL_Event& e);

    std::unordered_map<uint32_t, std::shared_ptr<BaseWindow>> _window_mapper;
public:
    WindowManager();

    void AddNewWindow(std::shared_ptr<BaseWindow> window);

    void EmuWindowManagerEventLoop();
};