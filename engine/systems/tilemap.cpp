#include "tilemap.hpp"

namespace spk {
    void tilemap_system_t::init(scene_t& scene, void* data) {
    }

    void tilemap_system_t::update(scene_t& scene, float deltatime) {
        q = scene.world.query<active_t, spk::tilemap_t>();
    
        q.iter([&](flecs::iter& it, active_t* c_active, spk::tilemap_t* c_tm) {
            uint32_t it_count = 0;

            for(auto i : it) {
                sfk_assert(it_count <= 1);
                spk::tilemap_t& tm = c_tm[i];
                
                it_count++;
            }
        });
    }

    void tilemap_system_t::tick(scene_t& scene, float deltatime) {
        q = scene.world.query<active_t, spk::tilemap_t>();
    
        q.iter([&](flecs::iter& it, active_t* c_active, spk::tilemap_t* c_tm) {
            uint32_t it_count = 0;

            for(auto i : it) {
                sfk_assert(it_count <= 1);
                spk::tilemap_t& tm = c_tm[i];
                
                it_count++;
            }
        });
    }

    void tilemap_system_t::free(scene_t& scene) {

    }
}