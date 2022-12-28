#include "particles.hpp"
#include "utility/ui.hpp"
#include "components/tilebody.hpp"
#include "systems/sprite_render.hpp"
#include <glm/gtx/vector_angle.hpp>
#include "state.hpp"
#include "spock.hpp"

namespace spk {
    float random_positive_float(float min, float max) {
        const float precision = 1000.0f;

        sfk_assert(min < max);
        sfk_assert(min >= 0.0f);

        float randomf = (float)(rand() % (uint32_t)precision);

        randomf = map_value(randomf, {0.0f, precision}, {min, max});

        return randomf;         
    };

    void process_particle_system(b2Body* body, float delta_time, comp_particles_t& ps) {
        if(!(ps.flags & PARTICLES_FLAGS_ACTIVE))
            return;

        /* now just a thought (JUST A THOUGHT), what if every particle was a entity*/

        // update the lifetime of every particle, remove those outdated
        bool erase = false;
        uint32_t amount = 0;
        for(uint32_t i = 0; i < ps.particles.size(); i++) {
            ps.particles[i].lifetime -= delta_time;

            if(ps.particles[i].lifetime <= 0.0f) {
                sfk_assert(amount == 0, "amount to erase is non-zero;" 
                                        " meaning a particle is in a invalid" 
                                        " position based on its lifetime."
                                        " particles must be in decesnding order");

                if(!erase) {
                    erase = true;
                }
            } else { // found particle with valid lifetime
                ps.particles[i].pos += ps.particles[i].dir * (ps.particles[i].speed * delta_time);

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
                const float angle = glm::angle(ps.dir, glm::normalize((glm::vec2){0.0f, 1.0f}));
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

                        if(ps.flags & PARTICLES_FLAGS_COLLIADABLE) {
                            p.init_body(ps.particle.id, body->GetWorld());
                            p.body->SetTransform(to_box_vec2(p.pos), 0.0f);
                        }

                        switch(ps.funnel) {
                            case PARTICLES_FUNNEL_LINE:
                                p.dir = ps.dir; 
                                break;

                            case PARTICLES_FUNNEL_FUNNEL:
                                p.dir = p_world_pos - ps_world_pos;
                                break;

                            default:
                                sfk_assert(ps.funnel < PARTICLES_FUNNEL_LAST, "invalid particle system funnel");
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

    void particles_system_tick(flecs::iter& iter, comp_b2Body_t* bodies, comp_particles_t* particles) {
        for(auto i : iter) {
            process_particle_system(bodies[i].body, stats.delta_time, particles[i]);
        }
    }

    void particles_system_tile_body_tick(flecs::iter& iter, comp_tilebody_t* bodies, comp_particles_t* particles) {
        for(auto i : iter) {
            process_particle_system(bodies[i].body, stats.delta_time, particles[i]);
        }
    }

    void add_particles(flecs::ref<sprite_render_system_ctx_t>& ctx, b2Body* body, comp_particles_t& ps) {
        tile_metadata_t& tmd = state.engine->rsrc_mng.get_tile_dictionary()[ps.particle.id];
       
        for(uint32_t j = 0; j < ps.particles.size(); j++) {
                particle_t& particle = ps.particles[j];

                std::array<glm::vec2, 4> vertexes = { 
                    (glm::vec2){particle.pos - tmd.sprite.size}, // bl
                    (glm::vec2){particle.pos.x + tmd.sprite.size.x, particle.pos.y - tmd.sprite.size.y}, // br
                    (glm::vec2){particle.pos + tmd.sprite.size}, // tr
                    (glm::vec2){particle.pos.x - tmd.sprite.size.x, particle.pos.y + tmd.sprite.size.y} // tl
                };
                
                if(!(ps.flags & PARTICLES_FLAGS_WORLD_POSITION)) {
                    for(glm::vec2& v : vertexes) {
                        v = ps.get_point(body, v);
                    }
                }

                ctx->add_sprite_mesh(tmd.sprite, vertexes[0], vertexes[1], vertexes[2], vertexes[3]);
        }         
    }

    void particles_system_update(flecs::iter& iter, comp_b2Body_t* bodies, comp_particles_t* particles) {
        auto ctx = SPK_GET_CTX_REF(iter, sprite_render_system_ctx_t);

        for(auto i : iter) {
            add_particles(ctx, bodies[i].body, particles[i]);            
        }

        ctx->draw_atlas_meshes();
    }


    void particles_system_tilebody_update(flecs::iter& iter, comp_tilebody_t* bodies, comp_particles_t* particles) {
        auto ctx = SPK_GET_CTX_REF(iter, sprite_render_system_ctx_t);

        for(auto i : iter) {
            add_particles(ctx, bodies[i].body, particles[i]);            
        }

        ctx->draw_atlas_meshes();
    }

    void _particles_cs_init(flecs::entity* ctx, flecs::world& world) {
        particles_comp_init(world);

        world.system<comp_b2Body_t, comp_particles_t>().iter(particles_system_tick);
        world.system<comp_tilebody_t, comp_particles_t>().iter(particles_system_tile_body_tick);

        world.system<comp_b2Body_t, comp_particles_t>().kind(flecs::OnUpdate)
            .ctx(ctx).kind(on_render).iter(particles_system_update).add<render_system_t>();  
        world.system<comp_tilebody_t, comp_particles_t>().kind(flecs::OnUpdate)
            .ctx(ctx).kind(on_render).iter(particles_system_tilebody_update).add<render_system_t>();  
    } 
}