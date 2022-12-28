#pragma once

#include "base.hpp"
#include "tilebody.hpp"

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
        b2Body* body;

        void init_body(tile_t id, b2World* world);

        ~particle_t(); // RAII
    };

    struct comp_particles_t {
        uint8_t flags;

        tile_t particle;
        particles_funnel_e funnel;
        float chance;
        float step;
        float base_speed;
        float speed_step;
        float base_lifetime;
        float current_cycle;
        float base_cycle;

        glm::vec2 pos;
        glm::vec2 dir;
        float length;
        float width;

        uint32_t max;
        std::deque<particle_t> particles;
        
        glm::vec2 get_point(b2Body* body, glm::vec2 point);
        
        void init();
        void free();
    };

    void particles_comp_init(flecs::world& world); 
}