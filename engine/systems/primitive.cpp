#include "primitive.hpp"
#include "state.hpp"
#include "spock.hpp"

namespace spk {
    void primitive_pre_mesh(flecs::iter& iter) {
        auto poly_mesh = get_ctx<polygon_batch_mesh_t>(iter);

        poly_mesh->zero();
    }

    void primitive_mesh(flecs::iter& iter, comp_rigid_body_t* bodies) {
        spk_trace();

        auto render_ctx = (render_system_t*)state.get_current_renderer();
        auto camera     = state.get_current_camera().get_ref<comp_camera_t>();
        auto poly_mesh  = get_ctx<polygon_batch_mesh_t>(iter);
        
        for(auto i : iter) {
            rigid_body_t& body = *bodies[i];

            if(bodies[i].body == nullptr)
                continue;

            poly_mesh->add_aabb(body.transform.pos + body.max.pos, body.max.aabb);

            for(fixture_t* cur = body.fixtures; cur != nullptr; cur = cur->next) {
                switch(cur->type) {
                    case FIXTURE_TYPE_BOX:
                        poly_mesh->add_aabb(cur->get_world_pos(), cur->def.box);
                        break;

                    default:
                        break;
                }
            }
        }
    }

    void primitive_world_mesh(flecs::iter& iter) {
        spk_trace();

        auto world = state.get_current_physics_world();
        auto poly_mesh  = get_ctx<polygon_batch_mesh_t>(iter);

        world->tree.iterate([&](quad_tree_t* qt){        
            poly_mesh->add_aabb(qt->area.pos, qt->area.aabb);
        });
    }

    void primitive_render_cs_init(system_ctx_allocater_t& ctx_alloc, flecs::world& world) {
        spk_trace();
        
        auto primi_renderer = ctx_alloc.allocate_ctx<primitive_renderer_t>();

        state.get_current_renderer()->rp_add_renderer(0, (base_renderer_t*)primi_renderer);

        world.system().ctx(&primi_renderer->poly_mesh)
            .kind(on_pre_mesh).iter(primitive_pre_mesh);

        world.system<comp_rigid_body_t>().ctx(&primi_renderer->poly_mesh)
            .term<comp_body_prim_t>().kind(on_mesh).iter(primitive_mesh);
        world.system().ctx(&primi_renderer->poly_mesh).kind(on_mesh).iter(primitive_world_mesh);
    }
}