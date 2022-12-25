#include "particles.hpp"
#include <glm/gtx/rotate_vector.hpp>
#include "../state.hpp"
#include "../utility/ui.hpp"

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
        if(!(ps.flags & PARTICLE_FLAG_ACTIVE))
            return;

        float angle = glm::dot({0.0f, 1.0f}, ps.dir);

        if(angle == 1.0f) {
            angle = 0.0f;
        }

        // update the lifetime of every particle, remove those outdated
        bool erase = false;
        uint32_t amount = 0;
        for(uint32_t i = 0; i < ps.particles.size(); i++) {
            ps.particles[i].lifetime -= delta_time;

            if(ps.particles[i].lifetime <= 0.0f) {
                if(!erase) {
                    sfk_assert(amount == 0, "amount to erase is non-zero;" 
                                                " meaning a particle is in a invalid" 
                                                " position based on its lifetime."
                                            " particles must be in decesnding order");
                    erase = true;
                }
            } else { // found particle with valid lifetime
                ps.particles[i].pos += ps.particles[i].dir * (ps.speed * delta_time);

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
            float y = 0.0f; 

            ps.current_cycle = ps.base_cycle;

            while(y <= ps.length && ps.particles.size() < ps.max) {
                if(random_positive_float(0.0f, 1.0f) < ps.chance) {
                    particle_t& p = ps.particles.emplace_back();

                    p.lifetime = ps.base_lifetime;
                    p.pos = {random_positive_float(0.0f, ps.width) - ps.width / 2.0f, y / ps.length};

                    p.pos = glm::rotate(p.pos, -angle);

                    if(ps.world_positioning)
                        p.pos = sfk::to_glm_vec2(body->GetWorldPoint(sfk::to_box_vec2(ps.pos + p.pos)));
                    
                    const b2Vec2& b2pos = sfk::to_box_vec2(p.pos);

                    switch(ps.funnel) {
                        case PARTICLE_SYSTEM_FUNNEL_LINE:
                            p.dir = ps.dir;
                            break;

                        case PARTICLE_SYSTEM_FUNNEL_FUNNEL:
                            if(ps.world_positioning)
                                p.dir = sfk::to_glm_vec2(b2pos - body->GetWorldPoint(sfk::to_box_vec2(ps.pos)));
                            else {
                                p.dir = p.pos - ps.pos;
                            }
                            break;

                        default:
                            sfk_assert(ps.funnel < PARTICLE_SYSTEM_FUNNEL_LAST, "invalid particle system funnel");
                            break;
                    }
                }

                y += ps.step;
            } 
        } else {
            ps.current_cycle -= delta_time;
        }
    }

    void particles_system_tick(flecs::iter& iter, comp_b2Body_t* bodies, comp_particles_t* particles) {
        for(auto i : iter) {
            process_particle_system(bodies[i].body, iter.delta_time(), particles[i]);
        }
    }

    void particles_system_tile_body_tick(flecs::iter& iter, comp_tilebody_t* bodies, comp_particles_t* particles) {
        for(auto i : iter) {
            process_particle_system(bodies[i].body, iter.delta_time(), particles[i]);
        }
    }

    void add_particles(flecs::ref<sprite_render_system_ctx_t>& ctx, b2Body* body, comp_particles_t& ps) {
        for(uint32_t j = 0; j < ps.particles.size(); j++) {
                particle_t& particle = ps.particles[j];

                std::array<glm::vec2, 4> vertexes = { 
                    (glm::vec2){particle.pos - ps.sprite.size}, // bl
                    (glm::vec2){particle.pos.x + ps.sprite.size.x, particle.pos.y - ps.sprite.size.y}, // br
                    (glm::vec2){particle.pos + ps.sprite.size}, // tr
                    (glm::vec2){particle.pos.x - ps.sprite.size.x, particle.pos.y + ps.sprite.size.y} // tl
                };
                
                if(!ps.world_positioning) {
                    for(glm::vec2& v : vertexes) {
                        v = sfk::to_glm_vec2(body->GetWorldPoint(sfk::to_box_vec2(ps.pos + v)));
                    }
                }

                ctx->add_sprite_mesh(ps.sprite, vertexes[0], vertexes[1], vertexes[2], vertexes[3]);
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

        world.system<comp_b2Body_t, comp_particles_t>()
            .kind(flecs::OnUpdate).interval(state.get_target_tps()).ctx(ctx).iter(particles_system_tick);
        world.system<comp_tilebody_t, comp_particles_t>()
            .kind(flecs::OnUpdate).interval(state.get_target_tps()).ctx(ctx).iter(particles_system_tile_body_tick);

        world.system<comp_b2Body_t, comp_particles_t>().kind(flecs::OnUpdate)
            .ctx(ctx).iter(particles_system_update);  
        world.system<comp_tilebody_t, comp_particles_t>().kind(flecs::OnUpdate)
            .ctx(ctx).iter(particles_system_tilebody_update);  
    } 
}