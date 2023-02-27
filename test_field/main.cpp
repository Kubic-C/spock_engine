#include "state.hpp"
#include "spock.hpp"
#include "utility/image_loader.hpp"

enum current_state_e: uint32_t {
    STATE_MENU,
    STATE_LOAD,
    STATE_PLAY,
    STATE_EXIT_PLAY,
    STATE_EXIT
};

MAIN {
    spk_trace();

    spk::log.log("size of quad tree: %llu", sizeof(spk::quad_tree_t));

    spk::log.log("running..");
    int code = 0;

    spk::engine_t engine;

    SPK_DEBUG_ENABLE(spk::DEBUG_FLAGS_ENABLE_ENGINE_LIFETIME);

    engine.init();

    spk::stats.print_ps_stats = true;

    engine.set_target_tps(20);

    spk::physics_world_t* world = engine.get_current_physics_world();   

    for(size_t i = 0; i < 7000; i++) {
        engine.world.entity().set(
            [&](spk::comp_rigid_body_t& rb){
                rb = world->add({fmod((float)rand(), 10.0f) - 5.0f, fmod((float)rand(), 10.0f) - 5.0f});

                spk::material_t m;
                m.density = 1.0f;
                m.friction = 1.0f;
                m.restitution = 0.5f;

                world->add_box_fixture(rb, m, {3.0f, 0.0f}, fmod((float)rand(), 3.0f) + 0.1f, fmod((float)rand(), 3.0f) + 0.1f);

                rb->velocity.x = fmod((float)rand(), 10.0f) - 5.0f;
                rb->velocity.y = fmod((float)rand(), 10.0f) - 5.0f;
            }).add<spk::comp_body_prim_t>();
    }

    code = engine.run();

    engine.free();

    return code;
}