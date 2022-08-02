#pragma once

#include "../systems.hpp"
#include "../utility/callback.hpp"

namespace spk {
    class input_tt : public system_tt {
    public:

        input_tt();
        void init(scene_tt& scene, void* data);
        void update(scene_tt& scene, float deltatime);
        void tick(scene_tt& scene, float deltatime);
        void free();

    private:
    };
}