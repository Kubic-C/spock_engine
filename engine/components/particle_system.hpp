#pragma once

#include "../base.hpp"
#include "sprite.hpp"

namespace spk {
    enum particles_funnel_e: uint16_t {
        PARTICLE_SYSTEM_FUNNEL_LINE,
        PARTICLE_SYSTEM_FUNNEL_FUNNEL,
        PARTICLE_SYSTEM_FUNNEL_LAST
    };

    enum particles_flags_e {
        PARTICLE_FLAG_ACTIVE = 1 << 0
       // PARTICLE_FLAG_USE_SPRITE = 1 < 1
    };

    struct particle_t {
        glm::vec2 pos;
        glm::vec2 dir;
        float lifetime;
    };

    struct comp_particles_t {
        uint8_t flags;

        comp_sprite_t sprite;
        particles_funnel_e funnel;
        float chance;
        float step;
        float speed;
        float base_lifetime;
        float current_cycle;
        float base_cycle;
        bool world_positioning;

        glm::vec2 pos;
        glm::vec2 dir;
        float length;
        float width;

        uint32_t max;
        std::deque<particle_t> particles;
        
        void init();
        void free();
    };

    void particles_comp_init(flecs::world& world); 
}