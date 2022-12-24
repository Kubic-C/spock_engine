#include "primitive_render.hpp"
#include "../state.hpp"
#include <glm/gtx/rotate_vector.hpp> 

const char* vs_colliders = R"###(
#version 330 core
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec3 a_color;

out vec3 v_color;

uniform mat4 u_vp;

void main() {
    gl_Position = u_vp * vec4(a_pos, 0.0, 1.0);
    v_color = a_color;
})###";

const char* fs_colliders = R"###(
#version 330 core
in vec3 v_color;

out vec4 fragment_color;

void main() {
    fragment_color = vec4(v_color.xyz, 1.0);
})###";

namespace spk {
    void primitive_render_system_ctx_t::render_box(glm::mat4& vp, sfk::static_index_buffer_t& ind, 
        comp_primitive_render_t* render_info, const comp_box_t* box) {
        const uint32_t quad_vert_count = 4;
        const uint32_t quad_index_count = 6;
        
        mesh[0].pos = { box->position.x,               box->position.y };
        mesh[1].pos = { box->position.x + box->size.x, box->position.y };
        mesh[2].pos = { box->position.x + box->size.x, box->position.y + box->size.y };
        mesh[3].pos = { box->position.x,               box->position.y + box->size.y };

        for(uint32_t j = 0; j < quad_vert_count; j++)  {
            mesh[j].color = render_info->color;
        }

        vertex_buffer.buffer_sub_data(0, quad_vert_count * sizeof(vertex_t), mesh.data());
        vertex_array.bind();

        ind.bind();
        program.use();
        program.set_mat4("u_vp", vp);
        glDrawElements(GL_TRIANGLES, quad_index_count, GL_UNSIGNED_INT, nullptr);   
    }

