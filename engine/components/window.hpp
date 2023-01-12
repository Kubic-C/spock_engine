#pragma once

#include "events.hpp"

namespace spk {
    struct tag_current_window_t {};

    struct tag_mouse_t {};

    struct comp_window_t {
        SDL_Window* win;
        SDL_GLContext gl_ctx;

        void init(flecs::entity entity);
        void free(flecs::entity entity);

        void set_title(const std::string& title) {
            SDL_SetWindowTitle(win, title.c_str());
        }

        void force_resize_event() {
            SDL_Event event;

            event.type = SDL_WINDOWEVENT_SIZE_CHANGED;
            event.window.data1 = 0;
            event.window.data2 = 0;
            event.window.timestamp = 0;
            event.window.windowID = SDL_GetWindowID(win);

            SDL_PushEvent(&event);
        }

        void set_size(int width, int height) {
            SDL_SetWindowSize(win, width, height);
        } 

        void set_size(const glm::ivec2& size) {
            set_size(size.x, size.y);
        }

        glm::ivec2 get_size() {
            int width, height;
            SDL_GetWindowSize(win, &width, &height);
            return { width, height};
        }
    };

    void window_component_init(flecs::world& world);
}