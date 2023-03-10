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

    spk::log.log("running..");
    int code = 0;

    spk::engine_t engine;

    SPK_DEBUG_ENABLE(spk::DEBUG_FLAGS_ENABLE_ENGINE_LIFETIME);

    engine.init();
    b2World* world = engine.get_current_physics_world();

    for(size_t i = 0; i < 100; i++) {
        engine.world.entity().set([&](spk::comp_rigid_body_t& rb) {
            b2BodyDef def;
            def.type = b2_dynamicBody;
            rb = world->CreateBody(&def);

            b2CircleShape shape;
            float random = rand();
            random *= 0.01f;

            shape.m_radius = fmod(random, 1.9f) + 0.1f;

            b2FixtureDef fdef;
            fdef.shape = &shape;
            fdef.density = 1.0f;
            rb->CreateFixture(&fdef);
        });
    }
    
    engine.world.entity().set([&](spk::comp_rigid_body_t& rb) {
        b2BodyDef def;
        def.type = b2_dynamicBody;
        def.angularVelocity = 15.0f;
        def.position.y = 10.0f;
        rb = world->CreateBody(&def);

        b2PolygonShape shape;
        shape.SetAsBox(0.5f, 0.5f);

        b2FixtureDef fdef;
        fdef.shape = &shape;
        fdef.density = 1.0f;
        rb->CreateFixture(&fdef);
    }).add<spk::comp_character_controller_t>();


    engine.world.entity().set([&](spk::comp_rigid_body_t& rb) {
        b2BodyDef def;
        def.type = b2_staticBody;
        def.position.y = -10.0f;
        rb = world->CreateBody(&def);

        b2PolygonShape shape;
        shape.SetAsBox(100.0f, 0.5f);

        b2FixtureDef fdef;
        fdef.shape = &shape;
        fdef.density = 1.0f;
        rb->CreateFixture(&fdef);
    }).add<spk::comp_character_controller_t>();

    code = engine.run();

    engine.free();

    return code;
}