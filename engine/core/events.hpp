/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once 

#include "base.hpp"

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