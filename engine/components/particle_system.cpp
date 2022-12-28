#include "particle_system.hpp"
#include "state.hpp"
#include "spock.hpp"

namespace spk {
        void particle_t::init_body(tile_t tile, b2World* world) {
            tile_metadata_t tmd = state.engine->rsrc_mng.get_tile_dictionary()[tile.id];

            sfk_assert(!true, "feature is not developed, particle systems in box2d are bugged");

            b2BodyDef def;
            def.linearVelocity = to_box_vec2(dir * speed);
            def.type = b2_dynamicBody;
            body = world->CreateBody(&def);

            b2PolygonShape shape;
            shape.SetAsBox(tmd.sprite.size.x, tmd.sprite.size.y);

            b2FixtureDef fix_def;
            fix_def.density = tmd.density;
            fix_def.restitution = tmd.restitution;
            fix_def.friction = tmd.friction;
            fix_def.shape = &shape;
            body->CreateFixture(&fix_def);
        }

    particle_t::~particle_t() {
        if(body) {
            body->GetWorld()->DestroyBody(body);
        }
    }
 
    void comp_particles_t::init() {
        flags = PARTICLES_FLAG_ACTIVE;
        
        funnel = PARTICLE_SYSTEM_FUNNEL_LINE;
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

    void comp_particles_t::free() {
    }


    glm::vec2 comp_particles_t::get_point(b2Body* body, glm::vec2 point) {
        if(flags & PARTICLES_FLAG_WORLD_POSITION) {
            return spk::to_glm_vec2(body->GetPosition()) + pos + point;
        } else {
            return spk::to_glm_vec2(body->GetWorldPoint(spk::to_box_vec2(pos + point)));
        }
    }

    void particles_comp_init(flecs::world& world) {
        spk_register_component(world, comp_particles_t);
    }
}