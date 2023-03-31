/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "window.hpp"
#include "core/internal.hpp"

namespace spk {
    void handle_window_event(flecs::iter& iter, SDL_WindowEvent& event) {
        switch(event.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED: {
                if(event.windowID != window().id()) {
                    return;
                } 

                event_window_size_t resize;
                glm::ivec2 size = window().size();

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
                    handle_window_event(iter, event.window);
                    break;

                default:
                    break;
            } // switch
        }
    }

    void window_cs_init(flecs::world& world) {
        spk_trace();
        
        world.system().kind(on_render_begin_id).iter(window_system_update).add<tag_renderer_t>();
    }
}