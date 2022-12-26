#include "particle_system.hpp"

namespace spk {
    void comp_particles_t::init() {
        flags = 0;
        
        funnel = PARTICLE_SYSTEM_FUNNEL_LINE;
        chance = 1.0f;
        step   = 0.5f;
        speed  = 1.0f;
        base_lifetime = 5.0f;
        current_cycle = 0.0f;
        base_cycle = 1.0f;
        world_positioning = false;
        world_direction = true;

        length = 1.0f;
        width  = 1.0f;
        pos = {0.0f, 0.0f};
        dir = {0.0f, 1.0f};

        max = 100;

        sprite.init();
    }   

    void comp_particles_t::free() {
        sprite.free();
    }


    glm::vec2 comp_particles_t::get_point(b2Body* body, glm::vec2 point) {
        if(world_direction) {
            return spk::to_glm_vec2(body->GetPosition()) + pos + point;
        } else {
            return spk::to_glm_vec2(body->GetWorldPoint(spk::to_box_vec2(pos + point)));
        }
    }

    void particles_comp_init(flecs::world& world) {
        spk_register_component(world, comp_particles_t);
    }
}