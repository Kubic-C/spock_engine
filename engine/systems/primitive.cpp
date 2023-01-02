#include "primitive.hpp"
#include "state.hpp"
#include "spock.hpp"

namespace spk {
    void primitive_pre_mesh(flecs::iter& iter) {
        auto poly_mesh = SPK_GET_CTX(iter, polygon_batch_mesh_t);

        poly_mesh->zero();
    }

    void primitive_mesh(flecs::iter& iter, comp_b2Body_t* bodies) {
        auto render_ctx = (render_system_t*)state.get_current_renderer();
        auto camera     = state.get_current_camera().get_ref<comp_camera_t>();
        auto poly_mesh  = SPK_GET_CTX(iter, polygon_batch_mesh_t);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        for(auto i : iter) {
            comp_b2Body_t* body = &bodies[i];

            b2Fixture* fixture = body->body->GetFixtureList();

            for(; fixture; fixture = fixture->GetNext()) {
                b2Shape* shape = fixture->GetShape();
                b2Shape::Type type = shape->GetType();

                switch(type) {
                case b2Shape::Type::e_polygon:
                    poly_mesh->add_polygon(body->body, (b2PolygonShape*)shape);
                    break;

                case b2Shape::Type::e_circle:
                    spk_assert(false);
                    //ctx->render_circle(camera->vp, body->body, (b2CircleShape*)shape);
                    break;

                case b2Shape::Type::e_edge:
                    spk_assert(false);
                    //ctx->render_edge(camera->vp, body->body, (b2EdgeShape*)shape);
                    break;

                default:
                    log.log(spk::LOG_TYPE_INFO, "unsupported collider type for render_info renderer");
                };
            }
        }
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void primitive_render_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world) {
        auto primi_renderer = ctx_alloc.allocate_ctx<primitive_renderer_t>();

        state.get_current_renderer()->rp_add_renderer(0, (base_renderer_t*)primi_renderer);

        world.system().ctx(&primi_renderer->poly_mesh)
            .kind(on_pre_mesh).iter(primitive_pre_mesh);
        world.system<comp_b2Body_t>().ctx(&primi_renderer->poly_mesh)
            .term<comp_body_prim_t>().kind(on_mesh).iter(primitive_mesh);
    }
}