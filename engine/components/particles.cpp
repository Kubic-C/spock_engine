#include "particles.hpp"
#include "state.hpp"
#include "spock.hpp"

namespace spk {
    void comp_particles_t::init(flecs::entity entity) {
        flags = PARTICLES_FLAGS_ACTIVE;
        
        funnel = PARTICLES_FUNNEL_LINE;
        chance = 1.0f;
        step   = 0.5f;
        base_speed  = 75.0f;
        speed_step = 0.0f;
        base_lifetime = 5.0f;
        current_cycle = 0.0f;
        base_cycle = 0.3f;

        length = 1.0f;
        width  = 1.0f;
        pos = {0.0f, 0.0f};
        dir = {0.0f, 1.0f};

        max = 100;

        particle.id = 0;
    }   

    void comp_particles_t::free(flecs::entity entity) {
    }


    glm::vec2 comp_particles_t::get_point(b2Body* body, glm::vec2 point) {
        if(flags & PARTICLES_FLAGS_WORLD_POSITION) {
            return body->GetPosition() + pos + point;
        } else {
            return body->GetWorldPoint(pos + point);
        }
    }

    void particles_comp_init(flecs::world& world) {
        spk_register_component(world, comp_particles_t);
    }
}