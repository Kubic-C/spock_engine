#include "primitive_renderer.hpp"
#include "state.hpp"
#include "spock.hpp"
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/rotate_vector.hpp> 

const char* vs_colliders = R"###(
#version 330 core
layout(location = 0) in vec2 a_pos;

out vec4 v_color;

uniform mat4 u_vp;
uniform vec4 color;

void main() {
    gl_Position = u_vp * vec4(a_pos, color.a, 1.0);
    v_color = color;
})###";

const char* fs_colliders = R"###(
#version 330 core
in vec4 v_color;

out vec4 fragment_color;

void main() {
    fragment_color = v_color;
})###";

namespace spk {
    primitive_renderer_t::primitive_renderer_t() {
        const size_t max_size = 1000;

        vertex_buffer.init(GL_ARRAY_BUFFER);
        vertex_buffer.buffer_data(sizeof(glm::vec2) * 4 * max_size, nullptr, GL_DYNAMIC_DRAW);
        
        prim_ctx.init();
        prim_ctx.vertex_layout.add(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0, vertex_buffer);
        prim_ctx.vertex_array.bind_layout(prim_ctx.vertex_layout);

        bool ret = prim_ctx.program.load_shader_str(vs_colliders, fs_colliders);
        spk_assert(ret && "primitive render shaders invalid");
    
        mesh.resize(max_size * 3);

        // b2Draw flags
        SetFlags(e_shapeBit);
    }

    primitive_renderer_t::~primitive_renderer_t() {
        prim_ctx.free();
    }

    void primitive_renderer_t::draw_mesh(const b2Color& color, uint32_t vertices) {
        render_system_t*     renderer = state.get_current_renderer();
        const comp_camera_t* camera   = state.get_current_camera().get<comp_camera_t>();

        prim_ctx.vertex_array.bind();
        prim_ctx.program.use();
        prim_ctx.program.set_mat4("u_vp", camera->vp);
        prim_ctx.program.set_vec4("color", color);

        renderer->quad_index_buffer.bind();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawElements(GL_TRIANGLES, vertices, GL_UNSIGNED_INT, nullptr);   
        glDisable(GL_BLEND);
    }
    
    void primitive_renderer_t::draw_mesh_array(const b2Color& color, uint32_t vertices) {
        render_system_t*     renderer = state.get_current_renderer();
        const comp_camera_t* camera   = state.get_current_camera().get<comp_camera_t>();

        prim_ctx.vertex_array.bind();
        prim_ctx.program.use();
        prim_ctx.program.set_mat4("u_vp", camera->vp);
        prim_ctx.program.set_vec4("color", color);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawArrays(GL_TRIANGLES, 0, vertices);   
        glDisable(GL_BLEND);
    }

    void primitive_renderer_t::draw() {
        b2World*             world    = state.get_current_physics_world();
        
        world->SetDebugDraw(dynamic_cast<b2Draw*>(this));
        world->DebugDraw();
    }

    uint32_t primitive_renderer_t::add_polygon(const b2Vec2* vertices, int32 vertexCount) {
        uint32_t count = (vertexCount / 2) * 3;

        for(uint32_t i = 0; i < vertexCount; i++) {
            mesh[i] = vertices[i];
        }

        vertex_buffer.buffer_sub_data(0, sizeof(glm::vec2) * mesh.size(), mesh.data());

        return count;
    }

    void primitive_renderer_t::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        draw_mesh(color, add_polygon(vertices, vertexCount));
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void primitive_renderer_t::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {        
        draw_mesh(color, add_polygon(vertices, vertexCount));
    }

    uint32_t primitive_renderer_t::add_circle(const b2Vec2& center, float radius) {
        const uint32_t steps = 16;
        const float angle_per_triangle = (2.0f * b2_pi) / steps; // the amount in radians how much we should rotate per triangle
        float angle = angle_per_triangle;

        // we are starting from the right of the circle, so cos = r
        // the Y will equal 0 when cos = r
        glm::vec2 prev = (glm::vec2){ radius, 0.0f };
        prev += (glm::vec2)center;

        uint32_t vertices = steps * 3; // steps is the amount of triangles, each TRI-angle has three vertices

        glm::vec2* section = mesh.data();
        for(uint32_t i = 0; i < steps; i++) {
            // using trig to find a point on the circles edge. cos = x, sin = y.
            glm::vec2 next = (glm::vec2){ (cos(angle) * radius), (sin(angle) * radius) } + (glm::vec2)center;

            section[0] = { center };
            section[1] = { prev   };
            section[2] = { next   };

            angle += angle_per_triangle;
            prev = next;
            section += 3;
        }

        vertex_buffer.buffer_sub_data(0, sizeof(glm::vec2) * vertices, mesh.data());

        return vertices;
    }

    void primitive_renderer_t::DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        draw_mesh_array(color, add_circle(center, radius));
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void primitive_renderer_t::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) {
        draw_mesh_array(color, add_circle(center, radius));
    }

    void primitive_renderer_t::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
        const float thickness = 0.09f;
        const float half_distance = glm::distance((glm::vec2)p1, (glm::vec2)p2) / 2.0f;
        const float angle_from_x = -glm::orientedAngle(glm::normalize((glm::vec2)(p1 - p2)), glm::vec2(1.0f, 0.0f));
        const glm::vec2 midpoint = (glm::vec2)(p1 + p2) / 2.0f;

        // we a AABB, then rotate it by angle from X
        b2Vec2 line[] = {
            glm::rotate(glm::vec2(0.0f - half_distance, 0.0f - thickness), angle_from_x) + midpoint, 
            glm::rotate(glm::vec2(0.0f + half_distance, 0.0f - thickness), angle_from_x) + midpoint, 
            glm::rotate(glm::vec2(0.0f + half_distance, 0.0f + thickness), angle_from_x) + midpoint, 
            glm::rotate(glm::vec2(0.0f - half_distance, 0.0f + thickness), angle_from_x) + midpoint
        };

        DrawSolidPolygon(line, 4, color);
    }

    void primitive_renderer_t::DrawTransform(const b2Transform& xf) {
        const float axis_length = 5.0f;

        DrawSolidCircle(xf.p, 0.2f, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f});

        DrawSegment(xf.p, ((glm::vec2)xf.q.GetXAxis() * axis_length) + xf.p, {1.0f, 0.0f, 0.0f, 1.0f});
        DrawSegment(xf.p, ((glm::vec2)xf.q.GetYAxis() * axis_length) + xf.p, {0.0f, 1.0f, 0.0f, 1.0f});
    }

    void primitive_renderer_t::DrawPoint(const b2Vec2& p, float size, const b2Color& color) {
        draw_mesh_array(color, add_circle(p, size));
    }

}