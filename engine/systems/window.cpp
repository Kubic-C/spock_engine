#include "window.hpp"
#include "core/internal.hpp"

namespace spk {
    void handle_window_event(flecs::iter& iter, SDL_WindowEvent& event, window_t* window) {
        switch(event.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED: {
                if(event.windowID != SDL_GetWindowID(window->sdl_window())) {
                    return;
                } 

                event_window_size_t resize;
                glm::ivec2 size = window->size_get();

                resize.width = size.x;
                resize.height = size.y;

                iter.world().event<event_window_size_t>()
                    .id<tag_events_t>()
                    .entity(internal->scene.event_system)
                    .ctx(resize)                            
                    .emit();
                
                break;
            }
        }
    }

    void window_system_update(flecs::iter& iter) {
        spk_trace();

        window_t* window = internal->scene.window;
        SDL_Event event;

        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    internal->settings.should_exit = true;
                    break;

                case SDL_MOUSEBUTTONDOWN: {
                    iter.world().event<event_window_mouse_click_t>()
                        .id<tag_events_t>()
                        .entity(internal->scene.event_system)
                        .ctx(event.button)                            
                        .emit();
                    break;
                }

                case SDL_MOUSEWHEEL:
                    iter.world().event<event_mouse_wheel_t>()
                        .id<tag_events_t>()
                        .entity(internal->scene.event_system)
                        .ctx(event.wheel) 
                        .emit();
                    break;

                case SDL_KEYDOWN: 
                    iter.world().event<event_keyboard_t>()
                        .id<tag_events_t>()
                        .entity(internal->scene.event_system)
                        .ctx(event.key)
                        .emit();
                    break;

                case SDL_KEYUP:
                    iter.world().event<event_keyboard_t>()
                        .id<tag_events_t>()
                        .entity(internal->scene.event_system)
                        .ctx(event.key)
                        .emit();
                    break;

                case SDL_WINDOWEVENT:
                    handle_window_event(iter, event.window, window);
                    break;

                default:
                    break;
            } // switch
        }
    }

    void window_cs_init(flecs::world& world) {
        spk_trace();
        
        world.system().kind(on_render_begin).iter(window_system_update).add<tag_render_system_t>();
    }
}