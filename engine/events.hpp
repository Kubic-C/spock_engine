#pragma once 

#include "framework.hpp"

namespace spk {
    struct tag_events_t {};

    // event types

    struct comp_window_size_t {
        int width;
        int height;
    };

    typedef SDL_MouseButtonEvent comp_window_mouse_click_t; 
}