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

    void window_t::title_set(const std::string& title) {
        SDL_SetWindowTitle(window, title.c_str());
    }

    void window_t::size_set(int width, int height) {
        SDL_SetWindowSize(window, width, height);
    } 

    void window_t::size_set(const glm::ivec2& size) {
        size_set(size.x, size.y);
    }

    glm::ivec2 window_t::size_get() {
        int width, height;
        SDL_GetWindowSize(window, &width, &height);
        return { width, height};
    }

    uint32_t window_t::id() {
        return SDL_GetWindowID(window);
    }

    bool window_t::key_state_get(SDL_Scancode scancode) {
        spk_assert(scancode < SDL_NUM_SCANCODES, "invalid key scancode");

        if(SDL_GetKeyboardFocus() == this->window) {
            return SDL_GetKeyboardState(nullptr)[scancode];
        }

        return false;
    }

    window_t& window() {
        return *internal->scene.window;
    }

    result_e window_make_current(window_t& window) {
        if(internal->scene.window != nullptr) {
            SDL_GL_MakeCurrent(nullptr, nullptr);
            internal->scene.window = nullptr;
        }

        if(SDL_GL_MakeCurrent(window.window, window.opengl_context) < 0) {
            log.log(spk::LOG_TYPE_ERROR, "failed to make the window context current: %s", SDL_GetError());
            return ERROR_EXTERNAL_LIBRARY_FAILED;
        }

        internal->scene.window = &window;
        
        if(!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) { 
            log.log(spk::LOG_TYPE_ERROR, "failed to load OpenGL with glad");
            return ERROR_EXTERNAL_LIBRARY_FAILED;
        }

        return SUCCESS;
    }

    result_e window_force_resize_event() {
        SDL_Event event;
        int code;

        event.type             = SDL_WINDOWEVENT;
        event.window.event     = SDL_WINDOWEVENT_SIZE_CHANGED;
        event.window.data1     = 0;
        event.window.data2     = 0;
        event.window.timestamp = SDL_GetTicks();
        event.window.windowID  = window().id();

        code = SDL_PushEvent(&event);

        if(code < 0) {
            return ERROR_EXTERNAL_LIBRARY_FAILED;
        }

        return SUCCESS;
    }
}