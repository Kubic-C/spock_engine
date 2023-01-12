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

const char* vs_arrayd_sprite = R"###(
#version 330 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_tex_coords;

out vec3 v_tex_coords;

uniform mat4 u_vp;

void main() {
    gl_Position = u_vp * vec4(a_pos, 1.0);
    v_tex_coords = a_tex_coords;
})###";

const char* fs_arrayd_sprite = R"###(
#version 330 core
in vec3 v_tex_coords;

uniform sampler2DArray array;

out vec4 fragment_color;

void main() {
    fragment_color = texture(array, v_tex_coords);
})###";

namespace spk {
    void sprite_renderer_t::init() {
        b_init();

        {
            atlasd_sprites.init();
            atlasd_ctx.init();
            atlasd_ctx.vertex_layout.add(0, 3, GL_FLOAT, GL_FALSE, sizeof(sprite_atlasd_vertex_t), 0);
            atlasd_ctx.vertex_layout.add(1, 2, GL_FLOAT, GL_FALSE, sizeof(sprite_atlasd_vertex_t), sizeof(glm::vec3));
            
            bool ret = atlasd_ctx.program.load_shader_str(vs_sprite, fs_sprite);
            spk_assert(ret);
        }

        {
            arrayd_sprites.init();
            arrayd_ctx.init();
            arrayd_ctx.vertex_layout.add(0, 3, GL_FLOAT, GL_FALSE, sizeof(sprite_arrayd_vertex_t), 0);
            arrayd_ctx.vertex_layout.add(1, 3, GL_FLOAT, GL_FALSE, sizeof(sprite_arrayd_vertex_t), sizeof(glm::vec3));

            bool ret = arrayd_ctx.program.load_shader_str(vs_arrayd_sprite, fs_arrayd_sprite);
            spk_assert(ret);
        }

    }

    void sprite_renderer_t::free() {
        atlasd_ctx.free();
        atlasd_sprites.free();       
        arrayd_sprites.free(); 
        b_free();
    }

    void sprite_renderer_t::draw() {        
        auto                rs       = (render_system_t*)state.get_current_renderer();
        auto                camera   = state.get_current_camera().get_ref<comp_camera_t>();
        resource_manager_t* rsrc_mng = &state.engine->rsrc_mng;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        {
            atlasd_ctx.vertex_layout.set_buffer(0, atlasd_sprites.vertex_buffer);
            atlasd_ctx.vertex_layout.set_buffer(1, atlasd_sprites.vertex_buffer);
            atlasd_ctx.vertex_array.bind_layout(atlasd_ctx.vertex_layout);

            for(uint32_t i = 0; i < SPK_MAX_ATLAS; i++) {
                if(!rsrc_mng->sprite_atlas_is_in_use(i))
                    continue;

                auto&           atlas_mesh = atlasd_sprites.meshes[i];
                sprite_atlas_t* atlas = rsrc_mng->get_atlas(i);

                if(0 < atlas_mesh.sprites) {
                    atlasd_sprites.subdata(i);

                    rs->quad_index_buffer.bind();
                    atlasd_ctx.vertex_array.bind();
                    atlasd_ctx.program.use();
                    atlasd_ctx.program.set_mat4("u_vp", camera->vp);
                    atlasd_ctx.program.set_int("atlas", 0);
                    atlas->texture.active_texture(GL_TEXTURE0);
                    glDrawElements(GL_TRIANGLES, atlas_mesh.sprites * atlasd_sprites.vertexes_per_sprite, GL_UNSIGNED_INT, nullptr);   
                }
            }
        }

        {
            arrayd_ctx.vertex_layout.set_buffer(0, arrayd_sprites.vertex_buffer);
            arrayd_ctx.vertex_layout.set_buffer(1, arrayd_sprites.vertex_buffer);
            arrayd_ctx.vertex_array.bind_layout(arrayd_ctx.vertex_layout);

            for(uint32_t i = 0; i < SPK_MAX_SPRITE_ARRAYS; i++) {
                if(!rsrc_mng->sprite_array_is_in_use(i))
                    continue;

                auto&           array_mesh = arrayd_sprites.meshes[i];
                sprite_array_t* array      = rsrc_mng->sprite_array_get(i);

                if(0 < array_mesh.sprites) {
                    arrayd_sprites.subdata(i);

                    rs->quad_index_buffer.bind();
                    arrayd_ctx.vertex_array.bind();
                    arrayd_ctx.program.use();
                    arrayd_ctx.program.set_mat4("u_vp", camera->vp);

                    arrayd_ctx.program.set_int("array", 0);
                    array->texture_array.bind();
                    glActiveTexture(GL_TEXTURE0);

                    glDrawElements(GL_TRIANGLES, array_mesh.sprites * arrayd_sprites.vertexes_per_sprite, GL_UNSIGNED_INT, nullptr);   
                }
            }
        }

        glDisable(GL_BLEND);
    }   

