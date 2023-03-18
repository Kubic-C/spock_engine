/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "sprite_renderer.hpp"
#include "core/data.hpp"

namespace spk {
    struct vertex_t {
        glm::vec3 pos;
        glm::vec3 tex;
    };
    
    glm::vec3 get_world_point(b2Body* body, glm::vec3 local_point) {
        return glm::vec3{(glm::vec2)body->GetWorldPoint(b2Vec2(local_point.x, local_point.y)), local_point.z};
    }

    void sprite_renderer_t::add_mesh(const sprite_arrayd_t& sprite, void* vertices) {
        auto& buffer             = meshes[sprite.array_id].buffer; 
        auto& vertexes_on_buffer = meshes[sprite.array_id].vertexes_on_buffer;
        auto& vertexes_to_render = meshes[sprite.array_id].vertexes_to_render;

        if(buffer.size() < (vertexes_on_buffer + 4) * sizeof(vertex_t)) 
            buffer.resize((vertexes_on_buffer + 4) * sizeof(vertex_t));

        buffer.bind();
        buffer.buffer_sub_data(vertexes_on_buffer * sizeof(vertex_t), sizeof(vertex_t) * 4, vertices);
        vertexes_on_buffer += 4;
        vertexes_to_render += 6;
    }

    void sprite_renderer_t::particles_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_particles_t* particles) {
        spk_trace();

        auto renderer = (sprite_renderer_t*)render_context().renderers[RENDERER_TYPE_SPRITE];

        for(auto i : iter) {
            comp_rigid_body_t& body            = bodies[i];
            comp_particles_t&  particle_system = particles[i];
            sprite_arrayd_t&   sprite = resources().tile_dictionary[particle_system.particle.id].sprite;
        
            for(uint32_t j = 0; j < particle_system.particles.size(); j++) {
                particle_t& particle = particle_system.particles[j];

                vertex_t vertices[] = { 
                    {glm::vec3(particle.pos - sprite.size, sprite.z),
                        glm::vec3(0.0f, 0.0f, sprite.index)}, // bl
                    {glm::vec3((glm::vec2){particle.pos.x + sprite.size.x, particle.pos.y - sprite.size.y}, sprite.z),
                        glm::vec3(1.0f, 0.0f, sprite.index)}, // br
                    {glm::vec3(particle.pos + sprite.size, sprite.z),
                        glm::vec3(1.0f, 1.0f, sprite.index)}, // tr
                    {glm::vec3((glm::vec2){particle.pos.x - sprite.size.x, particle.pos.y + sprite.size.y}, sprite.z),
                        glm::vec3(0.0f, 1.0f, sprite.index)} // tl
                };

                if(!(particle_system.flags & PARTICLES_FLAGS_WORLD_POSITION)) {
                    for(vertex_t& v : vertices) {
                        v.pos = glm::vec3(particle_system.get_point(body, glm::vec2(v.pos)), 0.0f);
                    }
                }

                renderer->add_mesh(sprite, vertices);
            }   
        }  
    }

    void sprite_renderer_t::tilemap_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_tilemap_t* tilemaps) {
        spk_trace();

        auto renderer = (sprite_renderer_t*)render_context().renderers[RENDERER_TYPE_SPRITE];

        for(auto i : iter) {
            comp_rigid_body_t&  body    = bodies[i];
            comp_tilemap_t&     tilemap = tilemaps[i];

            for(auto& pair : tilemap.tile_groups) {
                glm::uvec2   coords = tilemap.tiles.get_2D_from_1D(pair.first);
                tile_group_t tile   = pair.second;
                auto&        sprite = resources().tile_dictionary[tilemap.tiles.get(coords.x, coords.y).id].sprite;
                glm::vec2    offset = {};
                float        half_width  = tile.x / 2.0f, 
                             half_height = tile.y / 2.0f;

                if(tilemap.tiles.get(coords.x, coords.y).id == 0)
                    continue;

                offset.x = (coords.x + SPK_TILE_HALF_SIZE) - (float)tile.x  / 2.0f - tilemap.center.x;
                offset.y = (coords.y + SPK_TILE_HALF_SIZE) - (float)tile.y  / 2.0f - tilemap.center.y; 

                vertex_t vertices[] = {
                    { get_world_point(body, (glm::vec3){ glm::vec2(-half_width, -half_height) + offset, sprite.z}), 
                        glm::vec3(0.0f, 0.0f, sprite.index) },

                    { get_world_point(body, (glm::vec3){ glm::vec2(half_width, -half_height) + offset, sprite.z}),
                        glm::vec3(1.0f * tile.x, 0.0f, sprite.index) },

                    { get_world_point(body, (glm::vec3){ glm::vec2(half_width, half_height) + offset, sprite.z}),  
                        glm::vec3(1.0f * tile.x, 1.0f * tile.y, sprite.index) },

                    { get_world_point(body, (glm::vec3){ glm::vec2(-half_width, half_height) + offset, sprite.z}),  
                        glm::vec3(0.0f, 1.0f * tile.y, sprite.index)}
                };

                renderer->add_mesh(sprite, vertices);
            }
        }
    }

    void sprite_renderer_t::sprite_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_sprite_t* sprites) {
        spk_trace();

        auto renderer = (sprite_renderer_t*)render_context().renderers[RENDERER_TYPE_SPRITE];
        
        for(auto i : iter) {
            b2Body* body         = bodies[i];
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

            renderer->add_mesh(sprite, vertices);
        }        
    }

    void sprite_renderer_t::render() {
        spk_trace();

        auto& shader = render_context().shaders[SHADER_TYPE_SPRITE];
        auto& vao    = render_context().common.array;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for(uint32_t i = 0; i < SPK_MAX_SPRITE_ARRAYS; i++) {
            if(!resources().sprite_arrays.is_in_use(i))
                continue;

            auto&           mesh = meshes[i];
            sprite_array_t* array      = resources().sprite_arrays.get(i);

            if(mesh.vertexes_on_buffer > 0) {
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
                array->texture_array.bind();
                glActiveTexture(GL_TEXTURE0);

                glDrawElements(GL_TRIANGLES, mesh.vertexes_to_render, GL_UNSIGNED_INT, nullptr);   
                
                mesh.vertexes_on_buffer = 0;
                mesh.vertexes_to_render = 0;
            }
        }

        glDisable(GL_BLEND);
    }
}