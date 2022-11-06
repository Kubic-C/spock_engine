#pragma once

#include "../systems.hpp"
#include "../utility/tilemap.hpp"
#include "../utility/tags.hpp"

namespace spk {

    class tilemap_system_t : public system_t {
    public:
        void init(scene_t& scene, void* data);
        void update(scene_t& scene, float deltatime);
        void tick(scene_t& scene, float deltatime);
        void free(scene_t& scene);
    
    private:
        flecs::query<active_t, spk::tilemap_t> q;    
    };
}