    void primitive_render_system_ctx_t::render_polygon(glm::mat4& vp, sfk::static_index_buffer_t& ind, 
        comp_primitive_render_t* render_info, b2Body* body, b2PolygonShape* polygon) {
        // this will get the amount of indexes to use, so if quad has 4 verts: 4 / 2 = 2 -> 2 * 3 = 6 (indexes)
        int32_t index_count = (polygon->m_count / 2) * 3; // special case: 3 works because of integer division black magic

        for(int32 j = 0; j < polygon->m_count; j++)  {
            mesh[j].pos = { 
                body->GetWorldPoint(polygon->m_vertices[j]).x, body->GetWorldPoint(polygon->m_vertices[j]).y };
            mesh[j].color = render_info->color;
        }

        vertex_buffer.buffer_sub_data(0, polygon->m_count * sizeof(vertex_t), mesh.data());
        vertex_array.bind();

        ind.bind();
        program.use();
        program.set_mat4("u_vp", vp);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);   
    }

    void primitive_render_system_ctx_t::render_circle(glm::mat4& vp, comp_primitive_render_t* primitve, b2Body* body, b2CircleShape* circle) {
        const glm::vec2 position = sfk::to_glm_vec2(body->GetPosition());
        const uint32_t steps = 8;
        const float r = circle->m_radius;
        const float c = 2.0f * r * b2_pi;
        const float c_sect = (2.0f * b2_pi) / steps; // returns in radians how much we should rotate
        float body_angle = body->GetAngle();
        float angle = c_sect;

        // we are starting from the right of the circle, so cos = r
        // the Y will equal 0 when cos = r
        glm::vec2 prev = (glm::vec2){ r, 0.0f };
        prev = glm::rotate(prev, body_angle) + position;

        uint32_t vertices = steps * 3; // steps is the amount of triangles, each TRI-angle has three vertices

        vertex_t* section = mesh.data();
        for(uint32_t i = 0; i < steps; i++) {
            glm::vec2 next = (glm::vec2){ (cos(angle) * r), (sin(angle) * r) };

            // add rotation
            next = glm::rotate(next, body_angle) + position;

            section[0] = { position,  primitve->color }; // center
            section[1] = { prev,      primitve->color };
            section[2] = { next,      primitve->color };

            angle += c_sect;
            prev = next;
            section += 3;
        }

        vertex_buffer.buffer_sub_data(0, vertices * sizeof(vertex_t), mesh.data());
        vertex_array.bind();

        program.use();
        program.set_mat4("u_vp", vp);
        glDrawArrays(GL_TRIANGLES, 0, vertices);
    }

    void primitive_render_system_ctx_t::render_edge(glm::mat4& vp, comp_primitive_render_t* info, b2Body* body, b2EdgeShape* edge) {
        uint32_t vertices = 2;

        mesh[0] = { sfk::to_glm_vec2(body->GetWorldPoint(edge->m_vertex0)), info->color };
        mesh[1] = { sfk::to_glm_vec2(body->GetWorldPoint(edge->m_vertex1)), info->color };
    
        vertex_buffer.buffer_sub_data(0, vertices * sizeof(vertex_t), mesh.data());
        vertex_array.bind();

        program.use();
        program.set_mat4("u_vp", vp);
        glDrawArrays(GL_LINES, 0, vertices);
    }

    void primitive_render_system_ctx_t::init() {
        vertex_array.init();
        vertex_array.bind();

        vertex_buffer.init(GL_ARRAY_BUFFER);
        vertex_buffer.buffer_data(sizeof(vertex_t) * 4 * 100, nullptr, GL_DYNAMIC_DRAW);
        vertex_layout.add(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0, vertex_buffer);
        vertex_layout.add(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, sizeof(float) * 2, vertex_buffer);
        vertex_array.bind_layout(vertex_layout);

        program.init();
        bool ret = program.load_shader_str(vs_colliders, fs_colliders);
        sfk_assert(ret && "primitive render shaders invalid");
    
        mesh.resize(100 * 3);
    }

    void primitive_render_system_ctx_t::free() {
        vertex_array.free();
        vertex_buffer.free();
        program.free();
    }

    void primitive_render_system_update(flecs::iter& iter, comp_primitive_render_t* c_primi_render) {
        auto render_ctx = state.get_current_renderer().get_ref<render_system_ctx_t>(); // this is a safe op as render system only has pre and post update
        auto camera = state.get_current_camera().get_ref<comp_camera_t>();
        auto ctx = SPK_GET_CTX_REF(iter, primitive_render_system_ctx_t);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        for(auto i : iter) {
            comp_primitive_render_t* render_info = &c_primi_render[i];
            const comp_b2Body_t* comp_body = iter.entity(i).get<comp_b2Body_t>();
            const comp_box_t* box = iter.entity(i).get<comp_box_t>();
            const comp_tilebody_t* tilebody = iter.entity(i).get<comp_tilebody_t>();
            
            b2Body* body = nullptr;

            if(comp_body)
                body = comp_body->body;
            else if(tilebody)
                body = tilebody->body;

            if(body) {
                b2Fixture* fixture = body->GetFixtureList();

                for(; fixture; fixture = fixture->GetNext()) {
                    b2Shape* shape = fixture->GetShape();
                    b2Shape::Type type = shape->GetType();

                    switch(type) {
                    case b2Shape::Type::e_polygon:
                        ctx->render_polygon(camera->vp, render_ctx->quad_index_buffer,
                            render_info, body, (b2PolygonShape*)shape);
                        break;

                    case b2Shape::Type::e_circle:
                        ctx->render_circle(camera->vp, render_info, body, (b2CircleShape*)shape);
                        break;

                    case b2Shape::Type::e_edge:
                        ctx->render_edge(camera->vp, render_info, body, (b2EdgeShape*)shape);
                        break;

                    default:
                        sfk::log.log(sfk::LOG_TYPE_INFO, "unsupported collider type for render_info renderer");
                    };
                }
            } else if(box) {
                ctx->render_box(camera->vp, render_ctx->quad_index_buffer, render_info, box);
            } 
        }
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void primitive_render_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world) {
        flecs::entity* ctx;

        world.component<comp_primitive_render_t>();
        sfk_register_component(world, primitive_render_system_ctx_t);

        ctx = ctx_alloc.allocate_ctx<primitive_render_system_ctx_t>();

        world.system<comp_primitive_render_t>().ctx(ctx).kind(flecs::OnUpdate)
            .iter(primitive_render_system_update);
        
    }
}