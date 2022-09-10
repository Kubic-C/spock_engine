#include "tile_body.hpp"
#include "../systems/physics.hpp"

namespace spk {

    void tile_dictionary_t::init() {
        render.init();
        physics.init();
        texture.init();
    }
    
    void tile_dictionary_t::free() {
        texture.free();
        render.free();
        physics.free();
    }

    sfk::key_t tile_dictionary_t::init_tile(sfk::key_t key) {
        bool ret;

        ret = render.register_key(key);
        if(!ret) {
            sfk::log.log(sfk::LOG_TYPE_INFO, "failed to init render data of tile(%u)", key);
            return sfk::MAX_KEY;
        }

        ret = physics.register_key(key);
        if(!ret) {
            render.letgo(key);
            sfk::log.log(sfk::LOG_TYPE_INFO, "failed to init physics data of tile(%u)", key);
            return sfk::MAX_KEY;
        }

        physics[key].shape_def.SetAsBox(0.5f, 0.5f);
        physics[key].fix_def.shape = &physics[key].shape_def;

        return key;
    }

    void tile_dictionary_t::free_tile(sfk::key_t key) {
        render.letgo(key);
        physics.letgo(key);
    }

    void tile_body_t::init(b2World* world, b2BodyType type, tile_dictionary_t* dict) {
        sfk_assert(type != b2_kinematicBody && "kinematic body types are not allowed within tile bodies");

        b2BodyDef body_def;
        body_def.type = type;
        body_def.position = b2Vec2(0.0f, 0.0f);
        body = world->CreateBody(&body_def);
        
        sfk_assert(body);

        this->dict = dict;

        for(auto& arr : tiles) {
            arr.fill(tile_t{.type_id = sfk::MAX_KEY, .fixture = nullptr});
        }
    }

    void tile_body_t::create_tile(size_t x, size_t y, sfk::key_t type) {
        sfk_assert(x < max_tile_width && y < max_tile_height);
        b2Fixture* fix = body->CreateFixture(&dict->physics[type].fix_def);

        tiles[x][y].fixture = fix;
        tiles[x][y].type_id = type;

        ((b2PolygonShape*)fix->GetShape())->m_vertices[0] += b2Vec2(x, y);
        ((b2PolygonShape*)fix->GetShape())->m_vertices[1] += b2Vec2(x, y);
        ((b2PolygonShape*)fix->GetShape())->m_vertices[2] += b2Vec2(x, y);
        ((b2PolygonShape*)fix->GetShape())->m_vertices[3] += b2Vec2(x, y);
    }

    void tile_body_t::set_tile(size_t x, size_t y, sfk::key_t type) {
        sfk_assert(x < max_tile_width && y < max_tile_height);

        body->DestroyFixture(tiles[x][y].fixture);
        tiles[x][y].fixture = body->CreateFixture(&dict->physics[type].fix_def);
        tiles[x][y].type_id = type;
    }

    void tile_body_t::destroy_tile(size_t x, size_t y) {
        sfk_assert(x < max_tile_width && y < max_tile_height);

        body->DestroyFixture(tiles[x][y].fixture);
        tiles[x][y].type_id = sfk::MAX_KEY;
    }

    void tile_body_t::free(b2World* world) {
        world->DestroyBody(body);
    }

    void add_tile_body_components(scene_t& scene) {
        scene.world.component<tile_body_t>().on_add([&](flecs::entity e, tile_body_t& comp){
            comp.body = NULL;
        }).on_remove([&](flecs::entity e, tile_body_t& comp){
            if(comp.body != NULL)
                comp.free(scene.physics_scene->world);
        });
    }
}