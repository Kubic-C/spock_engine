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
    
    int           exit_code = 0;
    spk::engine_t engine;

    engine.init();

    spk::physics_world_t* world = engine.get_current_physics_world();
 
    spk::_memory_pool_t<uint16_t> pool(10);

    {
        spk::log.log("Testing single element allocation and deallocation");
        auto element = pool.allocate(1);

        *element = 5;

        pool.deallocate(element);

        spk::log.log("[green, em] no seg faults... [reset]\n");
    }

    {
        spk::log.log("Testing multiple element allocation and deallocation");
        auto element = pool.allocate(5);

        // element[4] = 5;

        pool.deallocate(element);

        spk::log.log("[green, em] no seg faults... [reset]\n");
    }

    {
        spk::log.log("Testing multiple element allocation and deallocation w/ merging headers");
        auto element = pool.allocate(10);

        // element[9] = 5;

        pool.deallocate(element);

        spk::log.log("[green, em] no seg faults... [reset]\n");
    }

    {
        spk::log.log("Testing resize element allocation and deallocation");
        auto element = pool.allocate(1000);

        if(element == nullptr)
            spk::log.log(spk::LOG_TYPE_ERROR, "could not allocate");

        // element[99] = 5;

        pool.deallocate(element);        

        spk::log.log("[green, em] no seg faults... [reset]\n");
    }

    // engine.world.entity().set(
    //     [&](spk::comp_rigid_body_t& rb){
    //         rb = world->add({1.0f, 1.0f});
            
    //         spk::material_t m;
    //         m.density = 1.0f;
    //         m.friction = 1.0f;
    //         m.restitution = 0.5f;

    //         world->add_box_fixture(rb, m, {0.0f, 0.0f}, 0.5f, 0.5f);
    //         world->add_box_fixture(rb, m, {3.0f, 0.0f}, 0.5f, 0.5f);

    //         rb->calculate_max();
    //     }).add<spk::comp_body_prim_t>();

    // engine.world.entity().set(
    //     [&](spk::comp_rigid_body_t& rb){
    //         rb = world->add({0.5f, 0.5f});
    //         rb->velocity.y = 5.0f;
    //     }).add<spk::comp_body_prim_t>();
    
    // engine.world.entity().set(
    //     [&](spk::comp_rigid_body_t& rb){
    //         rb = world->add({0.5f, 0.5f});
    //     }).add<spk::comp_body_prim_t>();

    //world->tree.search(bodies);

    world->set_gravity(0.0f, -0.01f);

    // spk::log.log("%llu", bodies.size());

    exit_code = engine.run();
    engine.free();

    return exit_code;
}