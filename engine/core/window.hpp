/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "base.hpp"
#include "utility/stack_allocator.hpp"

namespace spk {
    struct window_t {
    public:

        window_t();
        window_t(const char* name);
        ~window_t();

        void size_set(int width, int height);
        void size_set(const glm::ivec2& size);
        void title_set(const std::string& title);
        glm::ivec2 size();
  
        uint32_t  id(); // the SDL Window ID
        bool      key_get(SDL_Scancode scancode);
        uint32_t  mouse_get_cur(glm::vec2& pos, bool y_flip = false);

        // struct 

        glm::vec2  mouse_position;

        glm::vec2  mouse_click_pos;
        uint32_t   mouse_click_btn;
        bool       mouse_click_down;

        uint8_t*      keyboard;
        SDL_Window*   window;
        SDL_GLContext opengl_context;
    };

    /**
     * @brief makes the window the current window, meaning all rendering operations
     * will be operated on it, and it will be actively updated.
     * 
     * @return SUCCESS or on failure: ERROR_EXTERNAL_LIBRARY_FAILED
     */
    result_e window_make_current(window_t& window);

    /**
     * @brief forces a resize event on the current window, this does not change
     * the window size rather emits an event of the window size 
     * 
     * @return SUCCESS or on failure: ERROR_EXTERNAL_LIBRARY_FAILED
     */
    result_e window_force_resize_event();
}