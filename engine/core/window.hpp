#pragma once

#include "base.hpp"
#include "utility/stack_allocator.hpp"

namespace spk {
    struct window_t {
    public:

        window_t();
        window_t(const char* name);
        ~window_t();

        void make_current();
        void force_resize_event();
        void set_size(int width, int height);
        void set_size(const glm::ivec2& size);
        void set_title(const std::string& title);
        glm::ivec2 get_size();

        SDL_Window*    get_sdl_window();
        SDL_GLContext* get_context();
        bool           get_key_state(SDL_Scancode scancode);

    protected:
        uint8_t*      keyboard;
        SDL_Window*   window;
        SDL_GLContext opengl_context;
    };
}