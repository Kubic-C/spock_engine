#include "window.hpp"
#include "../state.hpp"

namespace spk {
    void handle_window_event(flecs::iter& iter, comp_window_t* c_window, flecs::ref<spk::window_system_ctx_t>& ctx) {
        const comp_window_t* window = state.get_current_window().get<comp_window_t>();

        // we only want to recieve window events for the current ID
        if(ctx->event.window.windowID == SDL_GetWindowID(window->win)) {
            switch(ctx->event.window.event) {
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    event_window_size_t size;

                    SDL_GetWindowSize(window->win, &size.width, &size.height);

                    iter.world().event<event_window_size_t>()
                        .id<tag_events_t>()
                        .entity(state.get_current_event_system())
                        .ctx(size)                            
                        .emit();
                    
                    break;
                }
            }
        }
    }

    void window_system_update(flecs::iter& iter, comp_window_t* c_window) {
        auto ctx = SPK_GET_CTX_REF(iter, window_system_ctx_t);

        while(SDL_PollEvent(&ctx->event)) {
            switch(ctx->event.type) {
                case SDL_QUIT:
                    state.exit(0);
                    break;

                case SDL_WINDOWEVENT:
                    handle_window_event(iter, c_window, ctx);
                    break;

                case SDL_MOUSEBUTTONDOWN: {
                    iter.world().event<event_window_mouse_click_t>()
                        .id<tag_events_t>()
                        .entity(state.get_current_event_system())
                        .ctx(ctx->event.button)                            
                        .emit();
                    break;
                }

                case SDL_MOUSEWHEEL:
                    iter.world().event<event_window_mouse_wheel_t>()
                        .id<tag_events_t>()
                        .entity(state.get_current_event_system())
                        .ctx(ctx->event.wheel) 
                        .emit();
                    break;

                default:
                    break;
            } // switch
        }
    }

    void window_system_current_update(comp_window_t& window, tag_current_window_t) {
    }

    void window_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world) {
        flecs::entity* ctx = ctx_alloc.allocate_ctx<window_system_ctx_t>();

        window_component_init(world);
        
        world.system<comp_window_t>().ctx(ctx).iter(window_system_update);
        world.system<comp_window_t, tag_current_window_t>().ctx(ctx).interval(state.get_target_fps()).each(window_system_current_update);
    }
}