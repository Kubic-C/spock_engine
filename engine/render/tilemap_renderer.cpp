/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "tilemap_renderer.hpp"
#include "core/internal.hpp"

namespace spk {
    void tilemap_renderer_t::tilemap_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_tilemap_t* tilemaps) {
        spk_trace();

        auto renderer = render_context().renderers[RENDERER_TYPE_TILEMAP].cast<tilemap_renderer_t>();

        for(auto i : iter) {
            comp_rigid_body_t&  body    = bodies[i];
            comp_tilemap_t&     tilemap = tilemaps[i];

            _tilemap_mesh_update(&body, tilemap);
        }
    }

    void tilemap_renderer_t::_chunk_render(kin::rigid_body_t* body, tile_chunk_mesh_t& mesh) {

        for(auto& pair : mesh.buffer) {
            sprite_array_t& array = sprite_arrays().get(pair.first);
            auto&           mesh  = pair.second;

            if(mesh.vertices_on_buffer > 0) {
                mesh.buffer.bind();
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(tilemap_vertex_t), nullptr);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(tilemap_vertex_t), (void*)sizeof(glm::vec3));
                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);

                array.texture_array.bind();
                render_context().common.quad_indexes.bind();

                glDrawElements(GL_TRIANGLES, mesh.vertices_to_render, GL_UNSIGNED_INT, nullptr);   
            }
        }
    } 

    void tilemap_renderer_t::tilemap_render(flecs::iter& iter, comp_rigid_body_t* bodies, comp_tilemap_t* tilemaps) {
        spk_trace();

        auto  renderer = render_context().renderers[RENDERER_TYPE_TILEMAP].cast<tilemap_renderer_t>();
        auto& shader = render_context().shaders[SHADER_TYPE_TILEMAP];
        auto& vao    = render_context().common.array;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        vao.bind();
        
        shader.use();
        shader.set_mat4("u_vp", render_context().world_camera);
        shader.set_int("array", 0);
        glActiveTexture(GL_TEXTURE0);

        for(auto i : iter) {
            comp_rigid_body_t&  body    = bodies[i];
            comp_tilemap_t&     tilemap = tilemaps[i];
            
            glm::mat4           local_model   = glm::mat4(1.0f); 
            local_model = glm::rotate(local_model, body->get_world_rot(), glm::vec3(0.0f, 0.0f, 1.0f));
            local_model = glm::translate(local_model, glm::vec3(-body->center_of_mass, 0.0f));
            glm::mat4 world_model = glm::translate(glm::mat4(1.0f), glm::vec3(body->get_world_pos(), 0.0f)) * local_model;

            shader.set_mat4("u_model", world_model);

            for(size_t x = 0; x < tilemap_max_width; x++) {
                for(size_t y = 0; y < tilemap_max_height; y++) {
                    tile_chunk_mesh_t& mesh = (*tilemap.mesh)[x][y];

                    renderer->_chunk_render(body, mesh);
                }   
            }
        }

        glDisable(GL_BLEND);
    }

    void render() {
        spk_trace();

    }
}