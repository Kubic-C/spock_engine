/*
                            MIT License

                    Copyright (c) 2023 Sawyer Porter

                           refer to license: 
      https://github.com/Kubic-C/spock_engine/blob/master/LICENSE.md
*/

#include "body_renderer.hpp"
#include "renderer.hpp"

namespace spk {
    // void body_renderer_t::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    //     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //     draw_mesh(color, add_polygon(vertices, vertexCount));
    //     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // }

    // void body_renderer_t::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {        
    //     draw_mesh(color, add_polygon(vertices, vertexCount));
    // }

    // uint32_t body_renderer_t::add_circle(const b2Vec2& center, float radius) {
    //     const uint32_t steps = 16;
    //     const float angle_per_triangle = (2.0f * b2_pi) / steps; // the amount in radians how much we should rotate per triangle
    //     float angle = angle_per_triangle;

    //     // we are starting from the right of the circle, so cos = r
    //     // the Y will equal 0 when cos = r
    //     glm::vec2 prev = (glm::vec2){ radius, 0.0f };
    //     prev += (glm::vec2)center;

    //     uint32_t vertices = steps * 3; // steps is the amount of triangles, each TRI-angle has three vertices

    //     glm::vec2* section = mesh.data();
    //     for(uint32_t i = 0; i < steps; i++) {
    //         // using trig to find a point on the circles edge. cos = x, sin = y.
    //         glm::vec2 next = (glm::vec2){ (cos(angle) * radius), (sin(angle) * radius) } + (glm::vec2)center;

    //         section[0] = { center };
    //         section[1] = { prev   };
    //         section[2] = { next   };

    //         angle += angle_per_triangle;
    //         prev = next;
    //         section += 3;
    //     }

    //     vertex_buffer.buffer_sub_data(0, sizeof(glm::vec2) * vertices, mesh.data());

    //     return vertices;
    // }

    // void body_renderer_ttive_renderer_t::DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {
    //     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //     draw_mesh_array(color, add_circle(center, radius));
    //     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // }

    // void body_renderer_ttive_renderer_t::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) {
    //     draw_mesh_array(color, add_circle(center, radius));
    // }

    // void body_renderer_ttive_renderer_t::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
    //     const float thickness = 0.09f;
    //     const float half_distance = glm::distance((glm::vec2)p1, (glm::vec2)p2) / 2.0f;
    //     const float angle_from_x = -glm::orientedAngle(glm::normalize((glm::vec2)(p1 - p2)), glm::vec2(1.0f, 0.0f));
    //     const glm::vec2 midpoint = (glm::vec2)(p1 + p2) / 2.0f;

    //     // we a AABB, then rotate it by angle from X
    //     b2Vec2 line[] = {
    //         glm::rotate(glm::vec2(0.0f - half_distance, 0.0f - thickness), angle_from_x) + midpoint, 
    //         glm::rotate(glm::vec2(0.0f + half_distance, 0.0f - thickness), angle_from_x) + midpoint, 
    //         glm::rotate(glm::vec2(0.0f + half_distance, 0.0f + thickness), angle_from_x) + midpoint, 
    //         glm::rotate(glm::vec2(0.0f - half_distance, 0.0f + thickness), angle_from_x) + midpoint
    //     };

    //     DrawSolidPolygon(line, 4, color);
    // }

    // void body_renderer_ttive_renderer_t::DrawTransform(const b2Transform& xf) {
    //     const float axis_length = 5.0f;

    //     DrawSolidCircle(xf.p, 0.2f, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f});

    //     DrawSegment(xf.p, ((glm::vec2)xf.q.GetXAxis() * axis_length) + xf.p, {1.0f, 0.0f, 0.0f, 1.0f});
    //     DrawSegment(xf.p, ((glm::vec2)xf.q.GetYAxis() * axis_length) + xf.p, {0.0f, 1.0f, 0.0f, 1.0f});
    // }

    uint32_t add_circle(vertex_buffer_t& vertex_buffer, size_t& vertex_count, uint32_t steps, const glm::vec2& center, float radius) {
        const float angle_per_triangle = (2.0f * glm::pi<float>()) / steps; // the amount in radians how much we should rotate per triangle
        float angle = angle_per_triangle;

        // we are starting from the right of the circle, so cos = r
        // the Y will equal 0 when cos = r
        glm::vec2 prev = (glm::vec2){ radius, 0.0f };
        prev += (glm::vec2)center;

        uint32_t count = steps * 3; // steps is the amount of triangles, each TRI-angle has three vertices

        std::vector<glm::vec2> mesh;
        mesh.resize(count);

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

        vertex_buffer.buffer_sub_data(sizeof(glm::vec2) * vertex_count, sizeof(glm::vec2) * count, mesh.data());
        vertex_count += count;

        return count;
    }

    void add_body(vertex_buffer_t& vertex_buffer, kin::rigid_body_t* body, size_t& vertex_count) {
        add_circle(vertex_buffer, vertex_count, 12, body->get_world_pos(), 0.1f);
        add_circle(vertex_buffer, vertex_count, 6, body->pos, 0.1f);
        
        body->iterate_fixtures([&](kin::fixture_t* current){
            kin::box_vertices_t local_vertices = {
                glm::vec2(-current->hw, -current->hh),
                glm::vec2( current->hw, -current->hh),
                glm::vec2( current->hw,  current->hh),
                glm::vec2(-current->hw,  current->hh)
            };

            glm::vec2 box_vertices[4];
                
            for(int i = 0; i < 4; i++) {
                // the shape should be rotated by its relative position and the bodies center of mass
                box_vertices[i] = body->get_world_point(local_vertices[i] + current->pos);
            }

            glm::vec2 vertices[] = {
                box_vertices[0],
                box_vertices[1],
                box_vertices[2],

                box_vertices[0],
                box_vertices[3],
                box_vertices[2]
            };
           
            vertex_buffer.buffer_sub_data(vertex_count * sizeof(glm::vec2), sizeof(vertices), vertices);
            vertex_count += 6;
        });
    }

    void body_renderer_t::mesh(flecs::iter& iter, comp_rigid_body_t* bodies) {
        auto renderer = (body_renderer_t*)render_context().renderers[RENDERER_TYPE_BODY];

        render_context().common.buffer.bind();

        // add meshes
        for(auto i : iter) {
            kin::rigid_body_t* body = bodies[i];

            add_body(render_context().common.buffer, body, renderer->vertex_count);
        }
    }

    void body_renderer_t::render() {
        // setup state
        auto& shader = render_context().shaders[SHADER_TYPE_BASIC];

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        shader.use();
        shader.set_mat4("u_vp", render_context().world_camera);

        // making the most constrast between the back ground and shapes
        shader.set_vec4("color", glm::vec4(glm::vec3(1.0f), 1.0f) - render_context().render.render_passes[0].clear_color);

        render_context().common.array.bind();
        render_context().common.buffer.bind();
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
        glEnableVertexAttribArray(0);

        // render
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        vertex_count = 0;
    }   
}