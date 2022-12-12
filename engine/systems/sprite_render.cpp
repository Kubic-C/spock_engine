#include "sprite_render.hpp"
#include "../state.hpp"
#include "../spock.hpp"
#include <glm/gtx/rotate_vector.hpp> 

const char* vs_sprite = R"###(
#version 330 core
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_tex_coords;

out vec2 v_tex_coords;

uniform mat4 u_vp;

void main() {
    gl_Position = u_vp * vec4(a_pos, 0.0, 1.0);
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

    void sprite_render_system_ctx_t::init() {
        vertex_array.init();
        vertex_array.bind();
        
        vertex_buffer.init(GL_ARRAY_BUFFER);
        vertex_buffer.buffer_data(sizeof(vertex_t) * 4 * 1000, nullptr, GL_DYNAMIC_DRAW);
        vertex_layout.add(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0, vertex_buffer);
        vertex_layout.add(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, sizeof(float) * 2, vertex_buffer);
        vertex_array.bind_layout(vertex_layout);

        program.init();
        bool ret = program.load_shader_str(vs_sprite, fs_sprite);
        sfk_assert(ret && "sprite render shaders invalid");

        for(uint32_t i = 0; i < SPK_MAX_ATLAS; i++) {
            meshes[i].mesh.resize(1000 * 4); // 4 vertex per sprite
            meshes[i].sprites = 0;
        }
    }

    void sprite_render_system_ctx_t::free() {
        vertex_array.free();
        vertex_buffer.free();
        program.free();
    }

    void sprite_render_system_update(flecs::iter& iter, comp_b2Body_t* bodies, comp_sprite_t* sprites) {
        auto render_ctx = state.get_current_renderer().get_ref<render_system_ctx_t>(); // this is a safe op as render system only has pre and post update
        auto ctx = SPK_GET_CTX_REF(iter, sprite_render_system_ctx_t);
        resource_manager_t* rsrc_mng = &state.engine->rsrc_mng;

        for(auto i : iter) {
            comp_b2Body_t&  body = bodies[i];
            comp_sprite_t&  sprite = sprites[i];

            if(sprite.atlas_id == UINT32_MAX)
                continue;

            const uint32_t atlas_id = sprite.atlas_id;
            sprite_atlas_t* atlas = rsrc_mng->get_atlas(atlas_id);
            uint32_t index = ctx->meshes[atlas_id].sprites * ctx->indexes_per_sprite;
            std::array<glm::vec2, 4> tex_coords = atlas->gen_tex_coords(sprite.tax, sprite.tay);

            ctx->meshes[atlas_id].mesh[index + 0] = { {body.get_world_point(-sprite.size.x, -sprite.size.y)},  tex_coords[0]};
            ctx->meshes[atlas_id].mesh[index + 1] = { {body.get_world_point( sprite.size.x, -sprite.size.y)},  tex_coords[1]};
            ctx->meshes[atlas_id].mesh[index + 2] = { {body.get_world_point( sprite.size.x,  sprite.size.y)},  tex_coords[2]};
            ctx->meshes[atlas_id].mesh[index + 3] = { {body.get_world_point(-sprite.size.x,  sprite.size.y)},  tex_coords[3]};
            ctx->meshes[atlas_id].sprites += 1;
        }

        for(uint32_t i = 0; i < SPK_MAX_ATLAS; i++) {
            if(!rsrc_mng->atlas_is_in_use(i))
                continue;

            sprite_render_system_ctx_t::atlas_mesh_t& atlas_mesh = ctx->meshes[i];
            sprite_atlas_t* atlas = rsrc_mng->get_atlas(i);

            if(0 < atlas_mesh.sprites) {
                ctx->vertex_buffer.buffer_sub_data(0, 
                    atlas_mesh.sprites * ctx->indexes_per_sprite * sizeof(sprite_render_system_ctx_t::vertex_t), atlas_mesh.mesh.data());
                ctx->vertex_array.bind();

                render_ctx->quad_index_buffer.bind();
                ctx->program.use();
                ctx->program.set_mat4("u_vp", render_ctx->vp);
                ctx->program.set_int("atlas", 0);
                atlas->texture.active_texture(GL_TEXTURE0);
                glDrawElements(GL_TRIANGLES, atlas_mesh.sprites * ctx->vertexes_per_sprite, GL_UNSIGNED_INT, nullptr);   

                atlas_mesh.sprites = 0;
            }
        }
    }

    void sprite_render_cs_init(system_ctx_allocater_t& allocater, flecs::world& world) {
        sfk_register_component(world, sprite_render_system_ctx_t);
        
        auto ctx = allocater.allocate_ctx<sprite_render_system_ctx_t>();

        world.system<comp_b2Body_t, comp_sprite_t>().ctx(ctx).kind(flecs::OnUpdate)
            .iter(sprite_render_system_update);
    }
}