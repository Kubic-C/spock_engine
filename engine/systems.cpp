#include "systems.hpp"

namespace spk {
    void system_ctx_allocater_t::init() {
        ctx_name_list.init();
    }

    void system_ctx_allocater_t::free() {
        for(system_t* sys : ctx_name_list) {
            sys->free();
        }

        ctx_name_list.free();
    }
}