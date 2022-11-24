#include "window.hpp"
#include "../state.hpp"

namespace spk {
    void comp_window_on_add(comp_window_t& window) {
        window.win = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, 
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE); 
        if(window.win == nullptr) {
            sfk::log.log(sfk::LOG_TYPE_ERROR, "could not create SDL2 window; %s", SDL_GetError());
        }

        window.gl_ctx = SDL_GL_CreateContext(window.win);
        if(window.gl_ctx == nullptr) {
            sfk::log.log(sfk::LOG_TYPE_ERROR, "could not create SDL2 window's OpenGL Context; %s", SDL_GetError());
        }
    }

    void comp_window_on_remove(comp_window_t& window) {
        SDL_DestroyWindow(window.win);
    }

    void tag_current_gl_ctx_on_add(flecs::entity e, tag_current_window_t c_ctx) {
        // check state if a window is already currently active if so, we cannot have multiple windows with current_gl_ctx

        if(state._get_current_window().id() != UINT64_MAX) {
            SDL_GL_MakeCurrent(nullptr, nullptr);
            state._get_current_window().remove<tag_current_window_t>();
        }

        sfk_assert(e.has<comp_window_t>());        

        const comp_window_t* window = e.get<comp_window_t>();

        {
            int width, height;

            SDL_GL_MakeCurrent(window->win, window->gl_ctx);
            state._set_current_window(e);

            if(!gladLoadGL()) {
                sfk::log.log(sfk::LOG_TYPE_ERROR, "failed to load OpenGL with glad");
            }

            SDL_GetWindowSize(window->win, &width, &height);
            // glViewport(0, 0, width, height); this is set in render resize
        }
    }

    void comp_window_component_init(flecs::world& world) {
        world.component<comp_window_t>();
        world.observer<comp_window_t>("OnAdd Window observer").event(flecs::OnAdd).each(comp_window_on_add);
        world.observer<comp_window_t>("OnRemove Window observer").event(flecs::OnRemove).each(comp_window_on_remove);
        world.observer<tag_current_window_t>("OnAdd Window OpenGL Current Ctx").event(flecs::OnAdd).each(tag_current_gl_ctx_on_add);
    }
}