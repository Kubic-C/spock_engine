#include "window.hpp"
#include "core/internal.hpp"

namespace spk {
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

                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    event_window_size_t resize;
                    glm::ivec2 size = window->get_size();

                    resize.width = size.x;
                    resize.height = size.y;

                    iter.world().event<event_window_size_t>()
                        .id<tag_events_t>()
                        .entity(internal->scene.event_system)
                        .ctx(resize)                            
                        .emit();
                    
                    break;
                }

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