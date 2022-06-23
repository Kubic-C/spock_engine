#pragma once

#include "../systems.hpp"

namespace spk {
    struct tile_grid {
        bool station;

    };

    struct tile {
        uint32_t type;

    };

    class tile_manager_tt : system_tt {
    public:

        void init(void* data);
        void update(flecs::world& world);
        void tick(flecs::world& world);
        void free();

    private:    

    };
}