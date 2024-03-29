/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "sprite_renderer.hpp"
#include "core/data.hpp"

namespace spk {
    void sprite_renderer_t::particles_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_particles_t* particles) {
        spk_trace();

        auto renderer = (sprite_renderer_t*)render_context().renderers[RENDERER_TYPE_SPRITE];

        for(auto i : iter) {
            comp_rigid_body_t& body            = bodies[i];
            comp_particles_t&  particle_system = particles[i];
            tile_sprite_t&     sprite = particle_system.sprite;
        
            for(uint32_t j = 0; j < particle_system.particles.size(); j++) {
                particle_t& particle = particle_system.particles[j];

                vertex_t vertices[] = { 
                    {glm::vec3(particle.pos - SPK_TILE_HALF_SIZE, sprite.z),
                        glm::vec3(0.0f, 0.0f, sprite.index)}, // bl
                    {glm::vec3((glm::vec2){particle.pos.x + SPK_TILE_HALF_SIZE, particle.pos.y - SPK_TILE_HALF_SIZE}, sprite.z),
                        glm::vec3(1.0f, 0.0f, sprite.index)}, // br
                    {glm::vec3(particle.pos + SPK_TILE_HALF_SIZE, sprite.z),
                        glm::vec3(1.0f, 1.0f, sprite.index)}, // tr
                    {glm::vec3((glm::vec2){particle.pos.x - SPK_TILE_HALF_SIZE, particle.pos.y + SPK_TILE_HALF_SIZE}, sprite.z),
                        glm::vec3(0.0f, 1.0f, sprite.index)} // tl
                };

                if(!(particle_system.flags & PARTICLES_FLAGS_WORLD_POSITION)) {
                    for(vertex_t& v : vertices) {
                        v.pos = glm::vec3(particle_system.get_point(body, glm::vec2(v.pos)), 0.0f);
                    }
                }

                renderer->meshes[sprite.id].add_mesh(vertices);
            }   
        }  
    }

    glm::vec3 get_world_point(kin::rigid_body_t* body, glm::vec3 v3) {
        return {body->get_world_point((glm::vec2){v3.x, v3.y} + body->center_of_mass), v3.z};
    }

    void sprite_renderer_t::sprite_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_sprite_t* sprites) {
        spk_trace();

        auto renderer = (sprite_renderer_t*)render_context().renderers[RENDERER_TYPE_SPRITE];
        
        for(auto i : iter) {
            kin::rigid_body_t*     body   = bodies[i];
            comp_sprite_t sprite = sprites[i];
            
            vertex_t vertices[] = {
                {get_world_point(body, glm::vec3(glm::vec2(-sprite.size.x, -sprite.size.y) + sprite.offset, sprite.z)), 
                    glm::vec3(0.0f, 0.0f, sprite.index)},
                {get_world_point(body, glm::vec3(glm::vec2( sprite.size.x, -sprite.size.y) + sprite.offset, sprite.z)), 
                    glm::vec3(1.0f, 0.0f, sprite.index)},
                {get_world_point(body, glm::vec3(glm::vec2( sprite.size.x,  sprite.size.y) + sprite.offset, sprite.z)), 
                    glm::vec3(1.0f, 1.0f, sprite.index)},
                {get_world_point(body, glm::vec3(glm::vec2(-sprite.size.x,  sprite.size.y) + sprite.offset, sprite.z)), 
                    glm::vec3(0.0f, 1.0f, sprite.index)}
            };

            renderer->meshes[sprite.id].add_mesh(vertices);
        }        
    }

    void sprite_renderer_t::render() {
        spk_trace();

        auto& shader = render_context().shaders[SHADER_TYPE_SPRITE];
        auto& vao    = render_context().common.array;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for(auto& pair : sprite_arrays().map()) {
            auto&           mesh  = meshes[pair.first];
            sprite_array_t& array = pair.second;

            if(mesh.vertices_on_buffer > 0) {
                vao.bind();
                mesh.buffer.bind();
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), nullptr);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)sizeof(glm::vec3));
                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);

                render_context().common.quad_indexes.bind();
                shader.use();
                shader.set_mat4("u_vp", render_context().world_camera);

                shader.set_int("array", 0);
                array.texture_array.bind();
                glActiveTexture(GL_TEXTURE0);

                glDrawElements(GL_TRIANGLES, mesh.vertices_to_render, GL_UNSIGNED_INT, nullptr);   
                
                mesh.vertices_on_buffer = 0;
                mesh.vertices_to_render = 0;
            }
        }

        glDisable(GL_BLEND);
    }
}