/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "base.hpp"

#include "sound.hpp"
#include "events.hpp"
#include "window.hpp"
#include "ui.hpp"
#include "sprite_atlas.hpp"
#include "sprite_array.hpp"
#include "tiles.hpp"

#include "render/renderer.hpp"

#include "utility/stack_allocator.hpp"
    
// well need this to define the allocators
#include "components/tilemap.hpp"

namespace spk {

    typedef std::function<void()> user_tick_t;
    typedef std::function<void()> user_update_t;

    enum vsync_setting_e {
        VSYNC_ADAPATIVE = -1,
        VSYNC_DISABLED = 0,
        VSYNC_ENABLED = 1,
        VSYNC_UKNOWN = 0xFFFF
    };

    const char* vsync_name(vsync_setting_e opt);

    // user data for the main loop. Mainly frame and tick callbacks.
    struct user_data_t {
        user_tick_t   tick   = nullptr;
        user_update_t update = nullptr;
    };

    // the internal settings: this includes target fps, if the engine should
    // stop and exit, etc.
    struct settings_t {
        bool            log_statistics   = false;
        bool            should_exit      = false;
        double          target_fps       = 120.0;
        double          target_tps       = 60.0;
        uint32_t        box2d_draw_flags = 0;
        vsync_setting_e vsync_opt        = VSYNC_DISABLED; 
        float           ppm              = 8.0f;
    };

    // information regarding the exit and destruction of the engine
    struct exit_info_t {
        int code = 0;
    };
    
    // statistics of the engine such as frame time
    // used to measure performance 
    struct statistics_t {
        double   delta_time         = 0.0;
        double   frame_time         = 0.0;
        double   average_delta_time = 0;
        uint32_t fps                = 0;
        uint32_t tps                = 0;
    };

    // note: this will be accessed the most often
    // What the engine should be currently writing and reading to (AKA working on)
    struct scene_t {
        flecs::world      ecs_world; // has constructor, no need to set
        render_context_t* render_context = nullptr;
        kin::world_t*     physics_world  = nullptr;

        // user callbacks for tick and frame
        user_data_t user_data;

        // used for the main loop
        flecs::entity render_pipeline = flecs::entity(UINT64_MAX);
        flecs::entity game_pipeline   = flecs::entity(UINT64_MAX);
        
        // entities must be owned by ecs_world
        flecs::entity event_system    = flecs::entity(UINT64_MAX);
        flecs::entity camera          = flecs::entity(UINT64_MAX);
        canvas_t*     canvas          = nullptr;
        window_t*     window          = nullptr;
        
        stack_allocator_t stack{4096 * 4}; // scenes stack allocator, for quick large allocations
    };

    // loading and management of resources like external files or prefabs
    struct resources_t {
        FT_Library                ft_lib; 
        font_dictionary_t         fonts;
        sprite_atlas_dictionary_t sprite_atlases;
        sprite_array_dictionary_t sprite_arrays;
        sound_dictionary_t        sounds;
    };

    // allocators for canvases, windows, etc. and stack allocators
    // FOR BIG things
    struct allocators_t {
        object_pool_t<tilemap_chunk_info_t>   chunk_info_pool;
        object_pool_t<tilemap_chunks_t> chunks_pool;
    };

    // gets the current ecs_world
    const flecs::world& ecs_world();

    // gets the current physics world
    kin::world_t& physics_world();

     // gets the current window
    window_t& window();

    // gets the current canvas
    canvas_t& canvas();

    // all self explanatory

    settings_t&   settings();
    statistics_t& statistics();
    scene_t&      scene();
    resources_t&  resources();
    allocators_t& allocators();
}