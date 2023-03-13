#include "window.hpp"
#include "internal.hpp"

namespace spk {
    void create_window(SDL_Window*& window, SDL_GLContext& opengl_context, const char* name) {
        spk_trace();

        window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, 
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE); 
        if(window == nullptr) {
            log.log(spk::LOG_TYPE_ERROR, "could not create SDL2 window; %s", SDL_GetError());
        }
    
        opengl_context = SDL_GL_CreateContext(window);
        if(opengl_context == nullptr) {
            log.log(spk::LOG_TYPE_ERROR, "could not create SDL2 window's OpenGL Context; %s", SDL_GetError());
        }
    }

    window_t::window_t() {
        create_window(window, opengl_context, "");
    }

    window_t::window_t(const char* name) {
        create_window(window, opengl_context, name);
    }

    window_t::~window_t() {
        SDL_DestroyWindow(window);
    }

    void window_t::make_current() {
        if(internal->scene.window != nullptr) {
            SDL_GL_MakeCurrent(nullptr, nullptr);
            internal->scene.window = nullptr;
        }

        if(SDL_GL_MakeCurrent(window, opengl_context) < 0) {
            log.log(spk::LOG_TYPE_ERROR, "failed to make the window context current: %s", SDL_GetError());
        }

        internal->scene.window = this;
        
        if(!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) { 
            log.log(spk::LOG_TYPE_ERROR, "failed to load OpenGL with glad");
        }
    }

    void window_t::set_title(const std::string& title) {
        SDL_SetWindowTitle(window, title.c_str());
    }

    void window_t::force_resize_event() {
        SDL_Event event;

        event.type = SDL_WINDOWEVENT;
        event.window.event = SDL_WINDOWEVENT_SIZE_CHANGED;
        event.window.data1 = 0;
        event.window.data2 = 0;
        event.window.timestamp = 0;
        event.window.windowID = SDL_GetWindowID(window);

        SDL_PushEvent(&event);
    }

    void window_t::set_size(int width, int height) {
        SDL_SetWindowSize(window, width, height);
    } 

    void window_t::set_size(const glm::ivec2& size) {
        set_size(size.x, size.y);
    }

    glm::ivec2 window_t::get_size() {
        int width, height;
        SDL_GetWindowSize(window, &width, &height);
        return { width, height};
    }

    SDL_Window* window_t::get_sdl_window() {
        return window;
    }

    SDL_GLContext* window_t::get_context() {
        return &opengl_context;
    }

    bool window_t::get_key_state(SDL_Scancode scancode) {
        spk_assert(scancode < SDL_NUM_SCANCODES, "invalid key scancode");

        if(SDL_GetKeyboardFocus() == this->window) {
            return SDL_GetKeyboardState(nullptr)[scancode];
        }

        return false;
    }
}