    // glm::vec3 get_world_point(b2Body* body, glm::vec3 local_point) {
    //     return (glm::vec3){spk::to_glm_vec2(body->GetWorldPoint(b2Vec2(local_point.x, local_point.y))), local_point.z};
    // }

    // template<>
    // void sprite_batch_mesh_t<sprite_arrayd_vertex_t, comp_sprite_arrayd_t>::add_sprite_mesh(
    //     b2Body* body, 
    //     comp_sprite_arrayd_t& sprite,
    //     uint32_t sprites_x,
    //     uint32_t sprites_y,
    //     const glm::vec2& offset) {
    //     if(sprite.array_id == UINT32_MAX)
    //         return;
        
    //     float                    half_width  = (float)sprites_x / 2.0f;
    //     float                    half_height = (float)sprites_y / 2.0f;
    //     resource_manager_t*      rsrc_mng   = &state.engine->rsrc_mng;
    //     const uint32_t           array_id   = sprite.array_id;
    //     sprite_array_t*          atlas      = rsrc_mng->sprite_array_get(array_id);
    //     uint32_t                 index      = meshes[array_id].sprites * indexes_per_sprite;

    //     meshes[array_id].sprites += 1;
    //     resize_mesh_if_need(array_id);

    //     meshes[array_id].mesh[index + 0] = 
    //         { get_world_point(body, (glm::vec3){ glm::vec2(-half_width, -half_height) + offset, sprite.z}), 
    //           glm::vec3(0.0f, 0.0f, sprite.index) };

    //     meshes[array_id].mesh[index + 1] = 
    //         { get_world_point(body, (glm::vec3){ glm::vec2(half_width, -half_height) + offset, sprite.z}),
    //           glm::vec3(1.0f * sprites_x, 0.0f, sprite.index) };

    //     meshes[array_id].mesh[index + 2] = 
    //         { get_world_point(body, (glm::vec3){ glm::vec2(half_width, half_height) + offset, sprite.z}),  
    //           glm::vec3(1.0f * sprites_x, 1.0f * sprites_y, sprite.index) };

    //     meshes[array_id].mesh[index + 3] = 
    //         { get_world_point(body, (glm::vec3){ glm::vec2(-half_width, half_height) + offset, sprite.z}),  
    //           glm::vec3(0.0f, 1.0f * sprites_y, sprite.index)};
    // }

    template<>
    void sprite_batch_mesh_t<sprite_arrayd_vertex_t, comp_sprite_arrayd_t>::add_sprite_mesh(
            comp_sprite_arrayd_t& sprite, 
            const glm::vec2& _1, 
            const glm::vec2& _2, 
            const glm::vec2& _3, 
            const glm::vec2& _4) {
        if(sprite.array_id == UINT32_MAX)
            return;
        
        resource_manager_t*      rsrc_mng   = &state.engine->rsrc_mng;
        const uint32_t           array_id   = sprite.array_id;
        sprite_array_t*          atlas      = rsrc_mng->sprite_array_get(array_id);
        uint32_t                 index      = meshes[array_id].sprites * indexes_per_sprite;

        meshes[array_id].sprites += 1;
        resize_mesh_if_need(array_id);

        meshes[array_id].mesh[index + 0] = {glm::vec3(_1, sprite.z), glm::vec3(0.0f, 0.0f, sprite.index)};
        meshes[array_id].mesh[index + 1] = {glm::vec3(_2, sprite.z), glm::vec3(1.0f, 0.0f, sprite.index)};
        meshes[array_id].mesh[index + 2] = {glm::vec3(_3, sprite.z), glm::vec3(1.0f, 1.0f, sprite.index)};
        meshes[array_id].mesh[index + 3] = {glm::vec3(_4, sprite.z), glm::vec3(0.0f, 1.0f, sprite.index)};
    }

    template<>
    void sprite_batch_mesh_t<sprite_atlasd_vertex_t, comp_sprite_atlasd_t>::add_sprite_mesh(
            comp_sprite_atlasd_t& sprite, 
            const glm::vec2& _1, 
            const glm::vec2& _2, 
            const glm::vec2& _3, 
            const glm::vec2& _4) { 
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
}