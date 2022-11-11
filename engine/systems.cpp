#include "systems.hpp"

namespace spk {
    void system_ctx_allocater_t::init(flecs::world& world) {
        this->world = &world;

        ctx_name_list.init();
    }

    void system_ctx_allocater_t::free() {
        ctx_name_list.free();
    }
}