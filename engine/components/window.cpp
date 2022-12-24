#include "window.hpp"
#include "../state.hpp"

namespace spk {
    void comp_window_t::init() {
        win = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, 
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE); 
        if(win == nullptr) {
            sfk::log.log(sfk::LOG_TYPE_ERROR, "could not create SDL2 window; %s", SDL_GetError());
        }

        gl_ctx = SDL_GL_CreateContext(win);
        if(gl_ctx == nullptr) {
            sfk::log.log(sfk::LOG_TYPE_ERROR, "could not create SDL2 window's OpenGL Context; %s", SDL_GetError());
        }
    }

    void comp_window_t::free() {
        SDL_DestroyWindow(win);
    }

    void tag_current_gl_ctx_on_add(flecs::entity e, comp_window_t& window, tag_current_window_t c_ctx) {
        // check state if a window is already currently active if so, we cannot have multiple windows with current_gl_ctx

        if(state.get_current_window().id() != UINT64_MAX) {
            SDL_GL_MakeCurrent(nullptr, nullptr);
            state.get_current_window().remove<tag_current_window_t>();
        }

        if(SDL_GL_MakeCurrent(window.win, window.gl_ctx) < 0) {
            sfk::log.log(sfk::LOG_TYPE_ERROR, "failed to make the window context current: %s", SDL_GetError());
        }

        state.set_current_window(e);
        
        if(!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) { 
            sfk::log.log(sfk::LOG_TYPE_ERROR, "failed to load OpenGL with glad");
        }
    }

    void window_component_init(flecs::world& world) {
        spk_register_component(world, comp_window_t);

        world.observer<comp_window_t, tag_current_window_t>("OnAdd Window OpenGL Current Ctx")
            .event(flecs::OnAdd).each(tag_current_gl_ctx_on_add);
    }
}