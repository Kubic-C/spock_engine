#pragma once

#include "base_renderer.hpp"

/* quick and dirty debug rendering for primitive shapes */

namespace spk {
    struct comp_body_prim_t {};

    class primitive_renderer_t : public base_renderer_t, public b2Draw {
    public:
        primitive_renderer_t();
        ~primitive_renderer_t();

        void draw() override;

        void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
        void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
        void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;
        void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;
        void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
        void DrawTransform(const b2Transform& xf) override;
        void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;

    private:
        uint32_t add_polygon(const b2Vec2* vertices, int32 vertexCount);
        uint32_t add_circle(const b2Vec2& center, float radius);
        void draw_mesh(const b2Color& color, uint32_t vertices);
        void draw_mesh_array(const b2Color& color, uint32_t vertices);

        render_ctx_t               prim_ctx;
        vertex_buffer_t            vertex_buffer;
        std::vector<glm::vec2> mesh;
    };
}