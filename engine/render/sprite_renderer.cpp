#include "sprite_renderer.hpp"
#include "renderer.hpp"

namespace spk {
    void sprite_renderer_t::sprite_mesh(flecs::iter& iter, comp_rigid_body_t* bodies, comp_sprite_t* sprites) {
        auto renderer = (sprite_renderer_t*)render_context().renderers[RENDERER_TYPE_SPRITE];
        
        for(auto i : iter) {
            b2Body* body         = bodies[i];
            comp_sprite_t sprite = sprites[i];
            auto& buffer = renderer->meshes[sprite.array_id].buffer; 
            auto& vertexes_on_buffer = renderer->meshes[sprite.array_id].vertexes_on_buffer;
            auto& vertexes_to_render = renderer->meshes[sprite.array_id].vertexes_to_render;

            // resize if needed
            if(buffer.size() < vertexes_on_buffer + 4) 
                buffer.resize(buffer.size() + 4);

            renderer->meshes[sprite.array_id].buffer.bind();
            
            b2Vec2 vertices[] = {
                body->GetWorldPoint(b2Vec2(-sprite.size.x, -sprite.size.y)),
                body->GetWorldPoint(b2Vec2( sprite.size.x, -sprite.size.y)),
                body->GetWorldPoint(b2Vec2( sprite.size.x,  sprite.size.y)),
                body->GetWorldPoint(b2Vec2(-sprite.size.x,  sprite.size.y))
            };

            buffer.buffer_sub_data(vertexes_on_buffer * sizeof(float), sizeof(vertices), vertices);
            vertexes_on_buffer += 4;
            vertexes_on_buffer += 6;
        }        
    }

    void sprite_renderer_t::render() {
        auto& shader = render_context().shaders[SHADER_TYPE_SPRITE];
        auto& vao    = render_context().common.array;

        for(uint32_t i = 0; i < SPK_MAX_SPRITE_ARRAYS; i++) {
            spk_assert(!resources().sprite_arrays.is_in_use(i));

            auto&           mesh = meshes[i];
            sprite_array_t* array      = resources().sprite_arrays.get(i);

            if(mesh.vertexes_on_buffer > 0) {
                vao.bind();
                mesh.buffer.bind();
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)sizeof(glm::vec3));
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
    }
}