#include "sprite_renderer.hpp"
#include "state.hpp"
#include "spock.hpp"

const char* vs_sprite = R"###(
#version 330 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_tex_coords;

out vec2 v_tex_coords;

uniform mat4 u_vp;

void main() {
    gl_Position = u_vp * vec4(a_pos, 1.0);
    v_tex_coords = a_tex_coords;
})###";

const char* fs_sprite = R"###(
#version 330 core
in vec2 v_tex_coords;

uniform sampler2D atlas;

out vec4 fragment_color;

void main() {
    fragment_color = texture(atlas, v_tex_coords);
})###";

namespace spk {
    void sprite_batch_mesh_t::init() {
        m_init();
        max_vertexes = 1024;

        vertex_buffer.buffer_data(sizeof(sprite_vertex_t) * max_vertexes, nullptr, GL_DYNAMIC_DRAW);

        for(auto& mesh : meshes)  {
            mesh.mesh.resize(max_vertexes);
        }       
    }

    void sprite_batch_mesh_t::free() {
        m_free();
    }
    
    void sprite_batch_mesh_t::add_sprite_mesh(comp_sprite_t& sprite, const glm::vec2& _1, const glm::vec2& _2, const glm::vec2& _3, const glm::vec2& _4) { 
        if(sprite.atlas_id == UINT32_MAX)
            return;
        
        resource_manager_t*      rsrc_mng   = &state.engine->rsrc_mng;
        const uint32_t           atlas_id   = sprite.atlas_id;
        sprite_atlas_t*          atlas      = rsrc_mng->get_atlas(atlas_id);
        uint32_t                 index      = meshes[atlas_id].sprites * indexes_per_sprite;
        std::array<glm::vec2, 4> tex_coords = atlas->gen_tex_coords(sprite.tax, sprite.tay);

        meshes[atlas_id].sprites += 1;
        resize_mesh_if_need(atlas_id);

        meshes[atlas_id].mesh[index + 0] = {glm::vec3(_1, sprite.z),  tex_coords[0]};
        meshes[atlas_id].mesh[index + 1] = {glm::vec3(_2, sprite.z),  tex_coords[1]};
        meshes[atlas_id].mesh[index + 2] = {glm::vec3(_3, sprite.z),  tex_coords[2]};
        meshes[atlas_id].mesh[index + 3] = {glm::vec3(_4, sprite.z),  tex_coords[3]};
    }

    void sprite_batch_mesh_t::add_sprite_mesh(b2Body* body, comp_sprite_t& sprite, glm::vec2 offset) {
        add_sprite_mesh(sprite, 
            spk::to_glm_vec2(body->GetWorldPoint((b2Vec2){-sprite.size.x, -sprite.size.y} + spk::to_box_vec2(offset))),
            spk::to_glm_vec2(body->GetWorldPoint((b2Vec2){ sprite.size.x, -sprite.size.y} + spk::to_box_vec2(offset))),
            spk::to_glm_vec2(body->GetWorldPoint((b2Vec2){ sprite.size.x,  sprite.size.y} + spk::to_box_vec2(offset))),
            spk::to_glm_vec2(body->GetWorldPoint((b2Vec2){-sprite.size.x,  sprite.size.y} + spk::to_box_vec2(offset))));
    }

    void sprite_batch_mesh_t::resize_mesh_if_need(size_t atlas) {
        size_t sprite_vertex_count = meshes[atlas].sprites * indexes_per_sprite;

        if(sprite_vertex_count > meshes[atlas].mesh.size()) {
            meshes[atlas].mesh.resize(sprite_vertex_count * 2);
        }
    }

    void sprite_batch_mesh_t::subdata(uint32_t atlas) {
        size_t sprite_vertex_count = meshes[atlas].sprites * indexes_per_sprite;

        if(sprite_vertex_count > max_vertexes) {
            resize(sizeof(sprite_vertex_t), sprite_vertex_count * 2, 0);
            
            max_vertexes = sprite_vertex_count * 2;
        }

        vertex_buffer.buffer_sub_data(0, sizeof(sprite_vertex_t) * sprite_vertex_count, meshes[atlas].mesh.data());
    }

    void sprite_renderer_t::init() {
        b_init();

        sprites.init();
        vertex_layout.add(0, 3, GL_FLOAT, GL_FALSE, sizeof(sprite_vertex_t), 0, 0);
        vertex_layout.add(1, 2, GL_FLOAT, GL_FALSE, sizeof(sprite_vertex_t), sizeof(glm::vec3), 0);

        program.load_shader_str(vs_sprite, fs_sprite);
    }

    void sprite_renderer_t::free() {
        sprites.free();        
        b_free();
    }

    void sprite_renderer_t::draw() {        
        auto                rs       = (render_system_t*)state.get_current_renderer();
        auto                camera   = state.get_current_camera().get_ref<comp_camera_t>();
        resource_manager_t* rsrc_mng = &state.engine->rsrc_mng;

        vertex_layout.set_buffer(0, sprites.vertex_buffer);
        vertex_layout.set_buffer(1, sprites.vertex_buffer);
        vertex_array.bind_layout(vertex_layout);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for(uint32_t i = 0; i < SPK_MAX_ATLAS; i++) {
            if(!rsrc_mng->atlas_is_in_use(i))
                continue;

            sprite_batch_mesh_t::atlas_mesh_t& atlas_mesh = sprites.meshes[i];
            sprite_atlas_t* atlas = rsrc_mng->get_atlas(i);

            if(0 < atlas_mesh.sprites) {
                sprites.subdata(i);
                vertex_array.bind();

                rs->quad_index_buffer.bind();
                program.use();
                program.set_mat4("u_vp", camera->vp);
                program.set_int("atlas", 0);
                atlas->texture.active_texture(GL_TEXTURE0);
                glDrawElements(GL_TRIANGLES, atlas_mesh.sprites * sprites.vertexes_per_sprite, GL_UNSIGNED_INT, nullptr);   
            }
        }

        glDisable(GL_BLEND);
    }   
}