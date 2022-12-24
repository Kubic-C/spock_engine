#pragma once 

#include "framework.hpp"

namespace spk {
    struct tag_events_t {};

    // event types

    struct event_window_size_t {
        int width;
        int height;
    };

    typedef SDL_MouseButtonEvent event_window_mouse_click_t; 
    typedef SDL_MouseWheelEvent event_mouse_wheel_t;
    typedef SDL_KeyboardEvent event_keyboard_t;
}