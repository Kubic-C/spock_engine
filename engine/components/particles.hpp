/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#pragma once

#include "components.hpp"
#include "core/tiles.hpp"

namespace spk {
    enum particles_funnel_e: uint16_t {
        PARTICLES_FUNNEL_LINE,
        PARTICLES_FUNNEL_FUNNEL,
        PARTICLES_FUNNEL_LAST
    };

    enum particles_flags_e {
        PARTICLES_FLAGS_ACTIVE = 1 << 0,
        PARTICLES_FLAGS_WORLD_POSITION = 1 << 1,
        PARTICLES_FLAGS_WORLD_DIRECTION = 1 << 2,
        PARTICLES_FLAGS_COLLIADABLE = 1 << 3
    };

    struct particle_t {
        glm::vec2 pos;
        glm::vec2 dir;
        float speed;
        float lifetime;
    };

    struct comp_particles_t : component_t {
        uint8_t flags;

        tile_sprite_t sprite; // used to get sprite information and physics info. Correlates to tile render ID
        particles_funnel_e funnel; // funnel of the particle systems, affects the direction and initial speed of particles
        float chance; // chance of a particle spawning, range is 0.0 to 1.0. Set to 1.0 to always spawn
        float base_speed; // the initial speed of a particle
        float speed_step; // how much to increase a particle's speed every tick, can be negative
        float base_lifetime; // the lifetime of a particle, higher lifetimes the longer they stay one screen
        float current_cycle; // do not touch
        float base_cycle; // how often to try and create a particle (in seconds)

        // the increment between spawning of particles on the spawn line, starts at 0 and stops once past length
        // shorter steps essentially mean more particles per cycle
        float step; 
        glm::vec2 pos; // position of the spawn line (relative to body)
        glm::vec2 dir; // direction of the spawn line
        float length; // length of the spawn line
        float width; // width of the spawn line

        uint32_t max; // the maximum amount of particles allowed
        std::deque<particle_t> particles;
        
        glm::vec2 get_point(kin::transform_t* transform, glm::vec2 point);
        
        void init(flecs::entity entity);
        void free(flecs::entity entity);
    };

    void particles_comp_init(const flecs::world& world); 
}