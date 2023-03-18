/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "particles.hpp"
#include "utility/ui.hpp"
#include "core/internal.hpp"
#include "components/rigid_body.hpp"
#include <glm/gtx/vector_angle.hpp>

namespace spk {
    float random_positive_float(float min, float max) {
        const float precision = 1000.0f;

        spk_assert(min < max);
        spk_assert(min >= 0.0f);

        float randomf = (float)(rand() % (uint32_t)precision);

        randomf = map_value(randomf, {0.0f, precision}, {min, max});

        return randomf;         
    };

    void process_particle_system(b2Body* body, double delta_time, comp_particles_t& ps) {
        if(!(ps.flags & PARTICLES_FLAGS_ACTIVE))
            return;

        /* now just a thought (JUST A THOUGHT), what if every particle was a entity and collidable?
            may be a todo in the future */

        // update the lifetime of every particle, remove those outdated
        bool erase = false;
        uint32_t amount = 0;
        for(uint32_t i = 0; i < ps.particles.size(); i++) {
            ps.particles[i].lifetime -= delta_time;

            if(ps.particles[i].lifetime <= 0.0f) {
                spk_assert(amount == 0, "amount to erase is non-zero;" 
                                        " meaning a particle is in a invalid" 
                                        " position based on its lifetime."
                                        " particles must be in decesnding order");

                if(!erase) {
                    erase = true;
                }
            } else { // found particle with valid lifetime
                ps.particles[i].pos += ps.particles[i].dir * (ps.particles[i].speed * (float)delta_time);

                ps.particles[i].speed += ps.speed_step;

                // to avoid switching the direction of the particle, we want to make
                // sure particle speed is not negative
                if(ps.particles[i].speed <= 0.0f) {
                    ps.particles[i].speed = 0.0f;
                }  else {
                    
                } 

                if(erase) {
                    amount = i + 1;
                    erase = false;
                }
            }
        }

        if(erase) { // if erase is still true, then the entire deque died
            ps.particles.clear();
        } else if(amount != 0) {
            ps.particles.erase(ps.particles.begin(), ps.particles.begin() + amount);
        }

        if(ps.current_cycle <= 0.0f) { // create new particle if cycle allows
            // delta time may make ps.current_cycle < 0 in a single tick
            // making ps.base_cycle unable to actually appear as its actual
            // rate, so really low cycles like 0.01 or 0.001 will cap out at delta time 
            // to counteract this we calculate how many cycles we have to make up for
            float cycles_to_do = glm::abs(ps.current_cycle) / ps.base_cycle;
            ps.current_cycle = ps.base_cycle;

            if(cycles_to_do <= 1.0f)
                cycles_to_do = 1;

            for(; cycles_to_do >= 1.0f; cycles_to_do--) {
                const float angle = glm::orientedAngle(ps.dir, glm::normalize((glm::vec2){0.0f, 1.0f}));
                float y = 0.0f; 

                while(y <= ps.length && ps.particles.size() < ps.max) {
                    if(random_positive_float(0.0f, 1.0f) < ps.chance) {
                        particle_t& p = ps.particles.emplace_back();

                        p.speed = ps.base_speed;
                        p.lifetime = ps.base_lifetime;
                        p.pos = {random_positive_float(0.0f, ps.width) - ps.width / 2.0f, y / ps.length};
                        p.pos = glm::rotate(p.pos, -angle);

                        glm::vec2 ps_world_pos = ps.get_point(body, ps.pos);
                        glm::vec2 p_world_pos  = ps.get_point(body, ps.pos + p.pos); 

                        if(ps.flags & PARTICLES_FLAGS_WORLD_POSITION) {
                            p.pos = p_world_pos;
                        }
                        
                        switch(ps.funnel) {
                            case PARTICLES_FUNNEL_LINE:
                                p.dir = ps.dir; 
                                break;

                            case PARTICLES_FUNNEL_FUNNEL:
                                p.dir = p_world_pos - ps_world_pos;
                                break;

                            default:
                                spk_assert(ps.funnel < PARTICLES_FUNNEL_LAST, "invalid particle system funnel");
                                break;
                        }
                    }

                    y += ps.step;
                } 
            }
        } else {
            ps.current_cycle -= delta_time;
        }
    }

    void particles_system_tick(flecs::iter& iter, comp_rigid_body_t* bodies, comp_particles_t* particles) {
        for(auto i : iter) {
            process_particle_system(bodies[i], iter.delta_time(), particles[i]);
        }
    }

    void particles_cs_init(flecs::world& world) {
        spk_trace();
        
        particles_comp_init(world);

        world.system<comp_rigid_body_t, comp_particles_t>().iter(particles_system_tick);
    } 
}