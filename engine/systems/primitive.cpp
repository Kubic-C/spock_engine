#include "primitive.hpp"
#include "state.hpp"
#include "spock.hpp"

namespace spk {
    void primitive_render_system_update(flecs::iter& iter) {
        auto render_ctx = (render_system_t*)state.get_current_renderer();
        auto camera     = state.get_current_camera().get_ref<comp_camera_t>();
        auto ctx        = SPK_GET_CTX(iter, primitive_renderer_t);


        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        ctx->poly_mesh.zero();

        for(auto i : iter) {
            const comp_b2Body_t* body = iter.entity(i).get<comp_b2Body_t>();

            if(body) {
                b2Fixture* fixture = body->body->GetFixtureList();

                for(; fixture; fixture = fixture->GetNext()) {
                    b2Shape* shape = fixture->GetShape();
                    b2Shape::Type type = shape->GetType();

                    switch(type) {
                    case b2Shape::Type::e_polygon:
                        ctx->poly_mesh.add_polygon(body->body, (b2PolygonShape*)shape);
                        break;

                    case b2Shape::Type::e_circle:
                        ctx->render_circle(camera->vp, body->body, (b2CircleShape*)shape);
                        break;

                    case b2Shape::Type::e_edge:
                        ctx->render_edge(camera->vp, body->body, (b2EdgeShape*)shape);
                        break;

                    default:
                        log.log(spk::LOG_TYPE_INFO, "unsupported collider type for render_info renderer");
                    };
                }
            }
        }
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void primitive_render_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world) {
        auto ctx = ctx_alloc.allocate_ctx<primitive_renderer_t>();

        state.get_current_renderer()->rp_add_renderer(0, (base_renderer_t*)ctx);

        ctx->systems.push_back(world.system<comp_body_prim_t>().ctx(ctx)
            .iter(primitive_render_system_update).add<tag_renderer_t>());
    }
}