#include "systems.hpp"

namespace spk {
    void system_ctx_allocater_t::init() {
        spk_trace();
    }

    void system_ctx_allocater_t::free() {
        spk_trace();

        for(system_t* sys : ctx_name_list) {
            delete sys;
        }
    }
}