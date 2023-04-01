#include "container_renderer.hpp"
#include "core/internal.hpp"

namespace spk {

    void container_renderer_t::button_mesh(const button_t& button) {
        auto& dim  = button.dimensions_get();
        auto& mesh = meshes[button.sprite_array_id];
        glm::vec4 color;

        if(button.clicked_get()) {
            color = glm::vec4(button.click_color, button.color.a);
        } else if(button.hovering_get()) {
            color = glm::vec4(button.hover_color, button.color.a);
        } else {
            color = button.color;
        }

        vertex_t vertices[] = {
            {{dim.pos.x             , dim.pos.y             , dim.pos.z}, {0.0f, 0.0f, button.sprite_index}, color},
            {{dim.pos.x + dim.size.x, dim.pos.y             , dim.pos.z}, {1.0f, 0.0f, button.sprite_index}, color},
            {{dim.pos.x + dim.size.x, dim.pos.y + dim.size.y, dim.pos.z}, {1.0f, 1.0f, button.sprite_index}, color},
            {{dim.pos.x             , dim.pos.y + dim.size.y, dim.pos.z}, {0.0f, 1.0f, button.sprite_index}, color}
        };

        mesh.add_mesh(vertices);
    }

    void container_renderer_t::container_mesh(const container_t& container) {
        auto&            dim   = container.dimensions_get();
        auto&            mesh  = meshes[container.sprite_array_id];
        const glm::vec4& color = container.color;

        vertex_t vertices[] = {
            {{dim.pos.x             , dim.pos.y             , dim.pos.z}, {0.0f, 0.0f, container.sprite_index}, color},
            {{dim.pos.x + dim.size.x, dim.pos.y             , dim.pos.z}, {1.0f, 0.0f, container.sprite_index}, color},
            {{dim.pos.x + dim.size.x, dim.pos.y + dim.size.y, dim.pos.z}, {1.0f, 1.0f, container.sprite_index}, color},
            {{dim.pos.x             , dim.pos.y + dim.size.y, dim.pos.z}, {0.0f, 1.0f, container.sprite_index}, color}
        };

        mesh.add_mesh(vertices);
    }

    void container_renderer_t::render() {
        render_context().common.array.bind();
    
        auto& shader = render_context().shaders[SHADER_TYPE_CONTAINER];
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
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, tex));
                glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, color));
                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);
                glEnableVertexAttribArray(2);

                render_context().common.quad_indexes.bind();

                shader.use();
                shader.set_mat4("u_vp", render_context().ui_camera);

                if(array.texture_array.id != UINT32_MAX) {
                    shader.set_int("array", 0);
                    array.texture_array.bind();
                    glActiveTexture(GL_TEXTURE0);
                }

                glDrawElements(GL_TRIANGLES, mesh.vertices_to_render, GL_UNSIGNED_INT, nullptr);   
                
                mesh.vertices_on_buffer = 0;
                mesh.vertices_to_render = 0;
            }
        }

        glDisable(GL_BLEND);
    }
}