#include "window.hpp"
#include "state.hpp"

namespace spk {
    void window_system_update(flecs::iter& iter, comp_window_t* c_window) {
        auto ctx = SPK_GET_CTX_REF(iter, window_system_ctx_t);
        uint64_t cur_win_i = 0;

        for(auto i : iter) {
            if(iter.entity(i).has<comp_window_t>()) {
                cur_win_i == i;
                break;
            }
        }

        while(SDL_PollEvent(&ctx->event)) {
            switch(ctx->event.type) {
                case SDL_QUIT:
                    state.exit(0);
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
                    iter.world().event<event_mouse_wheel_t>()
                        .id<tag_events_t>()
                        .entity(state.get_current_event_system())
                        .ctx(ctx->event.wheel) 
                        .emit();
                    break;

                case SDL_KEYDOWN: 
                    state.keys[ctx->event.key.keysym.scancode] = true;
                    iter.world().event<event_keyboard_t>()
                        .id<tag_events_t>()
                        .entity(state.get_current_event_system())
                        .ctx(ctx->event.key)
                        .emit();
                    break;

                case SDL_KEYUP:
                    state.keys[ctx->event.key.keysym.scancode] = false;
                    iter.world().event<event_keyboard_t>()
                        .id<tag_events_t>()
                        .entity(state.get_current_event_system())
                        .ctx(ctx->event.key)
                        .emit();
                    break;

                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    event_window_size_t resize;
                    glm::ivec2 size = c_window[cur_win_i].get_size();

                    resize.width = size.x;
                    resize.height = size.y;

                    iter.world().event<event_window_size_t>()
                        .id<tag_events_t>()
                        .entity(state.get_current_event_system())
                        .ctx(resize)                            
                        .emit();
                    
                    break;
                }

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
        
        world.system<comp_window_t>().kind(on_render_begin).ctx(ctx).iter(window_system_update)
            .add<render_system_t>();

        world.system<comp_window_t, tag_current_window_t>().ctx(ctx).interval(state.get_target_fps()).each(window_system_current_update);
    }
}