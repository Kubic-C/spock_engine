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
        glm::ivec2 size_get();
  
        uint32_t       id(); // the SDL Window ID
        bool           key_state_get(SDL_Scancode scancode);

        uint8_t*      keyboard;
        SDL_Window*   window;
        SDL_GLContext opengl_context;
    };

    // gets the current window
    window_t& window();

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